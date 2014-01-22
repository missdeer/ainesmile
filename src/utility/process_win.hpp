#ifndef _UTILITY_PROCESS_WIN_HPP_
#define _UTILITY_PROCESS_WIN_HPP_

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))

#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

namespace utility {
    class process_win
    {
    public:
        process_win(void);
        ~process_win(void);
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
        void commandline(const std::string& val);
        void arguments(std::vector<std::string>& args);
        DWORD pid() const { return pid_; }
        void pid(DWORD val) { pid_ = val;}
        bool is_shell_execute() const { return shell_execute_; }
        void is_shell_execute(bool val) { shell_execute_ = val;}
        void register_terminated_slot(boost::function<void()> f);
        void register_terminated_id_slot(boost::function<void(std::string)> f);
    private:
        bool auto_restart_;
        bool started_;
        bool should_stop_;
        bool auto_start_;
        bool shell_execute_;
        boost::thread* t_;
        DWORD pid_;
        HANDLE process_handler_;
        std::string commandline_;
        std::string id_;
        std::string name_;
        boost::signals2::connection terminated_connection_;
        boost::signals2::connection terminated_id_connection_;
        boost::signals2::signal<void ()> terminated_;
        boost::signals2::signal<void (std::string)> terminated_id_;

        void run();
        void do_attach();
        bool wait_for_process();
    };
} // namespace utility
#endif

#endif
