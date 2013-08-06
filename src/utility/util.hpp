#ifndef _HASH_64BITS_H_
#define _HASH_64BITS_H_

#include "datatype.hpp"
#include "utility_global.hpp"

#define DEFAULT_LEVEL1 0xb814f4ad1eabee1dULL

namespace utility {

    class utilities
    {
    public:
        static std::string unique_id();
        static uint64 hash64(uint8 *k, uint32 length, uint64 level = DEFAULT_LEVEL1);
        static uint64 get_local_peer_id(const std::string& additional_info = std::string());
        static void get_computer_name(char* computer_name);
        static void bin_to_hex(const unsigned char* bufin, int len, std::string& strout);

    };
} // namespace utility

#ifdef _WIN32
#define FMT64D    "%I64d"
#define FMT64U    "%I64u"
#define FMT64X    "%I64X"
#define FMT64x    "%I64x"
#else
#define FMT64D    "%lld"
#define FMT64U    "%llu"
#define FMT64X    "%llX"
#define FMT64x    "%llx"
#endif


#endif
