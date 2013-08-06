#ifndef _UTILITY_HARDWARE_LINUX_HPP_
#define _UTILITY_HARDWARE_LINUX_HPP_

#if defined(__linux__)
namespace utility {
    class hardware_linux
    {
    public:
        hardware_linux(void);
        ~hardware_linux(void);

        static int get_HDD_id(std::string& sHDDID);
        static int get_Mac_address(std::string& sMacAddr);
        static int get_CPU_id(std::string& sCPUID);
        static int get_root_path_id(std::string& sRootID);
        static int get_CPU_core_count();
    };
} // namespace utility

#endif


#endif
