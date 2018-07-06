#pragma once

#include "ustd/core.h"
#include "ustd/os/os_posix.h"

namespace ustd::os
{

namespace windows 
{

static constexpr let name       = str("windows");
static constexpr let shared_ext = str("dll");
static constexpr let mtx_size   = u64(40);
static constexpr let cnd_size   = u64(8);

pub fn get_error() noexcept->Error;
}

}
