#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
// QueueFamilyIndices 结构体用于存储 Vulkan 队列族的索引。
// graphicsFamily 表示支持图形操作的队列族索引。
// presentFamily 表示支持呈现操作的队列族索引。
// isComplete() 方法用于检查两个索引是否都已设置。
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// SwapChainSupportDetails 结构体用于存储交换链支持的详细信息。
// 包括表面能力、格式和呈现模式。
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanContext
{
public:
    VulkanContext(GLFWwindow* window, bool enableValidation = true);
    void ReleaseResouce();
    VkDevice& getDevice();
	VkSurfaceKHR& getSurface();
	VkPhysicalDevice& getPhysicalDevice();
    VkQueue& getGraphicQueue();
    VkQueue& getPresentQueue();
	void waitDeviceIdle() const;
	GLFWwindow* getWindow() const { return m_window; } // 获取 GLFW 窗口对象
	int getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; } // 获取最大帧数
public:
	void getFramebufferSize(int& width, int& height) const;
	QueueFamilyIndices getQueueFamilies();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
private:
    void createInstance();
    // 创建 Vulkan 实例（连接应用程序与驱动）
    void setupDebugMessenger();      // 设置调试回调（捕获 Vulkan 错误和警告）
    void createSurface(GLFWwindow* window);            // 创建窗口表面（连接 Vulkan 与操作系统窗口）
    void pickPhysicalDevice();       // 选择物理 GPU（如 NVIDIA/AMD 显卡）
    void createLogicalDevice();      // 创建逻辑设备（定义 GPU 的功能接口）

public:
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions(bool enableValidation);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
	GLFWwindow* m_window = nullptr; // GLFW 窗口对象。
    bool m_bValidationLayers = false;
	int MAX_FRAMES_IN_FLIGHT = 2; // 最大帧数，双缓冲。
private:
    const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME // 设备扩展：交换链。
    };
    const std::vector<const char*> validationLayers = {
   "VK_LAYER_KHRONOS_validation"
    };

};