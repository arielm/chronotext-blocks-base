/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "cinder/Cinder.h"

#if defined(CINDER_COCOA_TOUCH)
#include "chronotext/ios/cinder/CinderSketchComplex.h"
namespace chronotext
{
    typedef CinderSketchComplex CinderSketch;
}
#elif defined(CINDER_ANDROID)
#include "chronotext/android/cinder/CinderSketchComplex.h"
namespace chronotext
{
    typedef CinderSketchComplex CinderSketch;
}
#else
#include "chronotext/app/cinder/CinderSketchSimple.h"
namespace chronotext
{
    typedef CinderSketchSimple CinderSketch;
}
#endif
