#ifndef _RELAY_UTILITY_SOCKET_UTIL_HPP_
#define _RELAY_UTILITY_SOCKET_UTIL_HPP_

#include "sysdef.h"

namespace relay_utility {
    class socket_util
    {
    public:
        static int readable(socket_t sockfd, int sec, int usec);
        static int sendable(socket_t sockfd, int sec, int usec);
        static void set_non_block(socket_t fd, bool non_block);
    };
} // namespace relay_utility

#endif
