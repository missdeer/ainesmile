#ifndef DATATYPE_H
#define DATATYPE_H
//#define __WIN32

//#pragma pack(1)		/* Set data byte align */

#ifdef _MSC_VER
#ifndef pthread_t
#define pthread_t uint32
#endif
//#ifndef sleep
//#endif
#ifndef pthread_create
#define pthread_create mypthread_create
#endif
#ifndef pthread_mutex_t
#define pthread_mutex_t uint32
#endif
#endif


#undef	BIG_ENDIAN
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN	1
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
//#include <WinSock2.h>
typedef ULONGLONG uint64;
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <syslog.h>
typedef unsigned long long uint64;
#endif
typedef char int8;
typedef short int16;
typedef int int32;
#ifdef _MSC_VER
typedef LONGLONG int64;
#else
typedef long long int64;
#endif

#ifdef _MSC_VER
#define uint unsigned int
#endif
#ifndef uint
//typedef unsigned int uint;
#endif

#if !defined(uchar)
typedef unsigned char uchar;
#endif

#define BITS8	0xff
#define BITS16	0xffff
#define BITS32	0xffffffff
#define BITS48	0xffffffffffff

#if !defined(MAX)
#define MAX(a,b)	(((a)>(b)) ? (a):(b))
#endif
#if !defined(MIN)
#define MIN(a,b)	(((a)>(b)) ? (b):(a))
#endif

#ifdef DEBUG
#define DPRINTF printf
#else
#define DPRINTF
#endif

#ifndef __WIN32
#ifdef LITTLE_ENDIAN	/* Intel architecture */
static inline uint64 hton64(uint64 lli)
{
	/*uint8 *cold, *cnew;
	uint64 llinew;
	int i;
	cold = (uint8 *)&lli;
	cnew = (uint8 *)&llinew;
	for(i=0;i<8;i++)
		cnew[i] = cold[7-i];
	return llinew;*/
    return
    (uint64)(((uint64)(lli) & (uint64)0x00000000000000ffULL) << 56) |
    (uint64)(((uint64)(lli) & (uint64)0x000000000000ff00ULL) << 40) |
    (uint64)(((uint64)(lli) & (uint64)0x0000000000ff0000ULL) << 24) |
    (uint64)(((uint64)(lli) & (uint64)0x00000000ff000000ULL) <<  8) |
    (uint64)(((uint64)(lli) & (uint64)0x000000ff00000000ULL) >>  8) |
    (uint64)(((uint64)(lli) & (uint64)0x0000ff0000000000ULL) >> 24) |
    (uint64)(((uint64)(lli) & (uint64)0x00ff000000000000ULL) >> 40) |
    (uint64)(((uint64)(lli) & (uint64)0xff00000000000000ULL) >> 56) ;
}

static inline uint64 ntoh64(uint64 lli)
{
	return hton64(lli);
}

#define ntoh64	hton64

#elif BIG_ENDIAN	/* BIG_ENDIAN */
#define hton64
#define ntoh64
#endif	/* LITTLE_ENDIAN */
#endif	/* __WIN32 */


int mypthread_create(uint32* id, void * attr, void *(*start_routine)(void *), void * arg);
#ifdef _MSC_VER
void sleep(uint32 interval);
void usleep(uint32 usec);
#define pthread_cancel TVU_KillThread
#define pthread_join(v1, v2)
#define pthread_detach(v)
#define pthread_exit(v)
int  pthread_mutex_init(pthread_mutex_t  *mutex,  void *mutexattr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but significant condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */

#define LOG_LOCAL0      (16<<3) /* reserved for local use */
#define LOG_LOCAL1      (17<<3) /* reserved for local use */
#define LOG_LOCAL2      (18<<3) /* reserved for local use */
#define LOG_LOCAL3      (19<<3) /* reserved for local use */
#define LOG_LOCAL4      (20<<3) /* reserved for local use */
#define LOG_LOCAL5      (21<<3) /* reserved for local use */
#define LOG_LOCAL6      (22<<3) /* reserved for local use */
#define LOG_LOCAL7      (23<<3) /* reserved for local use */

#define LOG_PID         0x01    /* log the pid with each message */
#define LOG_CONS        0x02    /* log on the console if errors in sending */
#define LOG_ODELAY      0x04    /* delay open until first syslog() (default) */
#define LOG_NDELAY      0x08    /* don't delay open */
#define LOG_NOWAIT      0x10    /* don't wait for console forks: DEPRECATED */
#define LOG_PERROR      0x20    /* log to stderr as well */

#endif

#define TRANSACTION_BDB

#ifdef TRANSACTION_BDB
#define TXN_BEGIN(envp, txnp)   envp->txn_begin(envp, NULL, &(txnp), 0)
//#define TXN_BEGIN(envp, txnp)   envp->txn_begin(envp, NULL, &(txnp), DB_TXN_NOWAIT)
#define TXN_COMMIT(txnp)        txnp->commit(txnp, 0)
#define TXN_ABORT(txnp)         txnp->abort(txnp)
#else
#define TXN_BEGIN(envp, txnp)   
#define TXN_COMMIT(txnp)        
#define TXN_ABORT(txnp)         
#endif

#define STREAM2VIEWABLE(streamid) (ntohl(streamid) >> 12)

#ifdef _MSC_VER
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif


#define NETWORK_TYPE_EXTERNAL     0
#define NETWORK_TYPE_INTERNAL     1

#define PROCTYPE_RD_NORMAL		0
#define PROCTYPE_BD_NORMAL		1
#define PROCTYPE_BD_HEARTBEAT	2
#define PROCTYPE_PS_NORMAL		3
#define PROCTYPE_FILEP2P        4

#define STREAM_VALID   0
#define STREAM_INVALID 1

#endif	/* DATATYPE_H */
