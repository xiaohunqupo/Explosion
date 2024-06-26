//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Synchronous.h>

namespace RHI::Dummy {
    class DummyDevice;

    class DummyFence final : public Fence {
    public:
        NonCopyable(DummyFence)
        explicit DummyFence(DummyDevice& device, bool bInitAsSignal);
        ~DummyFence() override;

        bool IsSignaled() override;
        void Reset() override;
        void Wait() override;
    };

    class DummySemaphore final : public Semaphore {
    public:
        NonCopyable(DummySemaphore)
        explicit DummySemaphore(DummyDevice& device);
        ~DummySemaphore() override;
    };
}
