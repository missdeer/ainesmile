#ifndef _UTILITIY_IFCONFIG_OSX_HPP_
#define _UTILITIY_IFCONFIG_OSX_HPP_

#if defined(__APPLE__)
#include "utility_global.hpp"

namespace utility {
    class ifconfig_osx
    {
    public:
        ifconfig_osx(void);
        ~ifconfig_osx(void);
        void get_ifconfig(std::vector<if_config>& ifs);
        bool set_ifconfig(const if_config& ifs);
    };

} // namespace utility
#endif

#endif
