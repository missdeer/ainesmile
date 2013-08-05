#ifndef _RELAY_UTILITY_GLOBAL_HPP_
#define _RELAY_UTILITY_GLOBAL_HPP_

namespace relay_utility {
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
} // namespace relay_utility

#endif // _RELAY_UTILITY_GLOBAL_HPP_
