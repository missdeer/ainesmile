#ifndef _UTILITY_IFCONFIG_HPP_
#define _UTILITY_IFCONFIG_HPP_

#include <boost/shared_ptr.hpp>

#include "utility_global.hpp"

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#    include "ifconfig_win32.hpp"
#elif defined(__APPLE__)
#    include "ifconfig_osx.hpp"
#elif defined(__linux__)
#    include "ifconfig_linux.hpp"
#else
#    include "ifconfig_unsupported.hpp"
#endif

namespace utility
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    using ifconfig = ifconfig_win32;
#elif defined(__APPLE__)
    typedef ifconfig_osx ifconfig;
#elif defined(__linux__)
    typedef ifconfig_linux ifconfig;
#else
    typedef ifconfig_unsupported ifconfig;
#endif

    using ifconfig_ptr = boost::shared_ptr<ifconfig>;
} // namespace utility

#endif
