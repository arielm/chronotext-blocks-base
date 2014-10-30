/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "cinder/app/AppNative.h"

#include "chronotext/app/system/EmulatedDevice.h"
#include "chronotext/cinder/CinderSketch.h"
#include "chronotext/InputSource.h"

namespace chronotext
{
    class CinderApp : public ci::app::AppNative
    {
    public:
        CinderApp();
        virtual ~CinderApp() {}

        CinderSketch* getSketch();

        virtual CinderSketch* createSketch() = 0;
        virtual void destroySketch();
        
        virtual void action(int actionId) {}
        virtual void receiveMessageFromSketch(int what, const std::string &body) {}
        virtual void sendMessageToSketch(int what, const std::string &body);
        
        void applyDefaultSettings(Settings *settings);
        
        void setup();
        void shutdown();
        void resize();
        
        void update();
        void draw();

        void accelerated(AccelEvent event);

        void mouseDown(ci::app::MouseEvent event);
        void mouseUp(ci::app::MouseEvent event);
        void mouseDrag(ci::app::MouseEvent event);
        
        void touchesBegan(ci::app::TouchEvent event);
        void touchesMoved(ci::app::TouchEvent event);
        void touchesEnded(ci::app::TouchEvent event);
        
        bool isEmulated() const;
        WindowInfo getWindowInfo() const;
        DisplayInfo getDisplayInfo() const;
        
        void emulate(Settings *settings, EmulatedDevice &device, DisplayInfo::Orientation orientation = DisplayInfo::ORIENTATION_DEFAULT);
        bool emulate(Settings *settings, const std::string &deviceKey, DisplayInfo::Orientation orientation = DisplayInfo::ORIENTATION_DEFAULT);
        bool loadEmulators(chr::InputSourceRef source);
        
    protected:
        CinderSketch *sketch;

        bool emulated;
        EmulatedDevice emulatedDevice;
        std::map<std::string, std::shared_ptr<EmulatedDevice>> emulators;
        
        DisplayInfo realDisplayInfo;
        WindowInfo realWindowInfo;
        
        int startCount;
        int updateCount;

        void updateRealDisplayInfo();
        void updateRealWindowInfo();
        
        void start();
        void stop();
    };
}
