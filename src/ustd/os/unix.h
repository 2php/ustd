#pragma once

#include "ustd/core.h"
#include "ustd/os/posix.h"

namespace ustd::os
{

namespace unix
{
pub fn get_error() noexcept-> Error;
}

namespace linux 
{
using namespace unix;
static constexpr let name       = str("linux");
static constexpr let shared_ext = str("so");

static constexpr let mtx_size   = u64(40);
static constexpr let cnd_size   = u64(8);
}

namespace macos
{
using namespace unix;
static constexpr let name       = str("macos");
static constexpr let shared_ext = str("dylib");
static constexpr let mtx_size   = u64(56);
static constexpr let cnd_size   = u64(40);
}

}
