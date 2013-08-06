#ifndef _UTILITY_PROCESS_HPP_
#define _UTILITY_PROCESS_HPP_

#if defined(WIN32)
#include "process_win.hpp"
#else
#include "process_unix.hpp"
#endif

namespace utility {
#if defined(WIN32)
    typedef process_win process;
#else
    typedef process_unix process;
#endif
    typedef boost::shared_ptr<process> process_ptr;
} // namespace utility


#endif
