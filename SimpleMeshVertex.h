#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include "VulkanContext.h"
#include <memory>
#include "VulkanRenderCommand.h"
#include "VulkanMeshInterface.h"
#include "VulkanCommandSystem.h"
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

};

const std::vector<Vertex> sampleVertices1 = {
    {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

const std::vector<uint16_t> indices1 = {
    0, 1, 2
};

const std::vector<Vertex> sampleVertices2 = {
    {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};
const std::vector<uint16_t> indices2 = {
    0, 2, 1
};

class SimpleMesh : public IMesh
{
public:
    SimpleMesh(VulkanContext* context ,const std::vector<Vertex>& Vertices, const std::vector<uint16_t>& indices);
	~SimpleMesh();
	
    virtual std::unique_ptr<IRenderCommand> createRenderCommand() const override;

    void createVertexBuffer(VulkanCommandSystem* commandSystem);
    void createIndexBuffer(VulkanCommandSystem* commandSystem);
    void cleanup();

    const VkBuffer& getVertexBuffer() const { return vertexBuffer; }
    const VkBuffer& getIndexBuffer() const { return indexBuffer; }
	uint32_t getVertexCount() const { return static_cast<uint32_t>(m_vecVertices.size()); }
    uint32_t getIndexCount() const { return static_cast<uint32_t>(m_vecIndices.size()); }

private:
    const std::vector<Vertex> m_vecVertices;
    const std::vector<uint16_t> m_vecIndices;
    VulkanContext* m_context;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

