#pragma once

#include "ustd/os/os_posix.h"
#include "ustd/os/os_unix.h"
#include "ustd/os/os_win32.h"

namespace ustd::os
{

#if defined(_WIN32)
#define USTD_OS_WINDOWS
using namespace windows;
#endif

#if defined(__linux) || defined(__linux__)
#define USTD_OS_LINUX
using namespace linux;
#endif

#if defined(__APPLE__)
#define USTD_OS_MACOS
using namespace macos;
#endif

#if defined(__unix) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#define USTD_OS_UNIX
using namespace unix;
#endif

}
