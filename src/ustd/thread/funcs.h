#pragma once

#include "ustd/time.h"

namespace ustd::thread
{

pub fn sleep    (time::Duration dur) noexcept -> time::Duration;
pub fn sleep_ms (u32 ms)             noexcept -> u32;
pub fn sleep_to(time::Instant time)  noexcept -> time::Duration;

}
