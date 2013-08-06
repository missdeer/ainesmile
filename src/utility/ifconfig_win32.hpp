#ifndef _UTILITIY_IFCONFIG_WIN32_HPP_
#define _UTILITIY_IFCONFIG_WIN32_HPP_

#if defined(WIN32)
#include "utility_global.hpp"

namespace utility {
    class ifconfig_win32
    {
    public:
        ifconfig_win32(void);
        ~ifconfig_win32(void);
        void get_ifconfig(std::vector<if_config>& ifs);
        bool set_ifconfig(const if_config& ifs);
    };
} // namespace utility
#endif

#endif
