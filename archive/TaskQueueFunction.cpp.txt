#include "TaskQueueFunction.h"
#include <algorithm>

TaskQueueFunction::TaskQueueFunction()
: TaskWrapper("TaskQueueFunction", 4096, 1, 1) {}

int TaskQueueFunction::addJob(Job job) {
    int id = nextId++;
    jobs.push_back({id, job, true, false});
    Serial.println("TaskQueue neuer job");
    return id;
}

void TaskQueueFunction::pauseJob(int id) {
    if (auto* job = findJob(id)) {
        job->active = false;
    }
    Serial.println("Pause Job");
}

void TaskQueueFunction::resumeJob(int id) {
    if (auto* job = findJob(id)) {
        job->active = true;
    }
    Serial.println("Resume Job");
}

void TaskQueueFunction::removeJob(int id) {
    if (auto* job = findJob(id)) {
        job->toRemove = true;
    }
}

TaskQueueFunction::JobInfo* TaskQueueFunction::findJob(int id) {
    for (auto& job : jobs) {
        if (job.id == id) return &job;
    }
    return nullptr;
}

void TaskQueueFunction::run() {
    while (true) {
        for (auto& job : jobs) {
            if (job.active && !job.toRemove) {
                job.func();
            }
        }

        // markierte Jobs entfernen
        jobs.erase(
            std::remove_if(jobs.begin(), jobs.end(),
                           [](const JobInfo& j){ return j.toRemove; }),
            jobs.end()
        );

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
