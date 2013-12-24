#include "stdafx.h"

#if defined(__FREEBSD__) || defined(__OPENBSD__) || defined(__NETBSD__) 

namespace utility {
#include "hardware_bsd.hpp"

    hardware_bsd::hardware_bsd(void)
    {
    }

    hardware_bsd::~hardware_bsd(void)
    {
    }

    int hardware_linux::get_HDD_id( std::string& sHDDID )
    {
        int fd;
        return 0;
    }

    int hardware_linux::get_Mac_address( std::string& sMacAddr )
    {
        int ret = 0;
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
        char serialNumber[NAME_MAX] = "f98c4a27-215e-44bd-ba86-4a8df2869714";
        std::stringstream ss;
        ss << serialNumber;
        ss >> sRootID;

        return 0;
    }

    int hardware_linux::get_CPU_core_count()
    {
        return 0;
    }
} // namespace utility

#endif
