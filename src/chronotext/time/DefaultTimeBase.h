/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/time/TimeBase.h"

#include "cinder/Timer.h"

namespace chronotext
{
    class DefaultTimeBase : public TimeBase
    {
    public:
        DefaultTimeBase()
        {
            timer.start();
        }
        
        double getTime()
        {
            return timer.getSeconds();
        }
        
    protected:
        ci::Timer timer;
    };
}
