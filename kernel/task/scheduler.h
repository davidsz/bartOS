#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

namespace task {

class Task;

class IScheduler
{
    virtual TaskCreated(Task *task) = 0;
    virtual TaskExited(Task *) = 0;
    virtual TaskActivated(Task *task) = 0;
}

}; // namespace task

// TODO: Implement a task scheduler module
void set_scheduler(IScheduler *);

#endif // TASK_SCHEDULER_H
