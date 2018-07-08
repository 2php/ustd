
#include "config.inl"

namespace ustd::os
{

pub fn to_str(Error e) noexcept -> str {
    using namespace os;

    mut res = str("Unknown");

    switch(e) {
      case Error::Success            : res = "Success";             break;
      case Error::NotFound           : res = "NotFound";            break;
      case Error::PermissionDenied   : res = "PermissionDenied";    break;
      case Error::ConnectionRefused  : res = "ConnectionRefused";   break;
      case Error::ConnectionReset    : res = "ConnectionReset";     break;
      case Error::ConnectionAborted  : res = "ConnectionAborted";   break;
      case Error::NotConnected       : res = "NotConnected";        break;
      case Error::AddrInUse          : res = "AddrInUse";           break;
      case Error::AddrNotAvailable   : res = "AddrNotAvailable";    break;
      case Error::BrokenPipe         : res = "BrokenPipe";          break;
      case Error::AlreadyExists      : res = "AlreadyExists";       break;
      case Error::WouldBlock         : res = "WouldBlock";          break;
      case Error::InvalidInput       : res = "InvalidInput";        break;
      case Error::InvalidData        : res = "InvalidData";         break;
      case Error::TimedOut           : res = "TimedOut";            break;
      case Error::WriteZero          : res = "WriteZero";           break;
      case Error::Interrupted        : res = "Interrupted";         break;
      case Error::UnexpectedEof      : res = "UnexpectedEof";       break;
      case Error::Other              : res = "Other";               break;
    }

    return res;
}

}
