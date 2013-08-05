#ifndef _RELAY_LAUNCH_PROCESS_HPP_
#define _RELAY_LAUNCH_PROCESS_HPP_

#if defined(WIN32)
#include "process_win.hpp"
#else
#include "process_unix.hpp"
#endif

namespace relay_utility {
#if defined(WIN32)
    typedef process_win process;
#else
    typedef process_unix process;
#endif
    typedef boost::shared_ptr<process> process_ptr;
} // namespace relay_utility


#endif
