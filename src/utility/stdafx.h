#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef ERROR
#include <winsock2.h>
#include <windows.h>
#include <IPTypes.h>
#include <IPHlpApi.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <WinIoCtl.h>
#include <shlobj.h>
#pragma warning(disable: 4996)
#elif defined(__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <mach-o/dyld.h>
#include <sys/types.h>
#include <pwd.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <ctype.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

#endif
