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
 * TODO:
 *
 * 1) TEST AND DEVELOP FURTHER:
 *    - SEE MANY "INNER" TODOS IN TaskManger AND Task
 *
 * 2) PROPERLY HANDLE THE TERMINATION OF RUNNING-TASKS WHEN CONTEXT IS BEING SHUT-DOWN
 *    - SEE RELATED TODOS IN Context AND CinderDelegate
 *
 * 3) TRY TO USE NEW C++11 FEATURES LIKE std::future AND std::async
 *
 * 4) CREATE TESTS AND SAMPLES PROJECTS
 */

#pragma once

#include "chronotext/os/Task.h"

namespace chr
{
    class TaskManager : public std::enable_shared_from_this<TaskManager>
    {
    public:
        static std::shared_ptr<TaskManager> create()
        {
            return std::shared_ptr<TaskManager>(new TaskManager()); // XXX: std::make_shared ONLY WORKS WITH PUBLIC CONSTRUCTORS
        }
        
        /*
         * THE RETURNED POINTER IS NOT INTENDED FOR STORAGE
         */
        Task* getTask(int taskId);
        
        /*
         * RETURNS 0 IF THE TASK CAN'T BE REGISTERED
         *
         * CAUSES:
         *
         * - THE TASK IS ALREADY REGISTERED (NOTE: TASKS ARE NOT REUSABLE)
         * - Task::init() RETURNED FALSE
         */
        int registerTask(std::shared_ptr<Task> task);
        
        /*
         * RETURNS FALSE IF THE TASK CAN'T BE ADDED
         *
         * CAUSES:
         *
         * - NO TASK IS REGISTERED WITH THIS ID
         * - THE TASK HAS ALREADY BEEN ADDED
         * - IO-SERVICE IS NOT DEFINED
         */
        bool addTask(int taskId, bool forceSync = false);
        
        inline bool addTask(std::shared_ptr<Task> &&task, bool forceSync = false)
        {
            int taskId = registerTask(std::forward<std::shared_ptr<Task>>(task));
            return taskId ? addTask(taskId, forceSync) : false;
        }
        
        /*
         * RETURNS FALSE IF THE TASK CAN'T BE CANCELLED
         *
         * CAUSES:
         *
         * - NO TASK IS REGISTERED WITH THIS ID
         *
         * ASYNCHRONOUS TASKS ONLY:
         * - THE TASK IS ALREADY AWAITING CANCELLATION
         */
        bool cancelTask(int taskId);
        
    protected:
        int taskCount;
        std::map<int, std::shared_ptr<Task>> tasks;
        
        boost::mutex _mutex;
        
    private:
        friend class Task;
        
        TaskManager();
        TaskManager(const TaskManager &other) = delete;
        
        /*
         * RETURNS FALSE IF THE LAMBDA CAN'T BE POSTED
         *
         * CAUSES:
         *
         * - IO-SERVICE IS NOT DEFINED
         * - THE CONTEXT IS BEING SHUT-DOWN (TODO)
         */
        bool post(std::function<void()> &&fn, bool forceSync = false);
        
        void endTask(int taskId);
    };
}
