#include "stdafx.h"
#if defined(__linux__)
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ifconfig_linux.hpp"

#define MAXINTERFACES   16
#define BUFSIZE         8192
struct route_info
{   
    u_int dst_addr;   
    u_int src_addr;   
    u_int gate_way;   
    char if_name[IF_NAMESIZE];   
};  
namespace relay_utility {
    ifconfig_linux::ifconfig_linux(void)
    {
    }

    ifconfig_linux::~ifconfig_linux(void)
    {
    }

    void ifconfig_linux::get_ifconfig( std::vector<if_config>& ifs )
    {
        register int fd;
        if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
        {
            struct ifreq buf[MAXINTERFACES];
            struct ifconf ifc;
            ifc.ifc_len = sizeof buf;
            ifc.ifc_buf = (caddr_t) buf;
            if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc))
            {
                int intrface = ifc.ifc_len / sizeof (struct ifreq);
                LOG(INFO) << "interface num is interface = " << intrface;

                // read /etc/resolv.conf for DNS info
                std::string dns1, dns2;
                FILE* fp;
                if ((fp = fopen("/etc/resolv.conf", "r")) != NULL)
                {
                    while (!feof(fp))
                    {
                        char line[256] = {0};
                        fgets(line, 255, fp);
                        if (strstr(line, "nameserver") == line)
                        {
                            if (dns1.empty())
                            {
                                char* p = strchr(line, ' ');
                                while (*p == ' ')
                                {
                                    p++;
                                }
                                dns1 = p;
                                while ( dns1[dns1.size() - 1] == '\r' || dns1[dns1.size() - 1] == '\n') dns1.erase(dns1.size() -1, 1);
                            } 
                            else if (dns2.empty())
                            {
                                char* p = strchr(line, ' ');
                                while (*p == ' ')
                                {
                                    p++;
                                }
                                dns2 = p;
                                while ( dns2[dns2.size() - 1] == '\r' || dns2[dns2.size() - 1] == '\n') dns2.erase( dns2.size() -1 , 1);
                                break;
                            }
                        }
                    }

                    fclose(fp);
                }

                // enumerate all network interfaces
                while (intrface-- > 0)
                {
                    if_config if_cfg;
                    if_cfg.name_ = buf[intrface].ifr_name;

                    /*Get IP of the net card */
                    if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                    {
                        if_cfg.ip_ = inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr);
                    }
                    else
                    {
                        LOG(ERROR) << "cpm: ioctl device " << buf[intrface].ifr_name;
                        continue;
                    }
                    /* this section can't get Hardware Address,I don't know whether the reason is module driver*/
                    // ((struct sockaddr_in*)&arp.arp_pa)->;sin_addr=((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr;
                    if (!(ioctl (fd, SIOCGIFHWADDR, (char *) &buf[intrface])))
                    {
                        char mac_address[20] = {0};
                        snprintf(mac_address, sizeof(mac_address), "%02x-%02x-%02x-%02x-%02x-%02x", 
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[0],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[1],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[2],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[3],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[4],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]);
                        if_cfg.mac_address_ = mac_address;
                    }
                    else
                    {
                        LOG(ERROR) << "cpm: ioctl device " << buf[intrface].ifr_name;
                        continue;
                    }

                    if ( !(ioctl( fd, SIOCGIFNETMASK, (char *) &buf[intrface] ) ))    
                    {
                        struct sockaddr_in *net_mask = ( struct sockaddr_in * )&( buf[intrface].ifr_netmask );   
                        if_cfg.submask_ = inet_ntoa( net_mask->sin_addr );   
                    }
                    else
                    {
                        LOG(ERROR) << "cpm: ioctl device " << buf[intrface].ifr_name;
                        continue;
                    }

                    if_cfg.primary_dns_ = dns1;
                    if_cfg.second_dns_ = dns2;
                    
                    char gateway_buf[128] = {0};
                    if (!get_gateway(gateway_buf))
                    {
                        if_cfg.gateway_ = std::string(gateway_buf);
                    }
                    else
                    {
                        LOG(ERROR)<<"[ Kayvan :NETLINK]:Get gateway message error !"; 
                    }
            
                    ifs.push_back(if_cfg);
                }
            }
            else
            {
                LOG(ERROR) <<  "cpm: ioctl";
            }
        }
        else
        {
            LOG(ERROR) << "cpm: socket";
        }

        close(fd);
    }

    int ifconfig_linux::read_netlink_sock(int sock_fd, char *buf_ptr, unsigned int seq_num, unsigned int p_id)   
    {   
        struct nlmsghdr *nl_header;   
        int msg_len = 0;   
        do {
            int read_len = 0;
            if ((read_len = recv(sock_fd, buf_ptr, BUFSIZE - msg_len, 0)) < 0)   
            {   
                LOG(ERROR)<<"[ Kayvan :NETLINK]:SOCK read  error!";   
                return -1;   
            }   

            nl_header = (struct nlmsghdr *)buf_ptr;   
            if ((NLMSG_OK(nl_header, read_len)) == 0 || (nl_header->nlmsg_type == NLMSG_ERROR))   
            {   
                LOG(ERROR)<<"[ Kayvan :NETLINK]:Error in recieved packet";   
                return -1;   
            }          
            if (nl_header->nlmsg_type == NLMSG_DONE)    
            {   
                break;   
            }   
            else   
            {   
                buf_ptr += read_len;   
                msg_len += read_len;   
            }   
            if ((nl_header->nlmsg_flags & NLM_F_MULTI) == 0)    
            {   
                break;   
            }   
        } while ((nl_header->nlmsg_seq != seq_num) || (nl_header->nlmsg_pid != p_id));   
        return msg_len;   
    }  
    void ifconfig_linux::parse_routes(struct nlmsghdr *nl_header, struct route_info *rt_info,char *gateway)   
    {   
        struct rtmsg *route_msg;   
        struct rtattr *route_attr;   
        int route_len;   
        struct in_addr dst;   
        struct in_addr gate;   
  
        route_msg = (struct rtmsg *)NLMSG_DATA(nl_header);     
        if ((route_msg->rtm_family != AF_INET) || (route_msg->rtm_table != RT_TABLE_MAIN))   
            return;   

        route_attr = (struct rtattr *)RTM_RTA(route_msg);   
        route_len = RTM_PAYLOAD(nl_header);   
        for (;RTA_OK(route_attr,route_len);route_attr = RTA_NEXT(route_attr,route_len))
        {   
            switch (route_attr->rta_type) 
            {   
            case RTA_OIF:   
                if_indextoname(*(int *)RTA_DATA(route_attr), rt_info->if_name);   
                break;   
            case RTA_GATEWAY:   
                rt_info->gate_way = *(u_int *)RTA_DATA(route_attr);   
                break;   
            case RTA_PREFSRC:   
                rt_info->dst_addr = *(u_int *)RTA_DATA(route_attr);   
                break;   
            case RTA_DST:   
                rt_info->dst_addr = *(u_int *)RTA_DATA(route_attr);   
                break;   
            }   
        }   
        dst.s_addr = rt_info->dst_addr;   
        if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))   
        {   
            gate.s_addr = rt_info->gate_way;   
            snprintf(gateway, sizeof(gateway), (char *)inet_ntoa(gate));
            gate.s_addr = rt_info->dst_addr;   
            gate.s_addr = rt_info->dst_addr;   
        }    
        return;   
    }
    int ifconfig_linux::get_gateway(char *gate_way)   
    {   
        struct nlmsghdr *netlink_msge;   
        struct rtmsg *route_msg;   
        struct route_info *rt_info;   
        char msg_buf[BUFSIZE];       
        int sock, len= 0; 
        unsigned int msg_seque = 0;    
        if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)   
        {   
            LOG(ERROR)<<"[ Kayvan :NETLINK]:Socket create error !";   
            return -1;   
        }       
        memset(msg_buf, 0, BUFSIZE);       
        netlink_msge = (struct nlmsghdr *)msg_buf;   
        route_msg = (struct rtmsg *)NLMSG_DATA(netlink_msge);
        netlink_msge->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.   
        netlink_msge->nlmsg_type = RTM_GETROUTE;                      // Get the routes from kernel routing table .   
        netlink_msge->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;       // The message is a request for dump.   
        netlink_msge->nlmsg_seq = msg_seque++;                        // Sequence of the message packet.   
        netlink_msge->nlmsg_pid = getpid();                           // PID of process sending the request.   
        if (send(sock, netlink_msge, netlink_msge->nlmsg_len, 0) < 0)
        {   
            LOG(ERROR)<<"[ Kayvan :NETLINK]:Write To Socket Failed…";   
            return -1;   
        }   

        if ((len = read_netlink_sock(sock, msg_buf, msg_seque, getpid())) < 0) 
        {   
            LOG(ERROR)<<"[ Kayvan :NETLINK]:Read From Socket Failed…";   
            return -1;   
        }   
        rt_info = (struct route_info *)malloc(sizeof(struct route_info));   
        for (;NLMSG_OK(netlink_msge,len);netlink_msge = NLMSG_NEXT(netlink_msge,len))
        {   
            memset(rt_info, 0, sizeof(struct route_info));   
            parse_routes(netlink_msge, rt_info,gate_way);   
        }   
        free(rt_info);   
        close(sock);   
        return 0;   
    }
    bool ifconfig_linux::ifconfig_file( const std::string& devname,const std::string& conf_name,const std::string& config_buff)
    {
        const std::string PATH = "/etc/sysconfig/network-scripts/ifcfg-";
        std::string config_path = PATH+devname;
        char config_linebuf[256] = {0};
        unsigned int  alter_sign = 0;
        FILE * fp = fopen(config_path.c_str(),"r+");
        if (fp == NULL)
        {
            LOG(ERROR)<<"[ Kayvan :IFCONFIG]:OPEN CONFIG FILE: "<< config_path << "FAILD ! \n";
            return false;
        }
        char sum_buf[1024] = {0}; 
        while (fgets(config_linebuf,sizeof(config_linebuf)-1,fp) != NULL)
        {   
            if (strlen(config_linebuf) < 3)//If there is white line,copy and jump.
            {
                strcat(sum_buf,config_linebuf);
                continue;
            }
            char * leave_line = strstr(config_linebuf,"=");   
            if (leave_line == NULL) // If there is no "=",copy and jump.
            {
                strcat(sum_buf,config_linebuf);
                continue;
            }

            int leave_num = leave_line - config_linebuf;
            char line_name[64] = {0};
            strncpy(line_name,config_linebuf,leave_num);
            if (strcmp(line_name,conf_name.c_str()) ==0)
            {
                std::string add_line = conf_name + "=" + config_buff;
                strcat(sum_buf,add_line.c_str());
                strcat(sum_buf,"\n");
                alter_sign = 1;
            }
            else
            {
                strcat(sum_buf,config_linebuf);
            }
            if (fgetc(fp)==EOF)    break;  
            fseek(fp,-1,SEEK_CUR);
            memset(config_linebuf,0,sizeof(config_linebuf));
        }

        if (alter_sign == 0)      // If there is no configure name,add it.
        {
            std::string add_line = conf_name + "=" + config_buff;
            strcat(sum_buf,add_line.c_str());
            strcat(sum_buf,"\n");
        }
        if (fp)  fclose(fp);
        FILE *fpnw;
        fpnw = fopen(config_path.c_str(),"w");
        if (fpnw == NULL)
        {
            LOG(ERROR)<<"[ Kayvan :IFCONFIG]:OPEN CONFIG FILE: "<< config_path << "FAILD ! \n";
            return false;
        }
        fseek(fpnw,0,SEEK_SET);
        fputs(sum_buf,fpnw);
        if (fpnw)  fclose(fpnw);
        return true;
    }

    bool ifconfig_linux::set_ifconfig( const if_config& ifs )
    {
        std::string dev_name = ifs.name_;
        std::string config_gateway = "GATEWAY";
        const std::string net_working    = "NETWORKING=yes";
        const std::string network_path   = "/etc/sysconfig/network";
        const std::string config_boot    = "BOOTPROTO";
        const std::string config_IP      = "IPADDR";
        const std::string config_mask    = "NETMASK";
        const std::string config_peerdns = "PEERDNS";
        const std::string config_dns1    = "DNS1";
        const std::string config_dns2    = "DNS2";

        if (!ifs.dhcp_enabled_)
        {
            //configure  file "/etc/sysconfig/network"
            FILE * network_fp = fopen(network_path.c_str(),"w+");
            if (network_fp == NULL)
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:OPEN CONFIG FILE: "<< network_path << "  FAILD !";
                return false;
            }
            std::string host_name   = "#HOSTNAME=localhost.localdomain";
            if (ifs.gateway_.length() >= 7 && ifs.gateway_.length() <= 15)
            {
                config_gateway = "GATEWAY=" + ifs.gateway_;
            }
            else
            {
                config_gateway.clear();
            }
            std::string network_buf = net_working + "\n" + host_name + "\n" + config_gateway;
            fputs(network_buf.c_str(),network_fp);
            if (network_fp)  fclose(network_fp);

            //configure  file "/etc/sysconfig/network-scripts/ifcfg-eth(X)" 
            std::string config_boot_data = "none";
            if (!ifconfig_file(dev_name, config_boot, config_boot_data))
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig BOOTPROTO failed!"; 
                return false;
            }
            if (ifs.ip_.length() >= 7 && ifs.ip_.length() <= 15)
            {
                std::string config_IP_data = ifs.ip_;
                if (!ifconfig_file(dev_name, config_IP, config_IP_data))
                {
                    LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig IP failed !\n";
                    return false;
                }
            }
            else
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:wrong:: IP !\n";
                return false;
            }
            
            if (ifs.submask_.length() >= 7 && ifs.submask_.length() <= 15)
            {
                std::string config_mask_data = ifs.submask_;
                if (!ifconfig_file(dev_name, config_mask, config_mask_data))
                {
                    LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig NETMASK failed!\n";
                    return false;
                }
            }
            else
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:wrong:: NETMASK !\n";
                return false;
            }
            
            if (ifs.gateway_.length() >= 7 && ifs.gateway_.length() <= 15)
            {
                config_gateway = "GATEWAY";
                std::string config_gateway_data = ifs.gateway_;
                if (!ifconfig_file(dev_name, config_gateway, config_gateway_data))
                {
                    LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig GATEWAY failed!\n";
                    return false;
                }
            }
            else
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:wrong:: GATEWAY !\n";
                return false;
            }
        }
        else
        {
            //configure  file "/etc/sysconfig/network"
            FILE * network_fp = fopen(network_path.c_str(),"w+");
            if (network_fp == NULL)
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:OPEN CONFIG FILE: "<< network_path << "FAILD \n";
                return false;
            }
            std::string host_name   = "HOSTNAME=localhost.localdomain";
            std::string network_buf = net_working + "\n" + host_name + "\n";
            fputs(network_buf.c_str(),network_fp);
            if (network_fp)  fclose(network_fp);
            //configure  file "/etc/sysconfig/network-scripts/ifcfg-eth(X)" 
            std::string config_boot_data = "dhcp";
            if (!ifconfig_file(dev_name, config_boot, config_boot_data))
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig boot protocol failed!\n";
                return false;
            }
        }

        if (!ifs.auto_dns_)
        {
            std::string config_peerdns_data = "yes";
            if (!ifconfig_file(dev_name, config_peerdns, config_peerdns_data))
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig PEERDNS failed!\n";
                return false;
            }
            if (ifs.primary_dns_.length() >= 7 && ifs.primary_dns_.length()<= 15)
            {
                std::string config_dns1_data = ifs.primary_dns_;
                if (!ifconfig_file(dev_name, config_dns1, config_dns1_data))
                {
                    LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig DNS1 failed!\n";
                    return false;
                }
            }
            else
            {
                LOG(ERROR)<<"[ Kayvan :IFCONFIG]:wrong:: DNS1 !\n";
                return false;
            }
            if (ifs.second_dns_.length() >= 7 && ifs.second_dns_.length()<= 15)
            {
                std::string config_dns2_data = ifs.second_dns_;
                if (!ifconfig_file(dev_name, config_dns2, config_dns2_data))
                {
                    LOG(ERROR)<<"[ Kayvan :IFCONFIG]:ifconfig DNS2 failed!\n";
                    return false;
                }
            }
        }
        return true;
    }

} // namespace relay_utility

#endif
