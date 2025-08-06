#pragma once
#include "VulkanCommandSystem.h"

class IRenderCommand {
public:
    virtual ~IRenderCommand() = default;
    virtual void execute(VkCommandBuffer cmdBuffer) const = 0;
};

class SimpleMesh;
class SimpleMeshRenderCommand : public IRenderCommand {
public:
    explicit SimpleMeshRenderCommand(const SimpleMesh* mesh);

    void execute(VkCommandBuffer cmdBuffer) const override;

private:
    const SimpleMesh* m_mesh;
};