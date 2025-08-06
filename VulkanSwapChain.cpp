#include "VulkanSwapChain.h"
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <stdexcept> // Necessary for std::runtime_error
VulkanSwapChain::VulkanSwapChain(VulkanContext* context)
	: m_context(context)
{
	createSwapChain();
	createImageViews();
}

VulkanSwapChain::~VulkanSwapChain()
{
	cleanup();
}

void VulkanSwapChain::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = m_context->querySwapChainSupport(m_context->getPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_context->getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	QueueFamilyIndices indices = m_context->getQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	if (vkCreateSwapchainKHR(m_context->getDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_context->getDevice(), m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_context->getDevice(), m_swapChain, &imageCount, m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;

}

void VulkanSwapChain::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(m_context->getDevice(), &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void VulkanSwapChain::cleanup()
{
    for (auto imageView : m_swapChainImageViews) {
        vkDestroyImageView(m_context->getDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_context->getDevice(), m_swapChain, nullptr);
   
}

VkFormat VulkanSwapChain::getSwapChainFormat()
{
    return m_swapChainImageFormat;
}

VkExtent2D VulkanSwapChain::getSwapChianExtent()
{
    return m_swapChainExtent;
}

void VulkanSwapChain::present(VulkanCommandSystem* commandSystem, uint32_t imageIndex)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // 等待渲染完成信号量
    VkSemaphore waitSemaphores[] = { commandSystem->currentRenderFinishedSemaphore() };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapChain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    // 提交到呈现队列
    VkResult result = vkQueuePresentKHR(m_context->getPresentQueue(), &presentInfo);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

void VulkanSwapChain::getImageIndex(VulkanCommandSystem* commandSystem, uint32_t& imageIndex)
{
    vkAcquireNextImageKHR(m_context->getDevice(), m_swapChain, UINT64_MAX, commandSystem->currentImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(m_context->getWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
