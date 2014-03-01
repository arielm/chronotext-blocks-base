/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * STUB FOR RUNNING A CinderSketch
 * ON THE DESKTOP (OSX OR WINDOW)
 */

#include "chronotext/cinder/CinderApp.h"

#include "Sketch.h"

using namespace std;
using namespace ci;
using namespace app;
using namespace chr;

class Application : public CinderApp
{
public:
    Application();
    
    void prepareSettings(Settings *settings);
    void keyDown(KeyEvent event);
};

Application::Application()
{
    sketch = new Sketch(this);
}

void Application::prepareSettings(Settings *settings)
{
    settings->disableFrameRate();
    settings->enableHighDensityDisplay();
    settings->setResizable(false);

//  settings->setWindowSize(960, 640); // IPHONE RETINA / LANDSCAPE
    settings->setWindowSize(1024, 768); // IPAD / LANDSCAPE
//  settings->setWindowSize(800, 480); // ANDROID WVGA800 / LANDSCAPE
    
//  settings->setWindowSize(640, 960); // IPHONE RETINA / PORTRAIT
//  settings->setWindowSize(768, 1024); // IPAD / PORTRAIT
//  settings->setWindowSize(480, 800); // ANDROID WVGA800 / PORTRAIT

}

void Application::keyDown(KeyEvent event)
{
    switch (event.getCode())
    {
        case KeyEvent::KEY_ESCAPE:
            quit();
            break;
    }
}

CINDER_APP_NATIVE(Application, RendererGl(RendererGl::AA_NONE))
