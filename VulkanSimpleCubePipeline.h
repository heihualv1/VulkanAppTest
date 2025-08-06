#pragma once
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

class VulkanSimpleMeshRenderPass : public VulkanRenderPass
{
public:
	VulkanSimpleMeshRenderPass(VulkanContext* context, VulkanSwapChain* swapchain);
	~VulkanSimpleMeshRenderPass() override;
	virtual void createRenderPass() override; // 创建简单渲染通道
	virtual void createFramebuffers() override; // 创建帧缓冲区
	virtual void cleanup() override; // 清理资源
	virtual VkExtent2D getPassExtent();
private:
	VulkanSwapChain* m_swapchain; // 交换链

};

class VulkanSimpleMeshPipeline : public VulkanGraphicsPipeline
{
public:
	VulkanSimpleMeshPipeline(VulkanContext* context, VulkanRenderPass* pass, VkExtent2D extent);
	~VulkanSimpleMeshPipeline() override;
	virtual void createPipeline() override; // 创建简单管线
	virtual void cleanup() override; // 清理资源
	virtual void beginRenderPass(VulkanCommandSystem* commandSystem, uint32_t imageIndex);
	virtual void endRenderPass(VulkanCommandSystem* commandSystem);
	virtual void bindPipeline(VulkanCommandSystem* commandSystem);
	virtual void executeCommands(VulkanCommandSystem* commandSystem, const std::vector<std::unique_ptr<IRenderCommand>>& commands);
};

