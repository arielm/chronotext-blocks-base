/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/android/system/SystemManager.h"
#include "chronotext/Context.h"
#include "chronotext/utils/Utils.h"

#include <sys/system_properties.h>
#include <sys/utsname.h>

using namespace std;
using namespace ci;

namespace chr
{
    namespace system
    {
        namespace android
        {
            string getProperty(const char *name)
            {
                static char tmp[256];
                auto len = __system_property_get(name, tmp);
                
                return string(tmp, len);
            }
        }
        
        // ---
        
        Manager::Manager()
        {
            setup(); // REMINDER: INVOCATION FROM BASE-CONSTRUCTOR DISCARDS INHERITANCE
        }
        
        Manager::~Manager()
        {
            shutdown(); // REMINDER: INVOCATION FROM BASE-DESTRUCTOR DISCARDS INHERITANCE
        }
        
        // ---
        
        string Manager::getIpAddress(bool maskForBroadcast)
        {
            return "";
        }
        
        // ---
        
        void Manager::updateInfo()
        {
            /*
             * XXX: ORDER OF EXECUTION MATTERS!
             */
            
            info.model = getModel();
            info.manufacturer = getManufacturer();
            
            ManagerBase::updateInfo();
        }
        
        string Manager::getOsVersionString()
        {
            return android::getProperty("ro.build.version.release");
        }
        
        string Manager::getDeviceString()
        {
            return info.model + " [" + info.manufacturer + "]";
        }
        
        string Manager::getModel()
        {
            return android::getProperty("ro.product.model");
        }
        
        string Manager::getManufacturer()
        {
            return android::getProperty("ro.product.manufacturer");
        }
    }
}
