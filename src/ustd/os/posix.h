#pragma once

#include "ustd/core.h"

namespace ustd::os
{

enum class Error: i32
{
    Success,
    NotFound,
    PermissionDenied,
    ConnectionRefused,
    ConnectionReset,
    ConnectionAborted,
    NotConnected,
    AddrInUse,
    AddrNotAvailable,
    BrokenPipe,
    AlreadyExists,
    WouldBlock,
    InvalidInput,
    InvalidData,
    TimedOut,
    WriteZero,
    Interrupted,
    UnexpectedEof,
    Other,
};

pub fn to_str(Error e) noexcept -> str;

template <class T >
using Result = ustd::Result<T, Error>;

}
