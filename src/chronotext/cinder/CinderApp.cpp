/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/cinder/CinderApp.h"
#include "chronotext/utils/Utils.h"

using namespace std;
using namespace ci;
using namespace app;

namespace chronotext
{
    CinderApp::CinderApp()
    :
    startCount(0),
    updateCount(0)
    {}

    void CinderApp::setup()
    {
        /*
         * App::privateUpdate__ HACKING: SEE COMMENT IN CinderApp::update
         */
        io_service().post([this]{ sketch->clock().update(); });
        
        sketch->setIOService(io_service());
        sketch->timeline().stepTo(0);
        sketch->setup();
    }
    
    void CinderApp::shutdown()
    {
        stop();
        sketch->stop(CinderSketch::FLAG_APP_HIDDEN);
        sketch->shutdown();
        delete sketch;
    }
    
    void CinderApp::resize()
    {
        sketch->resize();
        
        if (startCount == 0)
        {
            start();
            sketch->start(CinderSketch::FLAG_APP_SHOWN);
            startCount++;
        }
    }
    
    void CinderApp::update()
    {
        /*
         * App::privateUpdate__ HACKING:
         * WE MUST UPDATE THE CLOCK AT THE BEGINNING OF THE FRAME,
         * AND WE NEED THIS TO TAKE PLACE BEFORE THE FUNCTIONS
         * "POSTED" DURING CinderSketch::update ARE "POLLED"
         */
        io_service().post([this]{ sketch->clock().update(); });
        
        /*
         * MUST BE CALLED BEFORE Sketch::update
         * ANY SUBSEQUENT CALL WILL RETURN THE SAME TIME-VALUE
         *
         * NOTE THAT getTime() COULD HAVE BEEN ALREADY CALLED
         * WITHIN ONE OF THE PREVIOUSLY "POLLED" FUNCTIONS
         */
        double now = sketch->clock().getTime();
        
        sketch->update();
        sketch->timeline().stepTo(now); // WE CAN'T CONTROL THE APP'S TIMELINE SO WE NEED OUR OWN
        updateCount++;
    }
    
    void CinderApp::draw()
    {
        if (updateCount == 0)
        {
            update(); // HANDLING CASES WHERE draw() IS INVOKED BEFORE update()
        }
        
        sketch->draw();
    }
    
    void CinderApp::mouseDown(MouseEvent event)
    {
        sketch->addTouch(0, event.getX(), event.getY());
    }
    
    void CinderApp::mouseDrag(MouseEvent event)
    {
        sketch->updateTouch(0, event.getX(), event.getY());
    }
    
    void CinderApp::mouseUp(MouseEvent event)
    {
        sketch->removeTouch(0, event.getX(), event.getY());
    }
    
    void CinderApp::touchesBegan(TouchEvent event)
    {
        for (auto &touch : event.getTouches())
        {
            sketch->addTouch(touch.getId() - 1, touch.getX(), touch.getY());
        }
    }
    
    void CinderApp::touchesMoved(TouchEvent event)
    {
        for (auto &touch : event.getTouches())
        {
            sketch->updateTouch(touch.getId() - 1, touch.getX(), touch.getY());
        }
    }
    
    void CinderApp::touchesEnded(TouchEvent event)
    {
        for (auto &touch : event.getTouches())
        {
            sketch->removeTouch(touch.getId() - 1, touch.getX(), touch.getY());
        }
    }
    
    void CinderApp::accelerated(AccelEvent event)
    {
        sketch->accelerated(event);
    }
    
    void CinderApp::sendMessageToSketch(int what, const string &body)
    {
        sketch->sendMessage(Message(what, body));
    }
    
    void CinderApp::emulate(Settings *settings, const EmulatedDevice &device)
    {
        /*
         * POINTLESS TO ALLOW RESIZE WHEN EMULATING
         * (IT WOULD ALSO MAKE THE CODE MORE COMPLEX...)
         */
        settings->setResizable(false);

        settings->setWindowSize(device.size);

        WindowInfo windowInfo;
        windowInfo.size = device.size;
        windowInfo.contentScale = device.contentScale;
        windowInfo.diagonal = device.diagonal;
        windowInfo.density = (device.diagonal == 0) ? 0 : (device.size.length() / device.diagonal);
        
        SystemInfo::instance().setWindowInfo(windowInfo);
    }
    
    void CinderApp::start()
    {
        sketch->clock().start();
    }
    
    void CinderApp::stop()
    {
        sketch->clock().stop();
        LOGI << "AVERAGE FRAME-RATE: " << getAverageFps() << " FPS" << endl;
    }
}
