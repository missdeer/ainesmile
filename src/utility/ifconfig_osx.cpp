#include "stdafx.h"
#if defined(__APPLE__)
#include "ifconfig_osx.hpp"

namespace utility {
    ifconfig_osx::ifconfig_osx(void)
    {
    }

    ifconfig_osx::~ifconfig_osx(void)
    {
    }

    void ifconfig_osx::get_ifconfig( std::vector<if_config>& ifs )
    {

    }

    bool ifconfig_osx::set_ifconfig( const if_config& ifs )
    {
        return true;
    }

} // namespace utility

#endif
