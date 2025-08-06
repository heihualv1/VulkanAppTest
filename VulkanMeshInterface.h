#pragma once
#include "VulkanRenderCommand.h"
#include <memory>
class IMesh 
{
public:
    virtual ~IMesh() = default;
    virtual std::unique_ptr<IRenderCommand> createRenderCommand() const = 0;
    virtual const VkBuffer& getVertexBuffer() const = 0;
    virtual uint32_t getVertexCount() const = 0;
};

