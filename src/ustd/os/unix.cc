#include "config.inl"

#ifdef USTD_OS_UNIX

namespace ustd::os::unix
{

pub fn get_error() noexcept -> Error {
    let eid = errno;
    mut res = Error::Success;

    if (eid != 0) {
        switch (eid) {
            case EBADF:
            case ENXIO:
            case EOVERFLOW:
            case EINVAL:    res = Error::InvalidInput;      break;
            case EACCES:    res = Error::PermissionDenied;  break;
            case ENOENT:    res = Error::NotFound;          break;
            case EEXIST:    res = Error::AlreadyExists;     break;
            case EINTR:     res = Error::Interrupted;       break;
            case ETIMEDOUT: res = Error::TimedOut;          break;
            default:        res = Error::Other;             break;
        }
    }

    return res;
}

}

#endif
