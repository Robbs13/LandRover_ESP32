#ifndef TASKQUEUEFUNCTION_H
#define TASKQUEUEFUNCTION_H

#include "TaskWrapper.h"
#include <vector>
#include <functional>

class TaskQueueFunction : public TaskWrapper {
public:
    using Job = std::function<void()>;

    TaskQueueFunction();

    int addJob(Job job);     // gibt eine eindeutige Job-ID zurück
    void pauseJob(int id);
    void resumeJob(int id);
    void removeJob(int id);

protected:
    void run() override;

private:
    struct JobInfo {
        int id;
        Job func;
        bool active;
        bool toRemove;
    };

    std::vector<JobInfo> jobs;
    int nextId = 0;

    JobInfo* findJob(int id);
};

#endif
