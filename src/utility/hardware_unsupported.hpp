#ifndef _UTILITY_HARDWARE_UNSUPPORTED_HPP_
#define _UTILITY_HARDWARE_UNSUPPORTED_HPP_

#if !defined(WIN32) && !defined(__APPLE__) && !defined(__linux__)

namespace utility {
    class hardware_unsupported
    {
    public:
        hardware_unsupported(void);
        ~hardware_unsupported(void);

        static int get_HDD_id(std::string& sHDDID){ return 0;}
        static int get_Mac_address(std::string& sMacAddr){ return 0;}
        static int get_CPU_id(std::string& sCPUID){ return 0; }
        static int get_root_path_id(std::string& sRootID){ return 0;}
        static int get_CPU_core_count(){ return 0;}
    };
} // namespace utility

#endif
#endif
