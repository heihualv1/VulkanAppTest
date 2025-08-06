#pragma once
#include "VulkanContext.h"
#include "VulkanCommandSystem.h"

class VulkanSwapChain
{
public:
	VulkanSwapChain(VulkanContext* context);
	~VulkanSwapChain();
public:
	void createSwapChain();
	void createImageViews();
	void cleanup();
	VkFormat getSwapChainFormat();
	VkExtent2D getSwapChianExtent();
	void getImageIndex(VulkanCommandSystem* commandSystem, uint32_t& imageIndex);
	void present(VulkanCommandSystem* commandSystem, uint32_t imageIndex);

public:
	const std::vector<VkImageView>& getSwapChainImageViews() const { return m_swapChainImageViews; }
public:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
private:
	VulkanContext* m_context;
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	std::vector<VkImageView> m_swapChainImageViews;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
};

