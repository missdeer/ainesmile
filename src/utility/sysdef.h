#ifndef _SYSDEF_H_
#define _SYSDEF_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <sys/types.h>
    #include <sys/timeb.h>
    #include <malloc.h>
    #include <process.h>
#else
    #include <pthread.h>
    #include <signal.h>
    #include <semaphore.h>
    #include <sys/time.h>
    #include <sys/times.h>
    #include <sys/types.h>
    #include <sys/timeb.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <fcntl.h>
//    #include <alloca.h>
    #include <string.h>
#endif
#include <map>
#include <list>
#include <string>
#include <set>
#ifdef _MSC_VER
    #include <hash_set>
    #include <hash_map>
#else
    #include <ext/hash_set>
    #include <ext/hash_map>
#endif
using namespace std;
#ifdef _MSC_VER
    using namespace stdext;
#else
    using namespace __gnu_cxx;
#endif

#ifndef int8
    typedef char int8;
#endif
#ifndef uint8
    typedef unsigned char uint8;
#endif
#ifndef int16
    typedef short int16;
#endif
#ifndef uint16
    typedef unsigned short uint16;
#endif
#ifndef uint
    typedef unsigned int uint;
#endif
#ifndef int32
    typedef int int32;
#endif
#ifndef uint32
    typedef unsigned int uint32;
#endif
#ifndef int64
    typedef long long int64;
#endif
#ifndef uint64
    typedef unsigned long long uint64;
#endif

#ifdef _MSC_VER
#define socket_t SOCKET
#define socketlen_t int
#define vsnprintf _vsnprintf
#define strncasecmp strnicmp
#define strcasecmp stricmp
#ifndef atoll
    #define atoll _atoi64
#endif
#ifndef strtoull
    #define strtoull _strtoui64
#endif
#else
#define socket_t int
#define socketlen_t socklen_t
#define closesocket(x) ::close(x)
#define _vsnprintf vsnprintf
#define strnicmp strncasecmp
#define stricmp strcasecmp
#define INVALID_SOCKET (~0)
#endif

//#pragma pack(push, 1)        /* Set data byte align */
////#ifdef LITTLE_ENDIAN    /* Intel architecture */
//static inline uint64 hton64(uint64 lli)
//{
//    uint8 *cold, *cnew;
//    uint64 llinew;
//    int i;
//    cold = (uint8 *)&lli;
//    cnew = (uint8 *)&llinew;
//    for (i=0;i<8;i++)
//        cnew[i] = cold[7-i];
//    return llinew;
//}
////#define ntoh64    hton64
////#elif BIG_ENDIAN    /* BIG_ENDIAN */
////#define hton64
////#define ntoh64
////#endif    /* LITTLE_ENDIAN */
//
//#pragma pack(pop)        /* Restore data byte align */

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

#define RETRET            {ret = __LINE__ * -1;/*dbgtrace("ret %d %s:%s:%u\n", ret, __FILE__, __FUNCTION__, __LINE__);*/}
#define RETRETBREAK        {ret = __LINE__ * -1;/*dbgtrace("ret %d %s:%s:%u\n", ret, __FILE__, __FUNCTION__, __LINE__);*/break;}
#define RETRETRETURN    {ret = __LINE__ * -1;/*dbgtrace("ret %d %s:%s:%u\n", ret, __FILE__, __FUNCTION__, __LINE__);*/return ret;}
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if (x) delete x; x = NULL;}
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(x) {if (x) free(x); x = NULL;}
#endif



#endif
