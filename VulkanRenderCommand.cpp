#include "VulkanRenderCommand.h"
#include "SimpleMeshVertex.h"
SimpleMeshRenderCommand::SimpleMeshRenderCommand(const SimpleMesh* mesh)
	: m_mesh(mesh)
{
}

void SimpleMeshRenderCommand::execute(VkCommandBuffer cmdBuffer) const
{
    VkBuffer vertexBuffers[] = { m_mesh->getVertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, m_mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_mesh->getIndexCount()), 1, 0, 0, 0);
}
