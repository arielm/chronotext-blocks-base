/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/cinder/CinderDelegateBase.h"

#include "cinder/app/TouchEvent.h"

#include <boost/asio.hpp>

namespace chr
{
    class CinderDelegate : public CinderDelegateBase
    {
    public:
        static std::atomic<bool> LOG_VERBOSE;
        static std::atomic<bool> LOG_WARNING;

        bool init();
        void launch();
        void setup(const WindowInfo &windowInfo);
        void shutdown();
        
        void resize(const ci::Vec2f &size);
        void update();
        void draw();

        void touchesBegan(ci::app::TouchEvent event);
        void touchesMoved(ci::app::TouchEvent event);
        void touchesEnded(ci::app::TouchEvent event);

        void messageFromBridge(int what, const std::string &body = "") final;
        void handleEvent(int eventId) final;

        void enableAccelerometer( float updateFrequency = 30, float filterFactor = 0.1f) final;
        void disableAccelerometer() final;

        bool isEmulated() const final;
        const WindowInfo& getWindowInfo() const final;
        
        double elapsedSeconds() const;
        int elapsedFrames() const;

        void start(CinderSketch::Reason reason);
        void stop(CinderSketch::Reason reason);
        
        void handleAcceleration(const ci::Vec3f &acceleration);
        
    protected:
        CinderSketch *sketch = nullptr;

        ci::Timer timer;
        int frameCount = 0;
        bool forceResize = false;
        
        WindowInfo windowInfo;

        AccelEvent::Filter accelFilter;

        std::shared_ptr<boost::asio::io_service> io;
        std::shared_ptr<boost::asio::io_service::work> ioWork;
        
        void startIOService();
        void stopIOService();
    };
}
