/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/system/SystemInfoBase.h"

/*
 * REFERENCE: http://theiphonewiki.com/wiki/Models
 */

namespace chr
{
    namespace system
    {
        struct InitInfo
        {};
        
        class Info : public InfoBase
        {
        public:
            enum Generation
            {
                GENERATION_UNDEFINED,
                
                GENERATION_IPAD,
                GENERATION_IPAD_2,
                GENERATION_IPAD_3,
                GENERATION_IPAD_4,
                GENERATION_IPAD_AIR,
                GENERATION_IPAD_AIR_2,
                
                GENERATION_IPAD_MINI_1G,
                GENERATION_IPAD_MINI_2,
                GENERATION_IPAD_MINI_3,
                
                GENERATION_IPHONE_3GS,
                GENERATION_IPHONE_4,
                GENERATION_IPHONE_4S,
                GENERATION_IPHONE_5,
                GENERATION_IPHONE_5C,
                GENERATION_IPHONE_5S,
                GENERATION_IPHONE_6,
                GENERATION_IPHONE_6_PLUS,
                
                GENERATION_IPOD_TOUCH_3G,
                GENERATION_IPOD_TOUCH_4G,
                GENERATION_IPOD_TOUCH_5G
            };
            
            std::string model;
            std::string machine;
            Generation generation;
            
            bool isPodTouch;
            bool isIPhone;
            bool isIPad;
            bool isIPadMini;
            bool isSimulator;
        };
    }
}
