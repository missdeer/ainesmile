#include "stdafx.h"

#if defined(__linux__)
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>                                               
#include <linux/hdreg.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "util.hpp"
#include "hardware_linux.hpp"

namespace utility {
    int    __getMacAddressUnix(unsigned char pAddr[6])
    {
        struct ifconf ifc;
        struct ifreq ifr, *IFR;
        char buf[1024] = {0};
        int s, i, ok = 0;
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s == -1)      
            return -1;
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        ioctl(s, SIOCGIFCONF, &ifc); 
        IFR = ifc.ifc_req;
        for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) {
            strcpy(ifr.ifr_name, IFR->ifr_name);
            if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) {
                if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
                    if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
                        ok = 1;
                        break;
                    }
                }
            }
        }
        close(s);
        if (ok)
            bcopy(ifr.ifr_hwaddr.sa_data, pAddr, 6);
        else
            return -1;
        return 0;

    }

    hardware_linux::hardware_linux(void)
    {
    }

    hardware_linux::~hardware_linux(void)
    {
    }

    int hardware_linux::get_HDD_id( std::string& sHDDID )
    {
        int fd;
        struct hd_driveid hdinfo;

#define MTAB_MAX_SIZE 2048
        /*char result[MTAB_MAX_SIZE] = {0};
        char devname[NAME_MAX] = {0};
        FILE* fp = fopen("/etc/mtab", "r");
        if (!fp)
        {
        return -1;
        }
        int ret = fread(result ,1, MTAB_MAX_SIZE, fp);
        fclose(fp);
        if (ret = 0)
        {
        return -1;
        }
        sscanf(result, "%s / ", devname);
        if (!strlen(devname))
        {
        return -1;
        }
        devname[strlen(devname) - 1] = 0;*/

        char devname[NAME_MAX] = "/dev/sda";
        fd = open(devname,O_NONBLOCK); 
        if (fd != -1)
        {
            int ret = ioctl(fd , HDIO_GET_IDENTITY , &hdinfo);
            if (ret != -1)
            {    
                sHDDID = (char*)hdinfo.model;
                sHDDID.append((char*)hdinfo.serial_no);
            }

        }
        return 0;
    }

    int hardware_linux::get_Mac_address( std::string& sMacAddr )
    {
#define MAC_ADDRESS_LENGTH  6
        int ret = 0;
        unsigned char tmpMacAddressBin[MAC_ADDRESS_LENGTH] = {0};
        ret = __getMacAddressUnix(tmpMacAddressBin);
        if (ret != -1)
        {
            utility::utilities::bin_to_hex(tmpMacAddressBin, MAC_ADDRESS_LENGTH, sMacAddr);
        }
        return ret;
    }

    int hardware_linux::get_CPU_id( std::string& sCPUID )
    {

        char szTmp[40] = {0};
        char szTmp2[40] = {0};
        unsigned long s1 = 0,s2=0;

        __asm__
            __volatile__
            (
            "movl $1, %%eax\n"
            "xor %%edx,%%edx\n"
            "cpuid\n"
            "movl %%edx, %0\n"
            "movl %%eax, %1\n":"=m"(s1),"=m"(s2)
            );
        snprintf(szTmp, sizeof(szTmp), "%08X%08X", s1, s2);       
        sCPUID = szTmp;   
        __asm__
            __volatile__
            (
            "movl $3, %%eax\n"
            "xor %%ecx, %%ecx\n"
            "xor %%edx, %%edx\n"
            "cpuid\n"
            "movl %%edx, %0\n"
            "movl %%ecx, %1\n":"=m"(s1),"=m"(s2)
            );

        snprintf(szTmp2, sizeof(szTmp2), "%08X%08X", s1, s2);       
        sCPUID += szTmp2;

        return 0;
    }

    int hardware_linux::get_root_path_id( std::string& sRootID )
    {
#define MTAB_MAX_SIZE 2048
        /*char result[MTAB_MAX_SIZE] = {0};
        char devname[NAME_MAX] = {0};
        FILE* fp = fopen("/etc/mtab", "r");
        if (!fp)
        {
        return -1;
        }
        int ret = fread(result ,1, MTAB_MAX_SIZE, fp);
        fclose(fp);
        if (ret = 0)
        {
        return -1;
        }
        sscanf(result, "%s / ", devname);
        if (!strlen(devname))
        {
        return -1;
        }
        //char cmd[NAME_MAX] = "/lib/udev/vol_id -u ";
        char cmd[NAME_MAX] = "/sbin/blkid -s UUID -o value ";
        strcat(cmd, devname);
        char serialNumber[NAME_MAX] = {0};
        FILE* stream = NULL;
        stream = popen(cmd, "r");
        if (!stream)
        {
        return -1;
        }
        fread(serialNumber, 1,NAME_MAX, stream);
        pclose(stream);*/

        char serialNumber[NAME_MAX] = "f98c4a27-215e-44bd-ba86-4a8df2869714";
        std::stringstream ss;
        ss << serialNumber;
        ss >> sRootID;

        return 0;
    }

    int hardware_linux::get_CPU_core_count()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
} // namespace utility

#endif
