#include "stdafx.h"
#if !defined(WIN32) && !defined(__APPLE__) && !defined(__linux__)

namespace utility {
#include "hardware_unsupported.hpp"

    hardware_unsupported::hardware_unsupported(void)
    {
    }

    hardware_unsupported::~hardware_unsupported(void)
    {
    }
} // namespace utility

#endif
