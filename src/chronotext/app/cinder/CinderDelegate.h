/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/cinder/CinderSketch.h"
#include "chronotext/system/SystemInfo.h"
#include "chronotext/InputSource.h"

#include "cinder/app/AppNative.h"

namespace chr
{
    class CinderDelegate : public ci::app::AppNative
    {
    public:
        CinderDelegate();

        CinderSketch* getSketch();

        virtual void sketchCreated(CinderSketch *sketch) {}
        virtual void sketchDestroyed(CinderSketch *sketch) {}

        virtual void action(int actionId) {}
        virtual void receiveMessageFromSketch(int what, const std::string &body) {}
        virtual void sendMessageToSketch(int what, const std::string &body = "");

        virtual void applyDefaultSettings(Settings *settings);
        virtual void applySettings(Settings *settings) {}
        void prepareSettings(Settings *settings) final;

        void setup() override;
        void shutdown() override;
        void resize() override;
        
        void update() override;
        void draw() override;

        void accelerated(AccelEvent event);

        void mouseDown(ci::app::MouseEvent event);
        void mouseUp(ci::app::MouseEvent event);
        void mouseDrag(ci::app::MouseEvent event);
        
        void touchesBegan(ci::app::TouchEvent event);
        void touchesMoved(ci::app::TouchEvent event);
        void touchesEnded(ci::app::TouchEvent event);
        
        bool isEmulated() const;
        const WindowInfo& getWindowInfo() const;
        
        void emulate(Settings *settings, EmulatedDevice &device, DisplayInfo::Orientation orientation = DisplayInfo::ORIENTATION_DEFAULT);
        bool emulate(Settings *settings, const std::string &deviceKey, DisplayInfo::Orientation orientation = DisplayInfo::ORIENTATION_DEFAULT);
        bool loadEmulators(chr::InputSource::Ref inputSource);
        
    protected:
        CinderSketch *sketch;

        system::InitInfo initInfo;
        WindowInfo windowInfo;
        
        std::map<std::string, std::shared_ptr<EmulatedDevice>> emulators;
        
        int startCount;
        int updateCount;
        
        void setSketch(CinderSketch *sketch);
       
        void start();
        void stop();
    };
}
