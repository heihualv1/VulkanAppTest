#include "VulkanSimpleCubePipeline.h"
#include "VulkanHelper.h"
#include "SimpleMeshVertex.h"
#include <stdexcept>

VulkanSimpleMeshRenderPass::VulkanSimpleMeshRenderPass(VulkanContext* context, VulkanSwapChain* swapchain)
	: VulkanRenderPass(context), m_swapchain(swapchain)
{
	createRenderPass();
	createFramebuffers();
}

VulkanSimpleMeshRenderPass::~VulkanSimpleMeshRenderPass()
{
	cleanup();
}

void VulkanSimpleMeshRenderPass::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapchain->getSwapChainFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(m_context->getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanSimpleMeshRenderPass::createFramebuffers()
{
	m_vecFramebuffers.resize(m_swapchain->getSwapChainImageViews().size());
	for (size_t i = 0; i < m_swapchain->getSwapChainImageViews().size(); i++) {
		VkImageView attachments[] = {
			m_swapchain->getSwapChainImageViews()[i]
		};
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapchain->getSwapChianExtent().width;
		framebufferInfo.height = m_swapchain->getSwapChianExtent().height;
		framebufferInfo.layers = 1;
		if (vkCreateFramebuffer(m_context->getDevice(), &framebufferInfo, nullptr, &m_vecFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanSimpleMeshRenderPass::cleanup()
{
	for (auto framebuffer : m_vecFramebuffers)
	{
		vkDestroyFramebuffer(m_context->getDevice(), framebuffer, nullptr);
	}
	vkDestroyRenderPass(m_context->getDevice(), m_renderPass, nullptr);
}

VkExtent2D VulkanSimpleMeshRenderPass::getPassExtent()
{
	return m_swapchain->getSwapChianExtent();
}

VulkanSimpleMeshPipeline::VulkanSimpleMeshPipeline(VulkanContext* context, VulkanRenderPass* pass, VkExtent2D extent)
	: VulkanGraphicsPipeline(context, pass, extent)
{
	createPipeline();
}

VulkanSimpleMeshPipeline::~VulkanSimpleMeshPipeline()
{
	cleanup();
}

void VulkanSimpleMeshPipeline::createPipeline()
{
	auto vertShaderCode = VulkanHelper::readFile("Shader/CubeVert.spv");
	auto fragShaderCode = VulkanHelper::readFile("Shader/CubeFrag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = m_extent.width;
	viewport.height = m_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_extent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	//光栅化
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	//多重采样
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	//深度模板测试


	//颜色混合
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	//管线布局
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(m_context->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;

	pipelineInfo.layout = m_pipelineLayout;

	pipelineInfo.renderPass = m_renderPass->getRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	vkCreateGraphicsPipelines(m_context->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);

	vkDestroyShaderModule(m_context->getDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(m_context->getDevice(), vertShaderModule, nullptr);
}

void VulkanSimpleMeshPipeline::cleanup()
{
	vkDestroyPipelineLayout(m_context->getDevice(), m_pipelineLayout, nullptr);
	vkDestroyPipeline(m_context->getDevice(), m_graphicsPipeline, nullptr);
}

void VulkanSimpleMeshPipeline::beginRenderPass(VulkanCommandSystem* commandSystem, uint32_t imageIndex)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->getRenderPass();
	renderPassInfo.framebuffer = m_renderPass->getFramebuffers()[imageIndex]; // 使用当前帧缓冲区
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_renderPass->getPassExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	// 开始渲染通道
	vkCmdBeginRenderPass(commandSystem->getPrimaryCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanSimpleMeshPipeline::endRenderPass(VulkanCommandSystem* commandSystem)
{
	vkCmdEndRenderPass(commandSystem->getPrimaryCommandBuffer());
}

void VulkanSimpleMeshPipeline::bindPipeline(VulkanCommandSystem* commandSystem)
{
	// 设置动态视口
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_renderPass->getPassExtent().width);
	viewport.height = static_cast<float>(m_renderPass->getPassExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandSystem->getPrimaryCommandBuffer(), 0, 1, &viewport);

	// 设置动态裁剪区域
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_renderPass->getPassExtent();
	vkCmdSetScissor(commandSystem->getPrimaryCommandBuffer(), 0, 1, &scissor);

	// 绑定管线
	vkCmdBindPipeline(commandSystem->getPrimaryCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, getGraphicsPipeline());
}

void VulkanSimpleMeshPipeline::executeCommands(VulkanCommandSystem* commandSystem, const std::vector<std::unique_ptr<IRenderCommand>>& commands)
{
	for (auto& command : commands) {
		command->execute(commandSystem->getPrimaryCommandBuffer());
	}
}