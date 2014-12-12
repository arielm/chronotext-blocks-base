/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * ADDITIONAL CONTRIBUTORS: MICHAEL BOCCARA
 */

/*
 *  TODO:
 *
 * 1) TEST AND DEVELOP FURTHER:
 *    - SEE MANY "INNER" TODOS IN TaskManger AND Task
 *
 * 2) TRY TO REPLACE cinder::sleep() BY:
 *    - boost::this_thread::sleep()
 *    - OR A PURE C++11 STL SOLUTION
 *
 * 3) TRY TO USE std::mutex AND std::lock_guard INSTEAD OF boost::mutex AND boost::mutex::scoped_lock
 *
 * 4) SEE IF THE NEW C++11 std::thread_local CAN BE USED
 *
 * 5) IMPLEMENT THREAD ATTACHMENT/DETACHMENT TO/FROM JAVA ON ANDROID
 *    - STUDY JNI'S AttachCurrentThread / DetachCurrentThread
 */

#pragma once

#include "cinder/Thread.h"

#include <boost/thread/mutex.hpp>

namespace chr
{
    class TaskManager;
    
    class Task
    {
    public:
        static const bool VERBOSE;
        
        /*
         * TODO: AVOID "MANDATORY CONSTRUCTOR"
         *
         * I.E. ANOTHER METHOD SHOULD BE USED TO INITIALIZE THE DEFAULT-VALUES
         */
        Task();
        
        virtual bool init() { return true; }
        virtual void shutdown() {}
        virtual void run() = 0;
        
        int getId() const;
        bool canBeRemoved();
        bool isCancelRequired();
        
        void sleep(float milliseconds);
        
    protected:
        int taskId;
        std::shared_ptr<TaskManager> manager;
        
        bool synchronous;
        bool started;
        bool ended;
        bool cancelRequired;
        
        std::thread _thread;
        boost::mutex _mutex;
        
        virtual ~Task();
        
        bool post(std::function<void()> &&fn);
        
    private:
        friend class TaskManager;
        
        bool start();
        bool cancel();
        void detach();
        
        bool performInit(std::shared_ptr<TaskManager> manager, int taskId);
        void performShutdown();
        void performRun();
    };
}
