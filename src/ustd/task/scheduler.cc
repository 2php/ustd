#include "config.inl"

namespace ustd::task
{

pub Scheduler::Scheduler()
{}

pub Scheduler::~Scheduler()
{}

pub Scheduler::Scheduler(Scheduler&& other) noexcept
    : _mtx(as_mov(other._mtx))
    , _cnd(as_mov(other._cnd))
    , _tasks(as_mov(other._tasks))
{}

pub fn Scheduler::add(ITask& task) noexcept -> void {
    mut lock = _mtx.lock();
    _tasks.push(&task);
    _cnd.notify_all();
}

pub fn Scheduler::run() noexcept -> void {

    while (true) {
        // get task
        let task_opt = [&]() -> Option<ITask&> {
            mut lock = _mtx.lock();

            while (!_tasks.is_empty()) {
                for (mut& ptask : _tasks.into_iter()) {
                    if (ptask->is_ready()) {
                        // swap
                        mut& plast = _tasks[_tasks._size - 1];
                        ustd::swap(ptask, plast);
                        _tasks.pop();

                        // sleep 10ms
                        thread::sleep_ms(10);
                        return Option<ITask&>::Some(*plast);
                    }
                }

                let timeout_ms = 1000;
                _cnd.wait_timeout_ms(lock, timeout_ms);
            };

            return Option<ITask&>::None();

        }();

        // check
        if (task_opt.is_none()) {
            break;
        }
        mut& task = task_opt._val;

        // run
        let ret = task.schedule();

        // notify
        mut lock = _mtx.lock();
        _cnd.notify_all();

        if (!ret) {
            break;
        }
    }

    return;
}

pub fn Scheduler::async_run(str thr_name) noexcept -> thread::JoinHandle<void> {
    let thr_info = thread::Builder().set_name(thr_name);

    mut thr_join = thr_info.spawn([&]() {
        this->run();
    });
    return as_mov(thr_join);
}

unittest(task) {
    class Task : public ITask {
    public:
        str _name;

        Task(str name) : _name(name)
        {}

        fn exec() -> void override {
            log::info("task: {}", _name);
            thread::sleep_ms(100);
        }
    };

    Task tasks[] = {
        Task("t0"),
        Task("t1"),
        Task("t2"),
        Task("t3"),
    };

    tasks[1].add_depend(tasks[0]);
    tasks[2].add_depend(tasks[0]);
    tasks[3].add_depend(tasks[1]);
    tasks[3].add_depend(tasks[2]);

    mut scheduler = Scheduler();
    scheduler.add(tasks[0]);
    scheduler.add(tasks[1]);
    scheduler.add(tasks[2]);
    scheduler.add(tasks[3]);

    scheduler.async_run("ustd::task::scheduler::test");
}

}
