/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/incubator/ui/Shape.h"

namespace chr
{
    typedef std::shared_ptr<class Container> ContainerRef;

    class Container : public Shape
    {
    public:
        std::vector<ShapeRef> components;

        Container();
        Container(std::shared_ptr<Shape::Style> style);
        
        void setWidth(float newWidth);
        void setHeight(float newHeight);
        void setAutoWidth(bool autoWidth);
        void setAutoHeight(bool autoHeight);
        void setPadding(float left, float top, float right, float bottom);
        
        float getWidth();
        float getHeight();
        Shape* getComponentByTag(int tag);

        void addComponent(ShapeRef component);
        void requestLayout();
        
        void draw();
        
    protected:
        bool layoutRequest;
        
        void drawComponents();
        float mergedMargin(float previousMargin, float nextMargin);
    };
}
