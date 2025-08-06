#pragma once
#include <GLFW/glfw3.h>
#include "VulkanContext.h"
#include "VulkanCommandSystem.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include <vector>
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class SimpleMesh;
class VulkanApp
{
public:
	void run(); // 运行应用程序的主入口。

private:
	void initWindow(); // 初始化窗口。
	void initVulkan(); // 初始化 Vulkan 上下文和相关资源。
	void mainLoop(); // 主循环，处理事件和渲染。
	void drawFrame();
	void cleanup(); // 清理资源

private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* window; // GLFW 窗口对象。
	VulkanContext* m_context;
	VulkanCommandSystem* m_commandSystem; // Vulkan 命令系统，用于提交渲染命令。
	VulkanSwapChain* m_swapChain; // Vulkan 交换链，用于管理帧缓冲区。
	VulkanRenderPass* m_renderPass;
	VulkanGraphicsPipeline* m_graphicsPipeline;
	bool framebufferResized = false; // 帧缓冲区是否已调整大小。
	std::vector<SimpleMesh*> m_meshes;
};

