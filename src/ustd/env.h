#pragma once

#include "ustd/core.h"
#include "ustd/os.h"

namespace ustd::env
{

static constexpr str os_name = ustd::os::name;

pub fn var    (str key)             -> str;
pub fn set_var(str key, str val)    -> void;

}
