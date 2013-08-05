#ifndef _RELAY_LAUNCH_PROCESS_UNIX_HPP_
#define _RELAY_LAUNCH_PROCESS_UNIX_HPP_

#if !defined(WIN32)

#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

namespace relay_utility {
    class process_unix
    {
    public:
        process_unix(void);
        ~process_unix(void);
        void shell_execute();
        void shell_run();
        void start();
        void stop();
        void attach();
        bool started() const { return started_; }
        bool auto_restart() const { return auto_restart_; }
        void auto_restart(bool val) { auto_restart_ = val; }
        std::string& id()  { return id_; }
        void id(const std::string& val) { id_ = val; }
        std::string& name()  { return name_; }
        void name(const std::string& val) { name_ = val; }
        std::string& commandline()  { return commandline_; }
        void arguments(std::vector<std::string>& args);
        void commandline(const std::string& val);
        pid_t pid() const { return pid_; }
        void pid(pid_t id) { pid_ = id; }
        bool is_shell_execute() const { return shell_execute_; }
        void is_shell_execute(bool val) { shell_execute_ = val; }
        void register_terminated_slot(boost::function<void()> f);
        void register_terminated_id_slot(boost::function<void(std::string)> f);
    private:
        bool auto_restart_;
        bool auto_start_;
        bool started_;
        bool shell_execute_;
        boost::thread* t_;
        pid_t pid_;
        std::string commandline_;
        std::string executable_;
        std::string id_;
        std::string name_;
        std::vector<std::string> arguments_;
        boost::signals2::connection terminated_connection_;
        boost::signals2::connection terminated_id_connection_;
        boost::signals2::signal<void ()> terminated_;
        boost::signals2::signal<void (std::string)> terminated_id_;

        void run();
        void do_attach();
        bool wait_for_child();
        bool wait_for_non_child();
    };
} // namespace relay_utility
#endif

#endif
