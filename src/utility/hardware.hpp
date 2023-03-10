#ifndef _PEERSERVICE_HARDWARE_HPP_
#define _PEERSERVICE_HARDWARE_HPP_

#include "utility_global.hpp"
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include "hardware_win32.hpp"
#elif defined(__APPLE__)
#include "hardware_osx.hpp"
#elif defined(__linux__)
#include "hardware_linux.hpp"
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) 
#include "hardware_bsd.hpp"
#else
#include "hardware_unsupported.hpp"
#endif

namespace utility {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    typedef hardware_win32 hardware_info;
#elif defined(__APPLE__)
    typedef hardware_osx hardware_info;
#elif defined(__linux__)
    typedef hardware_linux hardware_info;
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) 
    typedef hardware_bsd hardware_info;
#else
    typedef hardware_unsupported hardware_info;
#endif
    typedef std::shared_ptr<hardware_info> hardware_info_ptr;
} // namespace utility

#endif 



