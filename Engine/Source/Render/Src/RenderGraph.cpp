//
// Created by johnk on 2023/2/10.
//

#include <utility>
#include <unordered_set>
#include <optional>

#include <Render/RenderGraph.h>

namespace Render {
    static RHI::ComputePassBeginInfo GetRHIComputePassBeginInfo(const RGComputePassDesc& desc)
    {
        RHI::ComputePassBeginInfo result;
        result.pipeline = desc.pipeline;
        return result;
    }

    static std::vector<RHI::GraphicsPassColorAttachment> GetRHIColorAttachments(const RGRasterPassDesc& desc)
    {
        std::vector<RHI::GraphicsPassColorAttachment> result(desc.colorAttachments.size());
        for (auto i = 0; i < result.size(); i++) {
            auto& dst = result[i];
            const auto& src = desc.colorAttachments[i];

            dst.view = src.view == nullptr ? nullptr : src.view->GetRHI();
            dst.resolve = src.resolve == nullptr ? nullptr : src.resolve->GetRHI();
            dst.clearValue = src.clearValue;
            dst.loadOp = src.loadOp;
            dst.storeOp = src.storeOp;
        }
        return result;
    }

    static std::optional<RHI::GraphicsPassDepthStencilAttachment> GetRHIDepthStencilAttachment(const RGRasterPassDesc& desc)
    {
        if (!desc.depthStencilAttachment.has_value()) {
            return {};
        }

        const auto& src = desc.depthStencilAttachment.value();

        RHI::GraphicsPassDepthStencilAttachment result;
        result.view = src.view == nullptr ? nullptr : src.view->GetRHI();
        result.depthClearValue = src.depthClearValue;
        result.depthLoadOp = src.depthLoadOp;
        result.depthStoreOp = src.depthStoreOp;
        result.depthReadOnly = src.depthReadOnly;
        result.stencilClearValue = src.stencilClearValue;
        result.stencilLoadOp = src.stencilLoadOp;
        result.stencilStoreOp = src.stencilStoreOp;
        result.stencilReadOnly = src.stencilReadOnly;
        return result;
    }

    static RHI::GraphicsPassBeginInfo GetRHIGraphicsPassBeginInfo(
        const RGRasterPassDesc& desc,
        uint32_t colorAttachmentNum,
        const RHI::GraphicsPassColorAttachment* colorAttachments,
        const RHI::GraphicsPassDepthStencilAttachment* depthStencilAttachment)
    {
        RHI::GraphicsPassBeginInfo result;
        result.pipeline = desc.pipeline;
        result.colorAttachmentNum = colorAttachmentNum;
        result.colorAttachments = colorAttachments;
        result.depthStencilAttachment = depthStencilAttachment;
        return result;
    }

    static RHI::Barrier GetBarrier(const RGResTransition& transition)
    {
        if (transition.resType == RHI::ResourceType::BUFFER) {
            const auto& bufferTransition = transition.buffer;
            return RHI::Barrier::Transition(bufferTransition.buffer->GetRHI(), bufferTransition.before, bufferTransition.after);
        }
        if (transition.resType == RHI::ResourceType::TEXTURE) {
            const auto& textureTransition = transition.texture;
            return RHI::Barrier::Transition(textureTransition.texture->GetRHI(), textureTransition.before, textureTransition.after);
        }
        Assert(false);
        return {};
    }

    static RHI::BufferCreateInfo GetRHIBufferCreateInfo(const RGBufferDesc& desc)
    {
        RHI::BufferCreateInfo createInfo;
        createInfo.size = desc.size;
        createInfo.usages = desc.usages;
        return createInfo;
    }

    static RHI::TextureCreateInfo GetRHITextureCreateInfo(const RGTextureDesc& desc)
    {
        RHI::TextureCreateInfo createInfo;
        createInfo.extent = desc.extent;
        createInfo.mipLevels = desc.mipLevels;
        createInfo.samples = desc.samples;
        createInfo.dimension = desc.dimension;
        createInfo.format = desc.format;
        createInfo.usages = desc.usages;
        return createInfo;
    }

    static RHI::BufferViewCreateInfo GetRHIBufferViewCreateInfo(const RGBufferViewDesc& desc)
    {
        RHI::BufferViewCreateInfo createInfo;
        createInfo.offset = desc.offset;
        createInfo.size = desc.size;
        createInfo.vertex.stride = desc.vertex.stride;
        return createInfo;
    }

    static RHI::TextureViewCreateInfo GetRHITextureViewCreateInfo(const RGTextureViewDesc& desc)
    {
        RHI::TextureViewCreateInfo createInfo;
        createInfo.dimension = desc.dimension;
        createInfo.aspect = desc.aspect;
        createInfo.baseMipLevel = desc.baseMipLevel;
        createInfo.mipLevelNum = desc.mipLevelNum;
        createInfo.baseArrayLayer = desc.baseArrayLayer;
        createInfo.arrayLayerNum = desc.arrayLayerNum;
        return createInfo;
    }
}

namespace Render {
    RGResource::RGResource(std::string inName, bool inIsExternal, RGResource* inParent)
        : name(std::move(inName))
        , parent(inParent)
        , isExternal(inIsExternal)
        , rhiAccess(false)
    {
    }

    RGResource::~RGResource() = default;

    bool RGResource::IsExternal() const
    {
        return isExternal;
    }

    bool RGResource::IsCulled() const
    {
        return isCulled;
    }

    const std::string& RGResource::GetName() const
    {
        return name;
    }

    bool RGResource::CanAccessRHI() const
    {
        return rhiAccess;
    }

    RGResource* RGResource::GetParent() const
    {
        return parent;
    }

    void RGResource::SetCulled(bool inCulled)
    {
        isCulled = inCulled;
    }

    void RGResource::SetRHIAccess(bool inRhiAccess)
    {
        rhiAccess = inRhiAccess;
    }

    RGBuffer::RGBuffer(std::string inName, const RGBufferDesc& inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGBuffer::RGBuffer(std::string inName, RHI::Buffer* inBuffer)
        : RGResource(std::move(inName), true)
        , rhiHandle(inBuffer)
    {
    }

    RGBuffer::~RGBuffer() = default;

    RGResourceType RGBuffer::GetType()
    {
        return RGResourceType::BUFFER;
    }

    void RGBuffer::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHIBufferCreateInfo(desc);
            rhiHandle = device.CreateBuffer(&createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGBuffer::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Buffer* RGBuffer::GetRHI()
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    RGTexture::RGTexture(std::string inName, const RGTextureDesc& inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGTexture::RGTexture(std::string inName, RHI::Texture* inTexture)
        : RGResource(std::move(inName), true)
        , rhiHandle(inTexture)
    {
    }

    RGTexture::~RGTexture() = default;

    RGResourceType RGTexture::GetType()
    {
        return RGResourceType::TEXTURE;
    }

    void RGTexture::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHITextureCreateInfo(desc);
            rhiHandle = device.CreateTexture(&createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGTexture::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Texture* RGTexture::GetRHI()
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    RGBufferView::RGBufferView(std::string inName, RGBuffer* inBuffer, const RGBufferViewDesc& inDesc)
        : RGResource(std::move(inName), false, inBuffer)
        , buffer(inBuffer)
        , desc(inDesc)
    {
    }

    RGBufferView::RGBufferView(std::string inName, RHI::BufferView* inBufferView)
        : RGResource(std::move(inName), true)
        , rhiHandle(inBufferView)
    {
    }

    RGBufferView::~RGBufferView() = default;

    RGResourceType RGBufferView::GetType()
    {
        return RGResourceType::BUFFER_VIEW;
    }

    void RGBufferView::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHIBufferViewCreateInfo(desc);
            Assert(buffer);
            Assert(buffer->GetRHI());
            rhiHandle = buffer->GetRHI()->CreateBufferView(&createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGBufferView::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::BufferView* RGBufferView::GetRHI()
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    RGBuffer* RGBufferView::GetBuffer()
    {
        Assert(!IsExternal());
        return buffer;
    }

    RGTextureView::RGTextureView(std::string inName, RGTexture* inTexture, const RGTextureViewDesc& inDesc)
        : RGResource(std::move(inName), false, inTexture)
        , texture(inTexture)
        , desc(inDesc)
    {
    }

    RGTextureView::RGTextureView(std::string inName, RHI::TextureView* inTextureView)
        : RGResource(std::move(inName), true)
        , rhiHandle(inTextureView)
    {
    }

    RGTextureView::~RGTextureView() = default;

    RGResourceType RGTextureView::GetType()
    {
        return RGResourceType::TEXTURE_VIEW;
    }

    void RGTextureView::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHITextureViewCreateInfo(desc);
            Assert(texture);
            Assert(texture->GetRHI());
            rhiHandle = texture->GetRHI()->CreateTextureView(&createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGTextureView::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::TextureView* RGTextureView::GetRHI()
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    RGTexture* RGTextureView::GetTexture()
    {
        Assert(!IsExternal());
        return texture;
    }

    RGPass::RGPass(std::string inName)
        : name(std::move(inName))
    {
    }

    RGPass::~RGPass() = default;

    RGCopyPass::RGCopyPass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    RGPassType RGCopyPass::GetType()
    {
        return RGPassType::COPY;
    }

    RGComputePass::RGComputePass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGComputePass::~RGComputePass() = default;

    RGPassType RGComputePass::GetType()
    {
        return RGPassType::COMPUTE;
    }

    RGRasterPass::RGRasterPass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGRasterPass::~RGRasterPass() = default;

    RGPassType RGRasterPass::GetType()
    {
        return RGPassType::RASTER;
    }

    RGFuncCopyPass::RGFuncCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc)
        : RGCopyPass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
    {
    }

    RGFuncCopyPass::~RGFuncCopyPass() = default;

    void RGFuncCopyPass::Setup(RGCopyPassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncCopyPass::Execute(RHI::CommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RGFuncComputePass::RGFuncComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc)
        : RGComputePass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
        , executeFunc(nullptr)
    {
    }

    RGFuncComputePass::~RGFuncComputePass() = default;

    void RGFuncComputePass::Setup(RGComputePassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncComputePass::Execute(RHI::ComputePassCommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RGFuncRasterPass::RGFuncRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc)
        : RGRasterPass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
    {
    }

    RGFuncRasterPass::~RGFuncRasterPass() = default;

    void RGFuncRasterPass::Setup(RGRasterPassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncRasterPass::Execute(RHI::GraphicsPassCommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RenderGraph::RenderGraph(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    RenderGraph::~RenderGraph() = default;

    void RenderGraph::AddCopyPass(RGCopyPass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddComputePass(RGComputePass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddRasterPass(RGRasterPass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncCopyPass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::AddComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncComputePass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::AddRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncRasterPass(std::move(inName), std::move(inSetupFunc)));
    }

    RHI::Device& RenderGraph::GetDevice()
    {
        return device;
    }

    void RenderGraph::Setup()
    {
        for (auto& pass : passes) {
            if (pass->GetType() == RGPassType::COPY) {
                auto* copyPass = static_cast<RGCopyPass*>(pass.get());
                RGCopyPassBuilder builder(*this, *copyPass);
                copyPass->Setup(builder);
            } else if (pass->GetType() == RGPassType::COMPUTE) {
                auto* computePass = static_cast<RGComputePass*>(pass.get());
                RGComputePassBuilder builder(*this, *computePass);
                computePass->Setup(builder);
            } else if (pass->GetType() == RGPassType::RASTER) {
                auto* rasterPass = static_cast<RGRasterPass*>(pass.get());
                RGRasterPassBuilder builder(*this, *rasterPass);
                rasterPass->Setup(builder);
            } else {
                Assert(false);
            }
        }
    }

    void RenderGraph::Compile()
    {
        for (auto& resource : resources) {
            resource->Destroy();
            resource->SetCulled(false);
        }

        std::unordered_set<RGResource*> consumeds;
        for (auto& pass : passes) {
            for (const auto& read : pass->reads) {
                consumeds.emplace(read);
            }
        }

        for (auto& resource : resources) {
            if (!consumeds.contains(resource.get())) {
                resource->SetCulled(true);
            }
        }

        ComputeResBarriers();
    }

    void RenderGraph::Execute(RHI::Fence* mainFence, RHI::Fence* asyncFence)
    {
        std::vector<RGResource*> actualResesToDevirtualize;
        std::vector<RGResource*> actualResViewsToDevirtualize;

        for (auto& resource : resources) {
            resource->SetRHIAccess(false);

            auto type = resource->GetType();
            if (type == RGResourceType::BUFFER || type == RGResourceType::TEXTURE) {
                actualResesToDevirtualize.emplace_back(resource.get());
            }
            if (type == RGResourceType::BUFFER_VIEW || type == RGResourceType::TEXTURE_VIEW) {
                actualResViewsToDevirtualize.emplace_back(resource.get());
            }
        }

        for (auto* resource : actualResesToDevirtualize) {
            if (!resource->isCulled) {
                resource->Devirtualize(device);
            }
        }
        for (auto* resource : actualResViewsToDevirtualize) {
            if (!resource->isCulled) {
                resource->Devirtualize(device);
            }
        }

        Assert(device.GetQueueNum(RHI::QueueType::GRAPHICS) > 0);
        RHI::Queue* mainQueue = device.GetQueue(RHI::QueueType::GRAPHICS, 0);
        RHI::Queue* asyncComputeQueue = device.GetQueueNum(RHI::QueueType::COMPUTE) > 1 ? device.GetQueue(RHI::QueueType::COMPUTE, 1) : mainQueue;

        RHI::UniqueRef<RHI::CommandBuffer> mainBuffer = device.CreateCommandBuffer();
        RHI::UniqueRef<RHI::CommandBuffer> asyncComputeBuffer = device.CreateCommandBuffer();
        RHI::UniqueRef<RHI::CommandEncoder> mainEncoder = mainBuffer->Begin();
        RHI::UniqueRef<RHI::CommandEncoder> asyncComputeEncoder = asyncComputeBuffer->Begin();
        {
            for (auto& pass : passes) {
                if (pass->GetType() == RGPassType::COPY) {
                    auto* copyPass = static_cast<RGCopyPass*>(pass.get());
                    TransitionResources(mainEncoder.Get(), copyPass);
                    ExecuteCopyPass(mainEncoder.Get(), copyPass);
                } else if (pass->GetType() == RGPassType::RASTER) {
                    auto* computePass = static_cast<RGComputePass*>(pass.get());
                    auto& commandEncoder = computePass->isAsyncCompute ? asyncComputeEncoder : mainEncoder;
                    TransitionResources(commandEncoder.Get(), computePass);
                    ExecuteComputePass(commandEncoder.Get(), computePass);
                } else if (pass->GetType() == RGPassType::COMPUTE) {
                    auto* rasterPass = static_cast<RGRasterPass*>(pass.get());
                    TransitionResources(mainEncoder.Get(), rasterPass);
                    ExecuteRasterPass(mainEncoder.Get(), rasterPass);
                } else {
                    Assert(false);
                }
            }
        }
        mainEncoder->End();

        mainQueue->Submit(mainBuffer.Get(), mainFence);
        asyncComputeQueue->Submit(asyncComputeBuffer.Get(), asyncFence);
    }

    void RenderGraph::ExecuteCopyPass(RHI::CommandEncoder* encoder, RGCopyPass* copyPass)
    {
        copyPass->Execute(*encoder);
    }

    void RenderGraph::ExecuteComputePass(RHI::CommandEncoder* encoder, RGComputePass* computePass)
    {
        RHI::ComputePassBeginInfo beginInfo = GetRHIComputePassBeginInfo(computePass->passDesc);
        RHI::ComputePassCommandEncoder* computeEncoder = encoder->BeginComputePass(&beginInfo);
        {
            computePass->Execute(*computeEncoder);
        }
        computeEncoder->EndPass();
    }

    void RenderGraph::ExecuteRasterPass(RHI::CommandEncoder* encoder, RGRasterPass* rasterPass)
    {
        std::vector<RHI::GraphicsPassColorAttachment> colorAttachments = GetRHIColorAttachments(rasterPass->passDesc);
        std::optional<RHI::GraphicsPassDepthStencilAttachment> depthStencilAttachment = GetRHIDepthStencilAttachment(rasterPass->passDesc);
        RHI::GraphicsPassBeginInfo beginInfo = GetRHIGraphicsPassBeginInfo(
            rasterPass->passDesc,
            colorAttachments.size(),
            colorAttachments.data(),
            depthStencilAttachment.has_value() ? &depthStencilAttachment.value() : nullptr);
        RHI::GraphicsPassCommandEncoder* graphicsEncoder = encoder->BeginGraphicsPass(&beginInfo);
        {
            rasterPass->Execute(*graphicsEncoder);
        }
        graphicsEncoder->EndPass();
    }

    RGBuffer* RenderGraph::GetActualBufferRes(RGResource* res)
    {
        if (res->GetType() == RGResourceType::BUFFER) {
            return static_cast<RGBuffer*>(res);
        }
        if (res->GetType() == RGResourceType::BUFFER_VIEW) {
            return static_cast<RGBufferView*>(res)->GetBuffer();
        }
        Assert(false);
        return nullptr;
    }

    RGTexture* RenderGraph::GetActualTextureRes(RGResource* res)
    {
        if (res->GetType() == RGResourceType::TEXTURE) {
            return static_cast<RGTexture*>(res);
        }
        if (res->GetType() == RGResourceType::TEXTURE_VIEW) {
            return static_cast<RGTextureView*>(res)->GetTexture();
        }
        Assert(false);
        return nullptr;
    }

    RHI::BufferState RenderGraph::ComputeBufferState(RGPassType passType, RGResourceAccessType accessType)
    {
        if (passType == RGPassType::COPY) {
            if (accessType == RGResourceAccessType::READ) {
                return RHI::BufferState::COPY_SRC;
            }
            if (accessType == RGResourceAccessType::WRITE) {
                return RHI::BufferState::COPY_DST;
            }
        } else {
            if (accessType == RGResourceAccessType::READ) {
                return RHI::BufferState::SHADER_READ_ONLY;
            }
            if (accessType == RGResourceAccessType::WRITE) {
                return RHI::BufferState::STORAGE;
            }
        }
        return RHI::BufferState::UNDEFINED;
    }

    RHI::TextureState RenderGraph::ComputeTextureState(RGPassType passType, RGResourceAccessType accessType)
    {
        if (passType == RGPassType::COPY) {
            if (accessType == RGResourceAccessType::READ) {
                return RHI::TextureState::COPY_SRC;
            }
            if (accessType == RGResourceAccessType::WRITE) {
                return RHI::TextureState::COPY_DST;
            }
        }
        if (passType == RGPassType::COMPUTE) {
            if (accessType == RGResourceAccessType::READ) {
                return RHI::TextureState::SHADER_READ_ONLY;
            }
            if (accessType == RGResourceAccessType::WRITE) {
                return RHI::TextureState::STORAGE;
            }
        }
        if (passType == RGPassType::RASTER) {
            if (accessType == RGResourceAccessType::READ) {
                return RHI::TextureState::SHADER_READ_ONLY;
            }
            if (accessType == RGResourceAccessType::WRITE) {
                return RHI::TextureState::RENDER_TARGET;
            }
        }
        return RHI::TextureState::UNDEFINED;
    }

    void RenderGraph::ComputeResBarriers()
    {
        LastResStates lastResStates;
        for (const auto& pass : passes) {
            ComputeResTransitionsByAccessGroup<RGResourceAccessType::READ>(pass.get(), pass->reads, lastResStates);
            ComputeResTransitionsByAccessGroup<RGResourceAccessType::WRITE>(pass.get(), pass->writes, lastResStates);
            UpdateLastResStatesByAccessGroup<RGResourceAccessType::READ>(pass->GetType(), pass->reads, lastResStates);
            UpdateLastResStatesByAccessGroup<RGResourceAccessType::WRITE>(pass->GetType(), pass->writes, lastResStates);
        }
    }

    void RenderGraph::TransitionResources(RHI::CommandEncoder* encoder, RGPass* pass)
    {
        std::vector<RGResource*> transitionResources;
        for (auto* read : pass->reads) {
            transitionResources.emplace_back(read);
        }
        for (auto* write : pass->writes) {
            transitionResources.emplace_back(write);
        }
        for (auto* res : transitionResources) {
            encoder->ResourceBarrier(GetBarrier(resTransitionMap[std::make_pair(res, pass)]));
        }
    }

    RGPassBuilder::RGPassBuilder(RenderGraph& inGraph, RGPass& inPass)
        : graph(inGraph)
        , pass(inPass)
    {
    }

    RGPassBuilder::~RGPassBuilder() = default;

    RGCopyPassBuilder::RGCopyPassBuilder(RenderGraph& inGraph, RGCopyPass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , copyPass(inPass)
    {
    }

    RGCopyPassBuilder::~RGCopyPassBuilder() = default;

    RGComputePassBuilder::RGComputePassBuilder(RenderGraph& inGraph, RGComputePass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , computePass(inPass)
    {
    }

    RGComputePassBuilder::~RGComputePassBuilder() = default;

    void RGComputePassBuilder::SetAsyncCompute(bool inAsyncCompute)
    {
        computePass.isAsyncCompute = inAsyncCompute;
    }

    void RGComputePassBuilder::SetPassDesc(const RGComputePassDesc& inDesc)
    {
        computePass.passDesc = inDesc;
    }

    RGRasterPassBuilder::RGRasterPassBuilder(RenderGraph& inGraph, RGRasterPass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , rasterPass(inPass)
    {
    }

    RGRasterPassBuilder::~RGRasterPassBuilder() = default;

    void RGRasterPassBuilder::SetPassDesc(const RGRasterPassDesc& inDesc)
    {
        rasterPass.passDesc = inDesc;
    }
}