//
// Created by johnk on 21/2/2022.
//

#include <RHI/CommandRecorder.h>

namespace RHI {
    TextureSubResourceInfo::TextureSubResourceInfo(
        const uint8_t inMipLevel,
        const uint8_t inArrayLayer,
        const TextureAspect inAspect)
        : mipLevel(inMipLevel)
        , arrayLayer(inArrayLayer)
        , aspect(inAspect)
    {
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetMipLevel(const uint8_t inMipLevel)
    {
        mipLevel = inMipLevel;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetArrayLayer(const uint8_t inArrayLayer)
    {
        arrayLayer = inArrayLayer;
        return *this;
    }

    TextureSubResourceInfo& TextureSubResourceInfo::SetAspect(const TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    BufferCopyInfo::BufferCopyInfo(const size_t inSrcOffset, const size_t inDstOffset, const size_t inCopySize)
        : srcOffset(inSrcOffset)
        , dstOffset(inDstOffset)
        , copySize(inCopySize)
    {
    }

    BufferCopyInfo& BufferCopyInfo::SetSrcOffset(const size_t inSrcOffset)
    {
        srcOffset = inSrcOffset;
        return *this;
    }

    BufferCopyInfo& BufferCopyInfo::SetDstOffset(const size_t inDstOffset)
    {
        dstOffset = inDstOffset;
        return *this;
    }

    BufferCopyInfo& BufferCopyInfo::SetCopySize(const size_t inCopySize)
    {
        copySize = inCopySize;
        return *this;
    }

    TextureCopyInfo::TextureCopyInfo(const TextureSubResourceInfo& inSrcSubResource, const Common::UVec3& inSrcOrigin, const TextureSubResourceInfo& inDstSubResource, const Common::UVec3& inDstOrigin, const Common::UVec3& inCopyRegion)
        : srcSubResource(inSrcSubResource)
        , srcOrigin(inSrcOrigin)
        , dstSubResource(inDstSubResource)
        , dstOrigin(inDstOrigin)
        , copyRegion(inCopyRegion)
    {
    }

    TextureCopyInfo& TextureCopyInfo::SetSrcSubResource(const TextureSubResourceInfo& inSrcSubResource)
    {
        srcSubResource = inSrcSubResource;
        return *this;
    }

    TextureCopyInfo& TextureCopyInfo::SetSrcOrigin(const Common::UVec3& inSrcOrigin)
    {
        srcOrigin = inSrcOrigin;
        return *this;
    }

    TextureCopyInfo& TextureCopyInfo::SetDstSubResource(const TextureSubResourceInfo& inDstSubResource)
    {
        dstSubResource = inDstSubResource;
        return *this;
    }

    TextureCopyInfo& TextureCopyInfo::SetDstOrigin(const Common::UVec3& inDstOrigin)
    {
        dstOrigin = inDstOrigin;
        return *this;
    }

    TextureCopyInfo& TextureCopyInfo::SetCopyRegion(const Common::UVec3& inCopyRegion)
    {
        copyRegion = inCopyRegion;
        return *this;
    }

    BufferTextureCopyInfo::BufferTextureCopyInfo(const size_t inBufferOffset, const TextureSubResourceInfo& inTextureSubResource, const Common::UVec3& inTextureOrigin, const Common::UVec3& inCopyRegion)
        : bufferOffset(inBufferOffset)
        , textureSubResource(inTextureSubResource)
        , textureOrigin(inTextureOrigin)
        , copyRegion(inCopyRegion)
    {
    }

    BufferTextureCopyInfo& BufferTextureCopyInfo::SetBufferOffset(const size_t inBufferOffset)
    {
        bufferOffset = inBufferOffset;
        return *this;
    }

    BufferTextureCopyInfo& BufferTextureCopyInfo::SetTextureSubResource(const TextureSubResourceInfo& inTextureSubResource)
    {
        textureSubResource = inTextureSubResource;
        return *this;
    }

    BufferTextureCopyInfo& BufferTextureCopyInfo::SetTextureOrigin(const Common::UVec3& inTextureOrigin)
    {
        textureOrigin = inTextureOrigin;
        return *this;
    }

    BufferTextureCopyInfo& BufferTextureCopyInfo::SetCopyRegion(const Common::UVec3& inCopyRegion)
    {
        copyRegion = inCopyRegion;
        return *this;
    }

    ColorAttachment::ColorAttachment(
        TextureView* inView,
        const LoadOp inLoadOp,
        const StoreOp inStoreOp,
        const Common::LinearColor& inClearValue,
        TextureView* inResolveView)
        : ColorAttachmentBase(inLoadOp, inStoreOp, inClearValue)
        , view(inView)
        , resolveView(inResolveView)
    {
    }

    ColorAttachment& ColorAttachment::SetView(TextureView* inView)
    {
        view = inView;
        return *this;
    }

    ColorAttachment& ColorAttachment::SetResolveView(TextureView* inResolveView)
    {
        resolveView = inResolveView;
        return *this;
    }

    DepthStencilAttachment::DepthStencilAttachment(
        TextureView* inView,
        const bool inDepthReadOnly,
        const LoadOp inDepthLoadOp,
        const StoreOp inDepthStoreOp,
        const float inDepthClearValue,
        const bool inStencilReadOnly,
        const LoadOp inStencilLoadOp,
        const StoreOp inStencilStoreOp,
        const uint32_t inStencilClearValue)
        : DepthStencilAttachmentBase(
            inDepthReadOnly, inDepthLoadOp, inDepthStoreOp, inDepthClearValue,
            inStencilReadOnly, inStencilLoadOp, inStencilStoreOp, inStencilClearValue)
        , view(inView)
    {
    }

    DepthStencilAttachment& DepthStencilAttachment::SetView(TextureView* inView)
    {
        view = inView;
        return *this;
    }

    RasterPassBeginInfo::RasterPassBeginInfo() = default;

    RasterPassBeginInfo& RasterPassBeginInfo::SetDepthStencilAttachment(const DepthStencilAttachment& inDepthStencilAttachment)
    {
        depthStencilAttachment = inDepthStencilAttachment;
        return *this;
    }

    RasterPassBeginInfo& RasterPassBeginInfo::AddColorAttachment(const ColorAttachment& inColorAttachment)
    {
        colorAttachments.emplace_back(inColorAttachment);
        return *this;
    }

    CommandRecorder::CommandRecorder() = default;

    CommandRecorder::~CommandRecorder() = default;

    CommandCommandRecorder::~CommandCommandRecorder() = default;

    CopyPassCommandRecorder::CopyPassCommandRecorder() = default;

    CopyPassCommandRecorder::~CopyPassCommandRecorder() = default;

    ComputePassCommandRecorder::ComputePassCommandRecorder() = default;

    ComputePassCommandRecorder::~ComputePassCommandRecorder() = default;

    RasterPassCommandRecorder::RasterPassCommandRecorder() = default;

    RasterPassCommandRecorder::~RasterPassCommandRecorder() = default;
}
