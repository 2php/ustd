#include "config.inl"

namespace ustd::task
{

pub ITask::ITask() noexcept
    : _status(status_t::None)
{}

pub ITask::~ITask() noexcept
{}

pub fn ITask::get_depend() noexcept -> ITask* {
    if (_status != status_t::None) {
        return this;
    }

    for(mut ptask: _depends.into_iter()) {
        if (ptask->_status != status_t::Success) {
            return ptask;
        }
    }
    return nullptr;
}

pub fn ITask::is_ready() noexcept -> bool {
    let depend = get_depend();
    return depend == nullptr;
}

pub fn ITask::add_depend(ITask& task) noexcept -> void {
    _depends.push(&task);
}

pub fn ITask::schedule() noexcept -> bool {
    _status = status_t::Running;
    try {
        this->exec();
        _status = status_t::Success;
        return true;
    }
    catch(...) {
        _status = status_t::Failed;
        return false;
    }

}

}
