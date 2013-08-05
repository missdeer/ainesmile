#ifndef _RELAY_UTILITIY_IFCONFIG_LINUX_HPP_
#define _RELAY_UTILITIY_IFCONFIG_LINUX_HPP_

#if defined(__linux__)
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include "utility_global.hpp" 

struct route_info;
namespace relay_utility { 
    class ifconfig_linux
    {
    public:
        ifconfig_linux(void);
        ~ifconfig_linux(void);
        void get_ifconfig(std::vector<if_config>& ifs);
        bool set_ifconfig(const if_config& ifs);
    private:
        int  read_netlink_sock(int sock_fd, char *buf_ptr, unsigned int seq_num, unsigned int p_id);
        void parse_routes(struct nlmsghdr *netlink_headr, struct route_info *rt_info,char *gateway);
        int  get_gateway(char *gate_way);
        bool ifconfig_file(const std::string& devname,const std::string& conf_name,const std::string& config_buff);
    };
} // namespace relay_utility

#endif

#endif
