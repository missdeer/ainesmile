#include "stdafx.h"
#include "socket_util.hpp"

namespace utility {
    int socket_util::readable( socket_t sockfd, int sec, int usec )
    {

        struct timeval tv;
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        return select(sockfd+1, &rfds, NULL, NULL, &tv);
    }

    int socket_util::sendable( socket_t sockfd, int sec, int usec )
    {

        struct timeval tv;
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(sockfd, &wfds);
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        return select(sockfd+1, NULL, &wfds, NULL, &tv);
    }

    void socket_util::set_non_block( socket_t fd, bool non_block )
    {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
        unsigned long NonBlock =(non_block ? 1 : 0);
        ioctlsocket(fd, FIONBIO, &NonBlock);
#else
        int fl = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, (non_block ? (fl | O_NONBLOCK ) : ( fl & ~O_NONBLOCK)));
#endif
    }
} // namespace utility
