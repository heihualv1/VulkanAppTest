#pragma once
#include "VulkanContext.h"

class VulkanGraphicsPipeline; // 前向声明图形管线类
class VulkanCommandSystem
{
public:
    // 同步对象结构（每帧一组）
    struct FrameSyncObjects {
        VkSemaphore imageAvailable = VK_NULL_HANDLE;      // 图像获取信号量
        VkSemaphore renderFinished = VK_NULL_HANDLE;     // 渲染完成信号量
        VkFence inFlight = VK_NULL_HANDLE;               // 帧处理中栅栏
    };

    // 命令缓冲区分组（每帧一组）
    struct FrameCommandBuffers {
        VkCommandBuffer primary = VK_NULL_HANDLE;        // 主命令缓冲区
    };

public:
    VulkanCommandSystem(VulkanContext* context);
    ~VulkanCommandSystem();

	void Init();
    // 帧生命周期管理
    void beginFrame();
    void submitFrame();
    void waitIdle() const;
    void advanceFrame();

    // 命令缓冲区访问
    VkCommandBuffer& getPrimaryCommandBuffer() {
        return currentCommandBuffers().primary;
    }

    // 当前帧同步对象
    VkSemaphore& currentImageAvailableSemaphore() {
        return currentSyncObjects().imageAvailable;
    }

    VkSemaphore& currentRenderFinishedSemaphore() {
        return currentSyncObjects().renderFinished;
    }

    VkFence& currentFrameFence() {
        return currentSyncObjects().inFlight;
    }

    VkCommandPool& getGraphicCommandPool()
    {
        return m_graphicsCommandPool;
    }


    // 实用方法
    void resetFrame(); // 重置当前帧命令缓冲区（如果发生错误）

private:
    // 初始化方法
    void createCommandPools();
    void createCommandBuffers();
    void createSyncObjects();

    FrameCommandBuffers& currentCommandBuffers() {
        return m_commandBuffers[m_currentFrameIndex];
    }

    FrameSyncObjects& currentSyncObjects() {
        return m_syncObjects[m_currentFrameIndex];
    }

private:
    VulkanContext* m_context;                  // Vulkan上下文引用
    uint32_t m_framesInFlight;                           // 配置参数

    uint32_t m_currentFrameIndex = 0;           // 当前帧索引（0到framesInFlight-1）
    std::vector<FrameSyncObjects> m_syncObjects; // 每帧同步对象
    std::vector<FrameCommandBuffers> m_commandBuffers; // 每帧命令缓冲区

    VkCommandPool m_graphicsCommandPool = VK_NULL_HANDLE;
};

