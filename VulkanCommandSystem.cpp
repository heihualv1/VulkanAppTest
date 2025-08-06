#include "VulkanCommandSystem.h"
#include <stdexcept>
#include "VulkanPipeline.h"
VulkanCommandSystem::VulkanCommandSystem(VulkanContext* context)
{
	m_context = context;
	m_framesInFlight = context->getMaxFramesInFlight();
	// 初始化命令池、命令缓冲区和同步对象
	createCommandPools();
	createCommandBuffers();
	createSyncObjects();
}

VulkanCommandSystem::~VulkanCommandSystem()
{
	// 销毁同步对象
	for (auto& sync : m_syncObjects) {
		vkDestroySemaphore(m_context->getDevice(), sync.imageAvailable, nullptr);
		vkDestroySemaphore(m_context->getDevice(), sync.renderFinished, nullptr);
		vkDestroyFence(m_context->getDevice(), sync.inFlight, nullptr);
	}
	m_syncObjects.clear();
	// 销毁命令缓冲区
	for (auto& cmdBuffers : m_commandBuffers) {
		if (cmdBuffers.primary != VK_NULL_HANDLE) {
			vkFreeCommandBuffers(m_context->getDevice(), m_graphicsCommandPool, 1, &cmdBuffers.primary);
		}
	}
	m_commandBuffers.clear();
	// 销毁命令池
	vkDestroyCommandPool(m_context->getDevice(), m_graphicsCommandPool, nullptr);
}

void VulkanCommandSystem::Init()
{
	// 初始化命令池、命令缓冲区和同步对象
	createCommandPools();
	createCommandBuffers();
	createSyncObjects();
	m_currentFrameIndex = 0; // 重置当前帧索引
}

void VulkanCommandSystem::beginFrame()
{
	vkWaitForFences(m_context->getDevice(), 1, &m_syncObjects[m_currentFrameIndex].inFlight, VK_TRUE, UINT64_MAX);
	vkResetFences(m_context->getDevice(), 1, &m_syncObjects[m_currentFrameIndex].inFlight);

	vkResetCommandBuffer(getPrimaryCommandBuffer(), 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(getPrimaryCommandBuffer(), &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}

void VulkanCommandSystem::submitFrame()
{
	if (vkEndCommandBuffer(getPrimaryCommandBuffer()) != VK_SUCCESS) {
		throw std::runtime_error("failed to end command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// 等待图像可用信号量
	VkSemaphore waitSemaphores[] = { currentImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	// 设置要提交的命令缓冲区
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &getPrimaryCommandBuffer();

	// 设置渲染完成信号量
	VkSemaphore signalSemaphores[] = { currentRenderFinishedSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_context->getGraphicQueue(), 1, &submitInfo, currentFrameFence()) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
}

void VulkanCommandSystem::waitIdle() const
{
	if (vkQueueWaitIdle(m_context->getGraphicQueue()) != VK_SUCCESS) {
		throw std::runtime_error("failed to wait for queue idle!");
	}
}

void VulkanCommandSystem::advanceFrame()
{
	m_currentFrameIndex = (m_currentFrameIndex + 1) % m_framesInFlight;
}

void VulkanCommandSystem::resetFrame()
{
	// 重置当前帧的命令缓冲区和同步对象
	vkResetCommandBuffer(getPrimaryCommandBuffer(), 0);
	vkResetFences(m_context->getDevice(), 1, &currentFrameFence());
	m_currentFrameIndex = (m_currentFrameIndex + 1) % m_framesInFlight; // 更新当前帧索引
}

void VulkanCommandSystem::createCommandPools()
{
	// 创建图形命令池
	QueueFamilyIndices queueFamilyIndices = m_context->getQueueFamilies();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(m_context->getDevice(), &poolInfo, nullptr, &m_graphicsCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

void VulkanCommandSystem::createCommandBuffers()
{
	for (uint32_t i = 0; i < m_framesInFlight; ++i) {
		FrameCommandBuffers cmdBuffers;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_graphicsCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;
		if (vkAllocateCommandBuffers(m_context->getDevice(), &allocInfo, &cmdBuffers.primary) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate primary command buffer!");
		}
		m_commandBuffers.push_back(cmdBuffers);
	}
}

void VulkanCommandSystem::createSyncObjects()
{
	for (uint32_t i = 0; i < m_framesInFlight; ++i) {
		FrameSyncObjects syncObjects;
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(m_context->getDevice(), &semaphoreInfo, nullptr, &syncObjects.imageAvailable) != VK_SUCCESS ||
			vkCreateSemaphore(m_context->getDevice(), &semaphoreInfo, nullptr, &syncObjects.renderFinished) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // 初始状态为信号
		if (vkCreateFence(m_context->getDevice(), &fenceInfo, nullptr, &syncObjects.inFlight) != VK_SUCCESS) {
			throw std::runtime_error("failed to create fence!");
		}
		m_syncObjects.push_back(syncObjects);
	}
}
