#include "VulkanApp.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "VulkanSimplePipeline.h"
#include "VulkanSimpleCubePipeline.h"
#include "SimpleMeshVertex.h"
#include <memory>
void VulkanApp::run()
{
	initWindow();
	initVulkan();
	mainLoop();    // 主循环
	cleanup();
}

void VulkanApp::initWindow()
{
	glfwInit(); // 初始化 GLFW

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // 禁用 OpenGL 上下文

	// 创建窗口
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this); // 将当前对象与窗口关联
	glfwSetFramebufferSizeCallback(window, &VulkanApp::framebufferResizeCallback); // 设置窗口大小调整回调
}

void VulkanApp::initVulkan()
{
	m_context = new VulkanContext(window);
	m_commandSystem = new VulkanCommandSystem(m_context); // 创建 Vulkan 命令系统，支持双缓冲
	m_swapChain = new VulkanSwapChain(m_context); // 创建 Vulkan 交换链，管理帧缓冲区
	m_renderPass = new VulkanSimpleMeshRenderPass(m_context, m_swapChain);
	m_graphicsPipeline = new VulkanSimpleMeshPipeline(m_context, m_renderPass, m_swapChain->getSwapChianExtent());
	SimpleMesh* mesh1 = new SimpleMesh(m_context, sampleVertices1, indices1); // 创建第一个简单网格
	SimpleMesh* mesh2 = new SimpleMesh(m_context, sampleVertices2, indices2); // 创建第二个简单网格
	mesh1->createVertexBuffer(m_commandSystem);
	mesh2->createVertexBuffer(m_commandSystem);
	m_meshes.push_back(mesh1); // 将第一个网格添加到网格列表
	m_meshes.push_back(mesh2); // 将第二个网格添加到网格列表
}

void VulkanApp::mainLoop()
{
	while (!glfwWindowShouldClose(window)) { // 检查窗口是否关闭
		glfwPollEvents(); // 处理窗口事件

		drawFrame();
	}
	vkDeviceWaitIdle(m_context->getDevice()); // 等待设备空闲
}

void VulkanApp::drawFrame()
{
	std::vector<std::unique_ptr<IRenderCommand>> commands;
	for (SimpleMesh* mesh : m_meshes)
	{
		mesh->createVertexBuffer(m_commandSystem);
		mesh->createIndexBuffer(m_commandSystem);
		commands.push_back(mesh->createRenderCommand());
	}
	
	// 1. 开始记录命令缓冲区
	m_commandSystem->beginFrame();

	// 2. 获取下一帧图像索引（等待图像可用信号量）
	uint32_t imageIndex;
	m_swapChain->getImageIndex(m_commandSystem, imageIndex);


	m_graphicsPipeline->beginRenderPass(m_commandSystem, imageIndex);
	m_graphicsPipeline->bindPipeline(m_commandSystem);
	m_graphicsPipeline->executeCommands(m_commandSystem, commands);
	m_graphicsPipeline->endRenderPass(m_commandSystem);
	// 4. 提交到图形队列（发出渲染完成信号量）
	m_commandSystem->submitFrame();

	// 5. 提交呈现请求（等待渲染完成信号量）
	m_swapChain->present(m_commandSystem, imageIndex);

	// 6. 推进到下一帧（更新帧索引）
	m_commandSystem->advanceFrame();
}

void VulkanApp::cleanup()
{
	if(m_graphicsPipeline)
		delete m_graphicsPipeline;
	if(m_renderPass)
		delete m_renderPass;
	if (m_swapChain)
		delete m_swapChain; // 清理 Vulkan 交换链
	if (m_commandSystem)
		delete m_commandSystem; // 清理 Vulkan 命令系统
	if (m_context)
		delete m_context; // 清理 Vulkan 上下文
	glfwDestroyWindow(window); // 销毁 GLFW 窗口
}

void VulkanApp::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<VulkanApp*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true; // 标记帧缓冲区已调整大小
}