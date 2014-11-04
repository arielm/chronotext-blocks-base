/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * INSPIRED BY javax.media.Clock
 * http://docs.oracle.com/javame/config/cdc/opt-pkgs/api/jsr927/javax/media/Clock.html
 */

#include "Clock.h"

using namespace std;

namespace chronotext
{
    Clock::Clock()
    :
    timeBase(new DefaultTimeBase()),
    timeBaseIsOwned(true),
    mst(0),
    rate(1),
    state(STOPPED)
    {}
    
    Clock::Clock(TimeBase *timeBase)
    :
    timeBase(timeBase),
    timeBaseIsOwned(false),
    mst(0),
    rate(1),
    state(STOPPED)
    {}
    
    Clock::Clock(shared_ptr<Clock> master)
    :
    timeBase(master.get()),
    timeBaseIsOwned(false),
    master(master),
    mst(0),
    rate(1),
    state(STOPPED)
    {
        start();
    }
    
    Clock::~Clock()
    {
        if (timeBaseIsOwned)
        {
            delete timeBase;
        }
    }
    
    void Clock::start()
    {
        tbst = timeBase->getTime();
        state = STARTED;
    }
    
    void Clock::stop()
    {
        if (state != STOPPED)
        {
            mst = getTime();
            state = STOPPED;
        }
    }
    
    double Clock::getTime()
    {
        return mst + ((state == STOPPED) ? 0 : (timeBase->getTime() - tbst) * rate);
    }
    
    void Clock::setTime(double now)
    {
        if (state == STOPPED)
        {
            mst = now;
        }
        else
        {
            throw EXCEPTION(Clock, "CLOCK SHOULD BE STOPPED");
        }
    }
    
    int Clock::getState()
    {
        return state;
    }
    
    void Clock::setRate(double factor)
    {
        if (state == STOPPED)
        {
            rate = factor;
        }
        else
        {
            throw EXCEPTION(Clock, "CLOCK SHOULD BE STOPPED");
        }
    }
    
    void Clock::restart()
    {
        stop();
        setTime(0);
        start();
    }
}
