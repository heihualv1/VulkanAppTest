#include "VulkanPipeline.h"
#include <stdexcept>
VulkanRenderPass::VulkanRenderPass(VulkanContext* context)
	: m_context(context), m_renderPass(VK_NULL_HANDLE)
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanContext* context, VulkanRenderPass* pass, VkExtent2D extent)
	: m_context(context), m_renderPass(pass), m_pipelineLayout(VK_NULL_HANDLE), m_graphicsPipeline(VK_NULL_HANDLE), m_extent(extent)
{
}

VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_context->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}
