/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * DEVELOPED AND TESTED VIA THE ContextRework PROJECT IN THE chronotext-playground REPOSITORY:
 * https://github.com/arielm/chronotext-playground/blob/master/Sketches/ContextRework/src/TestingMemory.h
 *
 *
 * ADDITIONAL PLATFORM-RELATED INFO:
 *
 * - chronotext/cocoa/system/MemoryManager.mm
 * - chronotext/android/system/MemoryManager.cpp
 */

#pragma once

#include "chronotext/system/MemoryInfo.h"

namespace chr
{
    namespace memory
    {
        class ManagerBase
        {
        public:
            virtual ~ManagerBase() {}
            
            virtual void setup() {}
            virtual void shutdown() {}

            virtual Info getInfo() = 0;
            
            /*
             * HOW MUCH MEMORY WAS USED BETWEEN "BEFORE" AND "AFTER"
             */
            virtual int64_t compare(const Info &before, const Info &after) { return 0; }
        };
    }
}