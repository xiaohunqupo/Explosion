//
// Created by johnk on 2022/1/23.
//

#ifndef EXPLOSION_RHI_TEXTURE_H
#define EXPLOSION_RHI_TEXTURE_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct TextureViewCreateInfo;
    class TextureView;

    struct TextureCreateInfo {
        Extent<3> extent;
        size_t mipLevels;
        size_t samples;
        TextureDimension dimension;
        PixelFormat format;
        TextureUsageFlags usages;
    };

    class Texture {
    public:
        NON_COPYABLE(Texture)
        virtual ~Texture();

        virtual TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) = 0;
        virtual void Destroy() = 0;

    protected:
        explicit Texture(const TextureCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_TEXTURE_H