#ifndef _UTILITY_HARDWARE_BSD_HPP_
#define _UTILITY_HARDWARE_BSD_HPP_

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) 

namespace utility {
    class hardware_bsd
    {
    public:
        hardware_bsd(void);
        ~hardware_bsd(void);

        static int get_HDD_id(std::string& sHDDID);
        static int get_Mac_address(std::string& sMacAddr);
        static int get_CPU_id(std::string& sCPUID);
        static int get_root_path_id(std::string& sRootID);
        static int get_CPU_core_count();
    };
} // namespace utility

#endif
#endif
