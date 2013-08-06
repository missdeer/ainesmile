#ifndef _UTILITIY_IFCONFIG_UNSUPPORTED_HPP_
#define _UTILITIY_IFCONFIG_UNSUPPORTED_HPP_

#include "utility_global.hpp"

namespace utility {
    class ifconfig_unsupported
    {
    public:
        ifconfig_unsupported(void);
        ~ifconfig_unsupported(void);
        void get_ifconfig(std::vector<if_config>& ifs){}
        bool set_ifconfig(const if_config& ifs){ return true;}
    };
} // namespace utility

#endif
