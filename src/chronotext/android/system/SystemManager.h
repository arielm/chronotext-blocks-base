/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "cinder/Cinder.h"

#if !defined(CINDER_ANDROID)

#error UNSUPPORTED PLATFORM

#endif

#include "chronotext/system/SystemManagerBase.h"

namespace chronotext
{
    namespace system
    {
        namespace android
        {
            /*
             * THE SAME PROPERTIES THAT CAN BE QUERIED VIA ADB, E.G.
             * adb shell getprop ro.product.manufacturer
             */
            
            std::string property(const char *name);
        }
        
        class Manager : public ManagerBase
        {
        public:
            Manager();
            ~Manager() override;
            
            std::string getIpAddress(bool maskForBroadcast = false); // TODO
            
        protected:
            void updateInfo() override;
            
            std::string getOsVersionString() override;
            std::string getDeviceString() override;
            
            std::string getModel();
            std::string getManufacturer();
        };
    }
}
