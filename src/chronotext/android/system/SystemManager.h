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
#   error UNSUPPORTED PLATFORM
#endif

// ---

#include "chronotext/system/SystemManagerBase.h"

namespace chr
{
    namespace system
    {
        namespace android
        {
            /*
             * THE SAME PROPERTIES QUERIABLE VIA ADB, E.G.
             * adb shell getprop ro.product.manufacturer
             */
            const std::string getProperty(const char *name);
        }
        
        class Manager : public ManagerBase
        {
        public:
            void setup(const InitInfo &initInfo) final;

            std::string getIpAddress(bool maskForBroadcast = false) final;
            
        protected:
            void updateInfo() final;

            std::array<int, 3> getOsVersion() final;
            std::string getOsVersionString() final;
            std::string getDeviceString() final;
            
            int getSdkVersion();
            std::string getModel();
            std::string getManufacturer();
        };
    }
}
