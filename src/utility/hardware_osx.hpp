#ifndef _UTILITY_HARDWARE_OSX_HPP_
#define _UTILITY_HARDWARE_OSX_HPP_

#if defined(__APPLE__)
namespace relay_utility {
    class hardware_osx
    {
    public:
        hardware_osx(void);
        ~hardware_osx(void);
        static int get_HDD_id(std::string& sHDDID);
        static int get_Mac_address(std::string& sMacAddr);
        static int get_CPU_id(std::string& sCPUID);    
        static int get_root_path_id(std::string& sRootID);
        static int get_CPU_core_count();
    };
} // namespace relay_utility

#endif

#endif
