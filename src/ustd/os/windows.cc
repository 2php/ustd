
#include "config.inl"

#ifdef USTD_OS_WINDOWS

extern "C" fn GetLastError() -> i32;

namespace ustd::os::windows
{



pub fn get_error() noexcept -> Error {
    // @see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx

    let eid = ::GetLastError();

    switch (eid) {
        case 0:         return Error::Success;
        case 2:         return Error::NotFound; // file not found
        case 1460:      return Error::TimedOut;
        default:        return Error::Other;
    }
}

}

#endif
