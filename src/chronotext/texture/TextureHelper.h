/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/texture/TextureData.h"
#include "chronotext/system/MemoryInfo.h"

#include <map>

namespace chr
{
    class TextureHelper
    {
    public:
        static bool PROBE_MEMORY;

        struct MemoryProbe
        {
            TextureRequest textureRequest;
            size_t memoryUsage;
            MemoryInfo memoryInfo[3];
        };
        
        static MemoryInfo memoryInfo[2];
        static std::map<ci::gl::Texture*, MemoryProbe> probes;

        // ---
        
        static ci::gl::TextureRef loadTexture(const std::string &resourceName, bool useMipmap = false, TextureRequest::Flags flags = TextureRequest::FLAGS_NONE);
        static ci::gl::TextureRef loadTexture(InputSource::Ref inputSource, bool useMipmap = false, TextureRequest::Flags flags = TextureRequest::FLAGS_NONE);
        static ci::gl::TextureRef loadTexture(const TextureRequest &textureRequest);
        
        static TextureData fetchTextureData(const TextureRequest &textureRequest);
        static ci::gl::TextureRef uploadTextureData(const TextureData &textureData);
        
        static ci::Vec2i getTextureSize(const TextureData &textureData);
        static size_t getTextureMemoryUsage(const TextureData &textureData);
        
        // ---
        
        static void bindTexture(ci::gl::Texture *texture);
        static void beginTexture(ci::gl::Texture *texture);
        static void endTexture();
        
        static void drawTextureFromCenter(ci::gl::Texture *texture);
        static void drawTexture(ci::gl::Texture *texture, float rx = 0, float ry = 0);
        static void drawTextureInRect(ci::gl::Texture *texture, const ci::Rectf &rect, float ox = 0, float oy = 0);
        
    protected:
        static void textureDeallocator(void *refcon);
        static TextureData fetchTranslucentTextureData(const TextureRequest &textureRequest);
        static TextureData fetchPowerOfTwoTextureData(const TextureRequest &textureRequest);
    };
}
