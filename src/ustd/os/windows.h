#pragma once

#include "ustd/core.h"

namespace ustd::os
{

namespace windows 
{
static constexpr let $name      = str("windows");

static constexpr let mtx_size   = usize(40);
static constexpr let cnd_size   = usize(8);

pub fn get_error() noexcept->Error;

}

}
