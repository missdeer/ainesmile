#include "stdafx.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/crc.hpp>
#include "hardware.hpp"
#include "util.hpp"

namespace utility {
    uint64 utilities::get_local_peer_id(const std::string& additional_info)
    {
        std::string cpuinfo, hdinfo, macaddinfo, partitioninfo;
        uint64 pid;
#if !defined(__APPLE__)
        hardware_info::get_CPU_id(cpuinfo);
#endif
        hardware_info::get_HDD_id(hdinfo);
        hardware_info::get_Mac_address(macaddinfo);
        hardware_info::get_root_path_id(partitioninfo);
        std::string id = cpuinfo + hdinfo + macaddinfo + partitioninfo + additional_info;

        pid = utilities::hash64((uint8 *)(id.c_str()), id.length());

        return pid;
    }

#define hashsize(n) ((uint8)1<<(n))
#define hashmask(n) (hashsize(n)-1)

#define mix64(a,b,c) \
    { \
    a -= b; a -= c; a ^= (c>>43); \
    b -= c; b -= a; b ^= (a<<9); \
    c -= a; c -= b; c ^= (b>>8); \
    a -= b; a -= c; a ^= (c>>38); \
    b -= c; b -= a; b ^= (a<<23); \
    c -= a; c -= b; c ^= (b>>5); \
    a -= b; a -= c; a ^= (c>>35); \
    b -= c; b -= a; b ^= (a<<49); \
    c -= a; c -= b; c ^= (b>>11); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<18); \
    c -= a; c -= b; c ^= (b>>22); \
    }

    uint64 utilities::hash64(uint8 *k, uint32 length, uint64 level)
    {
        uint64 a,b,c;
        uint32 len;

        /* Set up the internal state */
        len = length;
        a = b = level;                         /* the previous hash value */
        c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

        /*---------------------------------------- handle most of the key */
        while (len >= 24)
        {
            a += (k[0]              +((uint64)k[ 1]<< 8)+((uint64)k[ 2]<<16)+((uint64)k[ 3]<<24)
                +((uint64)k[4 ]<<32)+((uint64)k[ 5]<<40)+((uint64)k[ 6]<<48)+((uint64)k[ 7]<<56));
            b += (k[8]              +((uint64)k[ 9]<< 8)+((uint64)k[10]<<16)+((uint64)k[11]<<24)
                +((uint64)k[12]<<32)+((uint64)k[13]<<40)+((uint64)k[14]<<48)+((uint64)k[15]<<56));
            c += (k[16]             +((uint64)k[17]<< 8)+((uint64)k[18]<<16)+((uint64)k[19]<<24)
                +((uint64)k[20]<<32)+((uint64)k[21]<<40)+((uint64)k[22]<<48)+((uint64)k[23]<<56));
            mix64(a,b,c);
            k += 24; len -= 24;
        }

        /*------------------------------------- handle the last 23 bytes */
        c += length;
        switch (len)              /* all the case statements fall through */
        {
        case 23: c+=((uint64)k[22]<<56);
        case 22: c+=((uint64)k[21]<<48);
        case 21: c+=((uint64)k[20]<<40);
        case 20: c+=((uint64)k[19]<<32);
        case 19: c+=((uint64)k[18]<<24);
        case 18: c+=((uint64)k[17]<<16);
        case 17: c+=((uint64)k[16]<<8);
            /* the first byte of c is reserved for the length */
        case 16: b+=((uint64)k[15]<<56);
        case 15: b+=((uint64)k[14]<<48);
        case 14: b+=((uint64)k[13]<<40);
        case 13: b+=((uint64)k[12]<<32);
        case 12: b+=((uint64)k[11]<<24);
        case 11: b+=((uint64)k[10]<<16);
        case 10: b+=((uint64)k[ 9]<<8);
        case  9: b+=((uint64)k[ 8]);
        case  8: a+=((uint64)k[ 7]<<56);
        case  7: a+=((uint64)k[ 6]<<48);
        case  6: a+=((uint64)k[ 5]<<40);
        case  5: a+=((uint64)k[ 4]<<32);
        case  4: a+=((uint64)k[ 3]<<24);
        case  3: a+=((uint64)k[ 2]<<16);
        case  2: a+=((uint64)k[ 1]<<8);
        case  1: a+=((uint64)k[ 0]);
            /* case 0: nothing left to add */
        }
        mix64(a,b,c);
        /*-------------------------------------------- report the result */
        return c;
    }

    void utilities::get_computer_name( char* computer_name )
    {
        char name[256] = {0};
        unsigned long length = sizeof(name)/sizeof(name[0]);
#if defined(WIN32)
        if (GetComputerNameA(name, &length))
        {
            strcpy(computer_name, name);
        }
#else 
        if (!gethostname(name, length))
        {
            strcpy(computer_name, name);
        }
#endif
    }

    std::string utilities::unique_id()
    {
        boost::uuids::random_generator gen;
        boost::uuids::uuid u = gen();   

        boost::crc_32_type result;
        result.process_bytes((void const*)&u, sizeof(u));
        std::stringstream ss;
        ss << std::setw(8) << std::setfill('0') << std::hex <<  result.checksum();
        return ss.str();
    }

    void utilities::bin_to_hex( const unsigned char* bufin, int len, std::string& strout )
    {
        strout = "";
        if (!bufin)
            return;

        for (int i = 0; i < len; i++)
        {
            char hexascii[4] = {0};
#if defined(WIN32)
            _snprintf(hexascii, sizeof(hexascii), "%02x", bufin[i]);
#else
            snprintf(hexascii, sizeof(hexascii), "%02x", bufin[i]);
#endif
            strout += hexascii;
        }
    }

} // namespace utility
