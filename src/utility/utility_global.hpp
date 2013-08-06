#ifndef _UTILITY_GLOBAL_HPP_
#define _UTILITY_GLOBAL_HPP_

namespace utility {
    struct if_config
    {
        bool dhcp_enabled_;
        bool auto_dns_;
        std::string name_;
        std::string description_;
        std::string mac_address_;
        std::string ip_;
        std::string submask_;
        std::string gateway_;
        std::string primary_dns_;
        std::string second_dns_;
    };

    enum license_type
    {
        LT_TRIAL = 0,
        LT_FULL,
    };
} // namespace utility

#endif // _UTILITY_GLOBAL_HPP_
