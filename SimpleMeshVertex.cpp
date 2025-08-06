#include "SimpleMeshVertex.h"
#include <stdexcept>
#include "VulkanHelper.h"
SimpleMesh::SimpleMesh(VulkanContext* context ,const std::vector<Vertex>& Vertices, const std::vector<uint16_t>& indices)
	: m_context(context), m_vecVertices(Vertices), m_vecIndices(indices)
{
}

SimpleMesh::~SimpleMesh()
{
	
}

std::unique_ptr<IRenderCommand> SimpleMesh::createRenderCommand() const
{
	return std::make_unique<SimpleMeshRenderCommand>(this);
}

void SimpleMesh::createVertexBuffer(VulkanCommandSystem* commandSystem)
{
	VkDeviceSize bufferSize = sizeof(m_vecVertices[0]) * m_vecVertices.size();
	//VulkanBufferHelper::createBuffer(m_context, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanBufferHelper::createBuffer(m_context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_vecVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_context->getDevice(), stagingBufferMemory);

	VulkanBufferHelper::createBuffer(m_context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	VulkanBufferHelper::copyBuffer(m_context, commandSystem, stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(m_context->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_context->getDevice(), stagingBufferMemory, nullptr);
}

void SimpleMesh::createIndexBuffer(VulkanCommandSystem* commandSystem)
{
	VkDeviceSize bufferSize = sizeof(m_vecIndices[0]) * m_vecIndices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanBufferHelper::createBuffer(m_context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_vecIndices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_context->getDevice(), stagingBufferMemory);

	VulkanBufferHelper::createBuffer(m_context,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	VulkanBufferHelper::copyBuffer(m_context, commandSystem, stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_context->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_context->getDevice(), stagingBufferMemory, nullptr);
}

void SimpleMesh::cleanup()
{
	if (vertexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(m_context->getDevice(), vertexBuffer, nullptr);
		vertexBuffer = VK_NULL_HANDLE;
	}
	if (vertexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(m_context->getDevice(), vertexBufferMemory, nullptr);
		vertexBufferMemory = VK_NULL_HANDLE;
	}

	if (indexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(m_context->getDevice(), indexBuffer, nullptr);
		indexBuffer = VK_NULL_HANDLE;
	}
	if (indexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(m_context->getDevice(), indexBufferMemory, nullptr);
		indexBufferMemory = VK_NULL_HANDLE;
	}
}
