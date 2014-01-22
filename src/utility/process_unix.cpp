#include "stdafx.h"
#include "process_unix.hpp"

#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <boost/lexical_cast.hpp>

namespace utility {
    process_unix::process_unix(void)
        : auto_restart_(true)
        , auto_start_(true)
        , started_(false)
        , shell_execute_(false)
        , t_(NULL)
        , pid_(0)
    {
    }

    process_unix::~process_unix(void)
    {
        if (t_)
        {
            t_->join();
            delete t_;
            t_ = NULL;
        }
        if (terminated_connection_.connected())
            terminated_connection_.disconnect();
        if (terminated_id_connection_.connected())
            terminated_id_connection_.disconnect();
    }

    void process_unix::start()
    {
        //LOG(INFO) << "start new process " << commandline_;
        t_ = new boost::thread(boost::bind(&process_unix::run, this));
    }

    void process_unix::stop()
    {
        started_ = false;
        //LOG(INFO) << "kill " << pid_;
        ::kill(pid_, SIGTERM);
    }

    void process_unix::run()
    {
        while (1) // loop infinitely
        {
            pid_ = fork();
            if (pid_ == -1)
            {
                //LOG(ERROR) << "fork error: " << errno;
                //exit(EXIT_FAILURE);
                break;
            }

            if (pid_ == 0)
            {  
                // Code executed by child 
                char **argvs = new char *[arguments_.size() + 1];
                for (size_t i = 0; i < arguments_.size(); i++)
                {
                    argvs[i] = (char *)arguments_[i].c_str();
                }
                argvs[arguments_.size()] = NULL;

                execvp( executable_.c_str(), argvs);
            }
            else
            {   
                // Code executed by parent 
                if (wait_for_child())
                    break; // jump out the loop, DO NOT restart
            }
        }
    }

    void process_unix::attach()
    {
        //LOG(INFO) << "attach to process " << pid_ << " with commandline " << commandline();
        t_ = new boost::thread(boost::bind(&process_unix::do_attach, this));
    }

    void process_unix::do_attach()
    {
        if (!wait_for_non_child())
        {
            if (is_shell_execute())
            {
                shell_run();
            }
            else
            {
                run();
            }
        }
    }
#if defined(__linux__)
    bool process_unix::wait_for_non_child()
    {
        // check the process status every 500 ms
        while (pid_ != 0)
        {
            std::string dir_path = "/proc/" + boost::lexical_cast<std::string>(pid_);
            // check the directory exist
            DIR* dir = opendir(dir_path.c_str());
            if (!dir)
            {
                // process doesn't exist
                //LOG(INFO) << "the attached process " << commandline() << " exited.";
                if (terminated_connection_.connected())
                    terminated_();
                if (terminated_id_connection_.connected())
                    terminated_id_(id_);
                return false;
            }
            closedir(dir);
#if BOOST_VERSION >= 105000
            boost::this_thread::sleep_for (boost::chrono::milliseconds(500));
#else
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
#endif
        }
        return false; // always return false to restart, since I can't get the exit code of the attached (monitored) process
    }
#else
#endif

    bool process_unix::wait_for_child()
    {
        int status;
        started_ = true;
        pid_t w = waitpid(pid_, &status, WUNTRACED | WCONTINUED);
        if (terminated_connection_.connected())
            terminated_();
        if (terminated_id_connection_.connected())
            terminated_id_(id_);
        if (w == -1)
        {
            //LOG(ERROR) << "waitpid error";
            return false;
        }

        if (status && auto_restart_)
        {
            //LOG(WARNING) << "process " << pid_ << " with command line " << commandline() << " exited abnormally, restart it now.";
            return false;
        }
        else
        {
            //LOG(INFO) << "process " << pid_ << " with command line " << commandline()  << " exit gracefully";
            return true; // jump out the loop, DO NOT restart
        } 
    }

    void process_unix::shell_run()
    {
        while (system(commandline().c_str()))
        {
            if (terminated_connection_.connected())
                terminated_();
            if (terminated_id_connection_.connected())
                terminated_id_(id_);
            if (auto_restart())
            {
                //LOG(WARNING) << "process " << commandline().c_str() << " exited abnormally, restart it now.";
            }
            else
            {
                //LOG(WARNING) << "process " << commandline().c_str() << " exited abnormally.";
                return;
            }
        }
        if (terminated_connection_.connected())
            terminated_();
        if (terminated_id_connection_.connected())
            terminated_id_(id_);
    }


    void process_unix::shell_execute()
    {
        shell_execute_ = true;
        t_ = new boost::thread(boost::bind(&process_unix::shell_run, this));  
    }

    void process_unix::arguments( std::vector<std::string>& args )
    {
        arguments_ = args;
        executable_ = args[0];
        commandline_.clear();
        for (std::vector<std::string>::iterator it = arguments_.begin();
            arguments_.end() != it;
            ++it)
        {
            commandline_.append(*it);
            commandline_.append(" ");
        }
    }

    void process_unix::commandline( const std::string& val )
    {
        commandline_ = boost::trim_copy(val);

        std::string commandline = commandline_;
        // split the executable and arguments
        BOOST_ASSERT(!commandline.empty());
        if (commandline[0] == '\"')
        {
            std::string::size_type pos = commandline.find('\"', 1);
            executable_ = commandline.substr(1, pos - 1);
            commandline.erase(0, pos + 1 + 1);
        }
        else
        {
            std::string::size_type pos = commandline.find(' ');
            executable_ = commandline.substr(0, pos);
            commandline.erase(0, pos + 1);
        }

        arguments_.clear();
        boost::split( arguments_, commandline, boost::is_any_of(" "), boost::token_compress_on );
        if (arguments_[0] != executable_)
        {
            arguments_.insert(arguments_.begin(), executable_);
        }
        // replace %20 with white space
        for (std::vector<std::string>::iterator it = arguments_.begin();
                arguments_.end() != it;
                ++it)
        {
            boost::replace_all(*it, "%20", " ");
        }
    }

    void process_unix::register_terminated_slot( boost::function<void()> f )
    {
        terminated_connection_ = terminated_.connect(f);
    }

    void process_unix::register_terminated_id_slot( boost::function<void(std::string)> f )
    {
        terminated_id_connection_ = terminated_id_.connect(f);
    }
} // namespace utility
#endif
