/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/texture/TextureHelper.h"
#include "chronotext/texture/Texture.h"
#include "chronotext/texture/PVRHelper.h"
#include "chronotext/utils/Utils.h"
#include "chronotext/utils/MathUtils.h"

#include "cinder/ImageIo.h"
#include "cinder/ip/Fill.h"

using namespace std;
using namespace ci;

namespace chronotext
{
    gl::TextureRef TextureHelper::loadTexture(const string &resourceName, bool useMipmap, TextureRequest::Flags flags)
    {
        return loadTexture(InputSource::getResource(resourceName), useMipmap, flags);
    }
    
    gl::TextureRef TextureHelper::loadTexture(InputSourceRef inputSource, bool useMipmap, TextureRequest::Flags flags)
    {
        return loadTexture(TextureRequest(inputSource, useMipmap, flags));
    }
    
    gl::TextureRef TextureHelper::loadTexture(const TextureRequest &textureRequest)
    {
        TextureData textureData = fetchTextureData(textureRequest);
        
        if (textureData.undefined())
        {
            throw chr::Exception<Texture>("TEXTURE IS UNDEFINED");
        }
        else if ((textureRequest.maxSize.x > 0) && (textureRequest.maxSize.y > 0))
        {
            const Vec2i size = textureData.getSize();
            
            if ((size.x > textureRequest.maxSize.x) || (size.y > textureRequest.maxSize.y))
            {
                throw chr::Exception<Texture>("TEXTURE IS OVER-SIZED (" + toString(size.x) + "x" + toString(size.y) + ")");
            }
        }

        return uploadTextureData(textureData);
    }
    
    TextureData TextureHelper::fetchTextureData(const TextureRequest &textureRequest)
    {
        if (boost::ends_with(textureRequest.inputSource->getFilePathHint(), ".pvr.gz"))
        {
            if (textureRequest.inputSource->isFile())
            {
                return TextureData(textureRequest, PVRHelper::decompressPVRGZ(textureRequest.inputSource->getFilePath()));
            }
            else
            {
                throw chr::Exception<Texture>("PVR.GZ TEXTURES CAN ONLY BE LOADED FROM FILES");
            }
        }
        else if (boost::ends_with(textureRequest.inputSource->getFilePathHint(), ".pvr.ccz"))
        {
            return TextureData(textureRequest, PVRHelper::decompressPVRCCZ(textureRequest.inputSource->loadDataSource()));
        }
        else if (boost::ends_with(textureRequest.inputSource->getFilePathHint(), ".pvr"))
        {
            return TextureData(textureRequest, textureRequest.inputSource->loadDataSource()->getBuffer());
        }
        else
        {
            if (textureRequest.flags & TextureRequest::FLAGS_TRANSLUCENT)
            {
                return TextureData(fetchTranslucentTextureData(textureRequest));
            }
            else if (textureRequest.flags & TextureRequest::FLAGS_POT)
            {
                return TextureData(fetchPowerOfTwoTextureData(textureRequest));
            }
            else
            {
                return TextureData(textureRequest, loadImage(textureRequest.inputSource->loadDataSource()));
            }
        }
        
        return TextureData();
    }
    
    gl::TextureRef TextureHelper::uploadTextureData(const TextureData &textureData)
    {
        gl::TextureRef texture;
        
        if (!textureData.undefined())
        {
            gl::Texture::Format format = textureData.request.getFormat();
            
            /*
             * NECESSARY IN ORDER TO CLEANUP EVENTUAL ERRORS
             */
            while(glGetError() != GL_NO_ERROR)
            {}
            
            switch (textureData.type)
            {
                case TextureData::TYPE_SURFACE:
                    texture = gl::Texture::create(textureData.surface, format);
                    texture->setCleanTexCoords(textureData.maxU, textureData.maxV);
                    break;
                    
                case TextureData::TYPE_IMAGE_SOURCE:
                    texture = gl::Texture::create(textureData.imageSource, format);
                    break;
                    
                case TextureData::TYPE_PVR:
                    texture = PVRHelper::getPVRTexture(textureData.buffer, format.hasMipmapping(), format.getWrapS(), format.getWrapT());
                    break;
                    
                case TextureData::TYPE_DATA:
                    format.setInternalFormat(textureData.glInternalFormat);
                    texture = gl::Texture::create(textureData.data.get(), textureData.glFormat, textureData.width, textureData.height, format);
                    break;
            }
            
            if (glGetError() == GL_OUT_OF_MEMORY)
            {
                throw chr::Exception<Texture>("GL: OUT-OF-MEMORY");
            }
            else if (texture)
            {
                texture->setDeallocator(&TextureHelper::textureDeallocator, texture.get());
                
                LOGD <<
                "TEXTURE UPLOADED: " <<
                textureData.request.inputSource->getFilePathHint() << " | " <<
                texture->getId() << " | " <<
                texture->getWidth() << "x" << texture->getHeight() <<
                endl;
            }
        }
        
        return texture;
    }
    
    void TextureHelper::bindTexture(gl::Texture *texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture->getId());
    }
    
    void TextureHelper::beginTexture(gl::Texture *texture)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);
        
        glBindTexture(GL_TEXTURE_2D, texture->getId());
    }
    
    void TextureHelper::endTexture()
    {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    
    /*
     * XXX: INCLUDES WORKAROUND FOR ci::gl::Texture::getCleanWidth() AND CO. WHICH ARE NOT WORKING ON GL-ES
     */
    void TextureHelper::drawTextureFromCenter(gl::Texture *texture)
    {
        drawTexture(texture, texture->getWidth() * texture->getMaxU() * 0.5f, texture->getHeight() * texture->getMaxV() * 0.5f);
    }
    
    /*
     * XXX: INCLUDES WORKAROUND FOR ci::gl::Texture::getCleanWidth() AND CO. WHICH ARE NOT WORKING ON GL-ES
     */
    void TextureHelper::drawTexture(gl::Texture *texture, float rx, float ry)
    {
        float u = texture->getMaxU();
        float v = texture->getMaxV();
        
        float x1 = -rx;
        float y1 = -ry;
        
        float x2 = x1 + texture->getWidth() * u;
        float y2 = y1 + texture->getHeight() * v;
        
        const float vertices[] =
        {
            x1, y1,
            x2, y1,
            x2, y2,
            x1, y2
        };
        
        const float coords[] =
        {
            0, 0,
            u, 0,
            u, v,
            0, v
        };
        
        glTexCoordPointer(2, GL_FLOAT, 0, coords);
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    /*
     * XXX: ONLY WORKS FOR "TRUE" POWER-OF-TWO TEXTURES
     */
    void TextureHelper::drawTextureInRect(gl::Texture *texture, const Rectf &rect, float ox, float oy)
    {
        const float vertices[] =
        {
            rect.x1, rect.y1,
            rect.x2, rect.y1,
            rect.x2, rect.y2,
            rect.x1, rect.y2
        };
        
        float u1 = (rect.x1 - ox) / texture->getWidth();
        float v1 = (rect.y1 - oy) / texture->getHeight();
        float u2 = (rect.x2 - ox) / texture->getWidth();
        float v2 = (rect.y2 - oy) / texture->getHeight();
        
        const float coords[] =
        {
            u1, v1,
            u2, v1,
            u2, v2,
            u1, v2
        };
        
        glTexCoordPointer(2, GL_FLOAT, 0, coords);
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    void TextureHelper::textureDeallocator(void *refcon)
    {
        gl::Texture *texture = reinterpret_cast<gl::Texture*>(refcon);
        
        LOGD <<
        "TEXTURE DISCARDED: " <<
        texture->getId() <<
        endl;
    }
    
    /*
     * BASED ON https://github.com/cinder/Cinder/blob/v0.8.5/src/cinder/gl/Texture.cpp#L478-490
     */
    TextureData TextureHelper::fetchTranslucentTextureData(const TextureRequest &textureRequest)
    {
        Surface surface(loadImage(textureRequest.inputSource->loadDataSource()));
        
        Channel8u &channel = surface.getChannel(0);
        shared_ptr<uint8_t> data;
        
        if ((channel.getIncrement() != 1) || (channel.getRowBytes() != channel.getWidth() * sizeof(uint8_t)))
        {
            data = shared_ptr<uint8_t>(new uint8_t[channel.getWidth() * channel.getHeight()], checked_array_deleter<uint8_t>());
            uint8_t *dest = data.get();
            int8_t inc = channel.getIncrement();
            int32_t width = channel.getWidth();
            int32_t height = channel.getHeight();
            
            for (int y = 0; y < height; ++y)
            {
                const uint8_t *src = channel.getData(0, y);
                
                for (int x = 0; x < width; ++x)
                {
                    *dest++ = *src;
                    src += inc;
                }
            }
        }
        else
        {
            data = shared_ptr<uint8_t>(channel.getData(), checked_array_deleter<uint8_t>());
        }
        
        return TextureData(textureRequest, data, GL_ALPHA, GL_ALPHA, channel.getWidth(), channel.getHeight());
    }
    
    TextureData TextureHelper::fetchPowerOfTwoTextureData(const TextureRequest &textureRequest)
    {
        /*
         * NO EXTRA DATA-COPYING WILL OCCUR BECAUSE ci::Surface
         * IS EMULATING shared_ptr BEHAVIOR INTERNALLY
         */
        
        Surface src(loadImage(textureRequest.inputSource->loadDataSource()));
        
        int srcWidth = src.getWidth();
        int srcHeight = src.getHeight();
        
        int dstWidth = nextPowerOfTwo(srcWidth);
        int dstHeight = nextPowerOfTwo(srcHeight);
        
        if ((srcWidth != dstWidth) || (srcHeight != dstHeight))
        {
            Surface dst(dstWidth, dstHeight, src.hasAlpha(), src.getChannelOrder());
            
            /*
             * NO NEED TO CLEAR THE WHOLE SURFACE
             */
            ip::fill(&dst, ColorA::zero(), Area(srcWidth + 1, 0, dstWidth, srcHeight));
            ip::fill(&dst, ColorA::zero(), Area(0, srcHeight + 1, srcWidth, dstHeight));
            ip::fill(&dst, ColorA::zero(), Area(srcWidth + 1, srcHeight + 1, dstWidth, dstHeight));
            
            dst.copyFrom(src, Area(0, 0, srcWidth, srcHeight), Vec2i::zero());
            
            /*
             * DUPLICATING THE RIGHT AND BOTTOM EDGES:
             * NECESSARY TO AVOID BORDER ARTIFACTS WHEN THE
             * TEXTURE IS NOT DRAWN AT ITS ORIGINAL SCALE
             */
            dst.copyFrom(src, Area(srcWidth - 1, 0, srcWidth, srcHeight), Vec2i(1, 0));
            dst.copyFrom(src, Area(0, srcHeight - 1, srcWidth, srcHeight), Vec2i(0, 1));
            
            return TextureData(textureRequest, dst, srcWidth / float(dstWidth), srcHeight / float(dstHeight));
        }
        else
        {
            return TextureData(textureRequest, src);
        }
    }
}
