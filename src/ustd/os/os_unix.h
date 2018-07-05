#pragma once

#include "ustd/core.h"

namespace ustd::os
{

namespace unix
{
}

namespace linux 
{
static constexpr str $name = "linux";
static constexpr let mtx_size   = u64(40);
static constexpr let cnd_size   = u64(8);
}

namespace macos
{
static constexpr str $name = "macos";
static constexpr let mtx_size   = u64(56);
static constexpr let cnd_size   = u64(40);
}

}
