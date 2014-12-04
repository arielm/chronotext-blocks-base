/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "cinder/gl/gl.h"

#include <vector>

namespace chr
{
    struct TexturedTriangleStrip
    {
        std::vector<ci::Vec2f> vertices;
        
        void clear()
        {
            vertices.clear();
        }
        
        bool empty() const
        {
            return vertices.empty();
        }
        
        void draw() const
        {
            if (!vertices.empty())
            {
                int stride = sizeof(ci::Vec2f) * 2;
                auto pointer = vertices.data();
                
                glVertexPointer(2, GL_FLOAT, stride, pointer);
                glTexCoordPointer(2, GL_FLOAT, stride, pointer + 1);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() >> 1);
            }
        }
    };
}
