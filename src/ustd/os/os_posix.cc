
#include "config.inl"

namespace ustd::os
{

pub fn get_error() noexcept -> Error {
    let eid = errno;

    if (eid == 0) {
        return Error::Success;
    }

    switch (eid) {
        case EBADF      :
        case ENXIO      :
        case EOVERFLOW  :
        case EINVAL     : return Error::InvalidInput;
        case EACCES     : return Error::PermissionDenied;
        case ENOENT     : return Error::NotFound;
        case EEXIST     : return Error::AlreadyExists;
        case EINTR      : return Error::Interrupted;
        default         : return Error::Other;
    }
}

pub fn to_str(Error e) noexcept -> str {
    using namespace os;

    switch(e) {
      case os::Error::Success            : return "Success";
      case os::Error::NotFound           : return "NotFound";
      case os::Error::PermissionDenied   : return "PermissionDenied";
      case os::Error::ConnectionRefused  : return "ConnectionRefused";
      case os::Error::ConnectionReset    : return "ConnectionReset";
      case os::Error::ConnectionAborted  : return "ConnectionAborted";
      case os::Error::NotConnected       : return "NotConnected";
      case os::Error::AddrInUse          : return "AddrInUse";
      case os::Error::AddrNotAvailable   : return "AddrNotAvailable";
      case os::Error::BrokenPipe         : return "BrokenPipe";
      case os::Error::AlreadyExists      : return "AlreadyExists";
      case os::Error::WouldBlock         : return "WouldBlock";
      case os::Error::InvalidInput       : return "InvalidInput";
      case os::Error::InvalidData        : return "InvalidData";
      case os::Error::TimedOut           : return "TimedOut";
      case os::Error::WriteZero          : return "WriteZero";
      case os::Error::Interrupted        : return "Interrupted";
      case os::Error::UnexpectedEof      : return "UnexpectedEof";
      case os::Error::Other              : return "Other";
    }

    return "Unknown";
}

}
