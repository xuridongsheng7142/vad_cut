/*
 * Copyright (c) 2017 Sinwt. All Rights Reserved.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#if     defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#define OS_WINSYS       1
#elif   defined(__linux__) || defined(__linuxppc__) || defined(__FreeBSD__)
#define OS_LINUX        2
#elif defined(__MacOSX__) || defined(__APPLE__) || defined(__MACH__)
#define OS_APPLE        3
#endif

// Windows system's version
#define OS_NONE     0   // ����
#define OS_WIN2000  1   // Windows 2000
#define OS_WINXP    2   // Windows XP
#define OS_WINVISTA 3   // Windows Vista
#define OS_WIN7     4   // Win7
#define OS_WIN8     5   // Win8
#define OS_WIN8_1   6   // Win8.1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <string>
#include <vector>
#include <map>
#include <list>
#include <deque>
#include <algorithm>
#include <functional>
#else
#include <string.h>
#include <ctype.h>
#endif
#ifdef  OS_WINSYS
// for CoInitializeEx
#define _WIN32_DCOM
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#ifndef __MINGW32__
#ifdef  __cplusplus
#include <atlbase.h>
#endif
#endif
#include <time.h>
#include <io.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif   OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <sys/uio.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <dlfcn.h>
#elif   OS_APPLE

#else
#error "No platform defined for OS (Platform.h)"
#endif

/*****************************  type defines for all platforms *****************************/

#if defined(_M_X64) || defined(__x86_64__)
#define OS_ARCH_X86_FAMILY
#define OS_ARCH_X86_64
#define OS_ARCH_64_BITS
#define OS_ARCH_LITTLEENDIAN
#elif defined(_M_IX86) || defined(__i386__)
#define OS_ARCH_X86_FAMILY
#define OS_ARCH_X86
#define OS_ARCH_32_BITS
#define OS_ARCH_LITTLEENDIAN
#elif defined(__ARMEL__)
#define OS_ARCH_32_BITS
#define OS_ARCH_LITTLEENDIAN
#else
#error "Please add support for your architecture in Platform.h"
#endif

#ifndef OS_WINSYS
#include <stdint.h>
#else
typedef signed char         int8_t;
//typedef char                int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef __int64             int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;
#endif

typedef float               OsFlt32;
typedef double              OsFlt64;
typedef long                OsLong;
typedef unsigned long       OsULong;
typedef int64_t             OsInt64;
typedef uint64_t            OsUInt64;
typedef int32_t             OsInt32;
typedef uint32_t            OsUInt32;
typedef int16_t             OsInt16;
typedef uint16_t            OsUInt16;
typedef char                OsInt8;
typedef uint8_t             OsUInt8;

#ifdef  OS_ARCH_LITTLEENDIAN
#define MakeFourCc(inChar0,inChar1,inChar2,inChar3)     \
                ((OsUInt32)(OsUInt8)(inChar3)        |  \
                ((OsUInt32)(OsUInt8)(inChar2) << 8)  |  \
                ((OsUInt32)(OsUInt8)(inChar1) << 16) |  \
                ((OsUInt32)(OsUInt8)(inChar0) << 24))
#else
#define MakeFourCc(inChar0,inChar1,inChar2,inChar3)     \
                ((OsUInt32)(OsUInt8)(inChar0)        |  \
                ((OsUInt32)(OsUInt8)(inChar1) << 8)  |  \
                ((OsUInt32)(OsUInt8)(inChar2) << 16) |  \
                ((OsUInt32)(OsUInt8)(inChar3) << 24))
#endif

typedef enum {
    OS_FALSE = 0,
    OS_TRUE
} OsBool;

// fprintf(1,'\t%.14f\n',w) for matlab
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

/***************************  macro defines for compiler preprocess ***************************/

#if defined(_MSC_VER)
// msvc 6.0   1200
// msvs 2005  1400
// msvs 2008  1500
// msvs 2010  1600
#if _MSC_VER <= 1500
// POSIX socket errorcodes. defines for compatible POSIX
#define ENOTCONN        1002
#define EADDRINUSE      1004
#define EINPROGRESS     1007
#define ENOBUFS         1008
#define EADDRNOTAVAIL   1009
#endif
// for same function
#if _MSC_VER >= 1200
#define ftruncate   _chsize
#define strncasecmp _strnicmp
#define snprintf    _snprintf
#define vsnprintf   _vsnprintf
#pragma warning(disable:4996)   // not print message of unsafe warning
#pragma warning(disable:4244)   // not print message of possible loss of data
#endif
// strtok_r��linuxƽ̨�µ�strtok�������̰߳�ȫ�棬Windows��Ϊstrtok_s
// strtok������ͨ������ѭ���İ취�������ȡ������Ϣ������
#if _MSC_VER > 1200
#define strtok_r    strtok_s
#endif
// for export function
#define OS_INLINE   __inline
#define OS_NOINLINE __declspec(noinline)
// #define OS_FORCEINLINE  __forceinline
#define likely(x)   (x)
#define unlikely(x) (x)

#elif defined(__GNUC__)

#if __GNUC__ >= 4
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif
#define OS_INLINE   __attribute__((__always_inline__)) inline
#define OS_NOINLINE __attribute__((noinline))

#endif

#ifdef  OS_WINSYS
#define OS_EXTERN   __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define OS_EXTERN   __attribute__((visibility("default")))
// ���������ѡ�-fvisibility=hidden
#else
#define OS_EXTERN   // nothing
#endif
#endif

#ifdef  OS_DEBUG
#define OsLog(inFile,inFormat,...) fprintf(inFile,inFormat,__VA_ARGS__)
// fprintf(inFile,inFormat##" [%d:%s]\n",__VA_ARGS__,__LINE__,__FUNCTION__)
// fprintf(inFile,inFormat##" [%d:%s] -> %s\n",__VA_ARGS__,__LINE__,__FUNCTION__,__FILE__)
#else
#define OsLog(inFile,inFormat,...)
#endif

#endif // PLATFORM_H