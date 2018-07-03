#pragma once

namespace ustd::task
{

class Scheduler;

class ITask
{
public:
    enum class status_t
    {
        None,
        Running,
        Success,
        Failed,
    };

    constexpr static let $capacity = 8u;
    using depends_t = FixedList<ITask*, $capacity>;

    volatile status_t   _status;
    depends_t           _depends;

    pub virtual ~ITask() noexcept;

    pub virtual fn add_depend(ITask& task)  noexcept -> void;
    pub virtual fn get_depend()             noexcept -> ITask*;
    pub virtual fn is_ready()               noexcept -> bool;

    pub virtual fn schedule()               noexcept -> bool;
    pub virtual fn exec()                            -> void = 0;
protected:
    friend class Scheduler;

    pub ITask() noexcept;

};

}

