#pragma once
#include "VulkanContext.h"
#include "VulkanCommandSystem.h"
#include "VulkanRenderCommand.h"
#include <Memory>
class SimpleMesh;
class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanContext* context);
	virtual ~VulkanRenderPass() = default;
	virtual void createRenderPass() = 0; // 创建渲染通道
	virtual void createFramebuffers() = 0; // 创建帧缓冲区
	virtual void cleanup() = 0; // 清理资源
	virtual VkRenderPass getRenderPass() const { return m_renderPass; } // 获取渲染通道
	virtual const std::vector<VkFramebuffer>& getFramebuffers() { return m_vecFramebuffers; } // 获取交换链帧缓冲区
	virtual VkExtent2D getPassExtent() = 0;
protected:
	VulkanContext* m_context; // Vulkan 上下文
	VkRenderPass m_renderPass; // 渲染通道
	std::vector<VkFramebuffer> m_vecFramebuffers;
};

class VulkanGraphicsPipeline
{
public:
	VulkanGraphicsPipeline(VulkanContext* context, VulkanRenderPass* pass, VkExtent2D extent);
	virtual ~VulkanGraphicsPipeline() = default;
	virtual void createPipeline() = 0; // 创建管线
	virtual void cleanup() = 0; // 清理资源
	virtual VkPipeline getGraphicsPipeline() const { return m_graphicsPipeline; } // 获取图形管线
	virtual VulkanRenderPass* getRenderPass() const { return m_renderPass; }

	virtual void beginRenderPass(VulkanCommandSystem* commandSystem, uint32_t imageIndex) = 0;
	virtual void endRenderPass(VulkanCommandSystem* commandSystem) = 0;
	virtual void bindPipeline(VulkanCommandSystem* commandSystem) = 0;
	virtual void executeCommands(VulkanCommandSystem* commandSystem, const std::vector<std::unique_ptr<IRenderCommand>>& commands) = 0;


protected:
	VkShaderModule createShaderModule(const std::vector<char>& code);
protected:
	VulkanContext* m_context; // Vulkan 上下文
	VulkanRenderPass* m_renderPass; // 渲染通道
	VkPipelineLayout m_pipelineLayout; // 管线布局
	VkPipeline m_graphicsPipeline; // 图形管线
	VkExtent2D m_extent; // 渲染区域
};

