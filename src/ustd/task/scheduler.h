#pragma once

#include "ustd/thread.h"
#include "ustd/sync.h"
#include "ustd/task/task.h"

namespace ustd::task
{

class Scheduler final
{
public:
    constexpr static let $capacity = 64;
    using tasks_t = FixedList<ITask*, $capacity>;

    sync::Mutex     _mtx;
    sync::CondVar   _cnd;
    tasks_t         _tasks;

    pub Scheduler();
    pub ~Scheduler();
    pub Scheduler(Scheduler&& other) noexcept;

    pub fn add(ITask& task) noexcept -> void;

    pub fn run()                    noexcept -> void;
    pub fn async_run(str thr_name)  noexcept->thread::JoinHandle<void>;
};

}