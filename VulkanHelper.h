#pragma once
#include "VulkanContext.h"
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include "VulkanCommandSystem.h"
class VulkanHelper
{
public:
	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary); // 以二进制模式打开文件，并定位到文件末尾

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!"); // 打开失败抛出异常
		}

		size_t fileSize = (size_t)file.tellg(); // 获取文件大小
		std::vector<char> buffer(fileSize); // 分配缓冲区
		file.seekg(0); // 回到文件开头
		file.read(buffer.data(), fileSize); // 读取文件内容
		file.close();
		return buffer;
	}
};

class VulkanImageHelper
{
public:
	static void createImage(VulkanContext* context, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	static VkImageView createImageView(VulkanContext* context, const VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
};

class VulkanBufferHelper
{
public:
	static void createBuffer(VulkanContext* context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	static void copyBuffer(VulkanContext* context, VulkanCommandSystem* commandSystem, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	
};