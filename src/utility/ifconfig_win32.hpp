#ifndef _RELAY_UTILITIY_IFCONFIG_WIN32_HPP_
#define _RELAY_UTILITIY_IFCONFIG_WIN32_HPP_

#if defined(WIN32)
#include "utility_global.hpp"

namespace relay_utility {
    class ifconfig_win32
    {
    public:
        ifconfig_win32(void);
        ~ifconfig_win32(void);
        void get_ifconfig(std::vector<if_config>& ifs);
        bool set_ifconfig(const if_config& ifs);
    };
} // namespace relay_utility
#endif

#endif
