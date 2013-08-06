#include "stdafx.h"
#include "process_win.hpp"

#if defined(WIN32)
namespace utility {
    process_win::process_win(void)
        : auto_restart_(true)
        , started_(false)
        , should_stop_(false)
        , auto_start_(true)
        , shell_execute_(false)
        , t_(NULL)
        , pid_(0)
        , process_handler_(NULL)
    {
    }

    process_win::~process_win(void)
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

    void process_win::start()
    {
        should_stop_ = false;
        t_ = new boost::thread(boost::bind(&process_win::run, this));        
    }

    void process_win::stop()
    {
        should_stop_ = true;
        ::TerminateProcess(process_handler_, 0xf291);
    }

    void process_win::run()
    {
        while (!should_stop_)
        {
            PROCESS_INFORMATION pi;        
            STARTUPINFOA si = {0};
            si.cb = sizeof(si);
            if ( CreateProcessA(0, 
                (char *)commandline_.c_str(),
                0, 
                0, 
                FALSE, 
                0, 
                0,
                0,
                &si, 
                &pi)) 
            {
                started_ = true;
                process_handler_ = pi.hProcess;
                pid_ = pi.dwProcessId;
                ::WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
                if (terminated_connection_.connected())
                    terminated_();
                if (terminated_id_connection_.connected())
                    terminated_id_(id_);
                if (should_stop_)
                {
                    //LOG(WARNING) << "process " << commandline_ << " exited abnormally.";
                }
                else
                {
                    //LOG(WARNING) << "process " << commandline_ << " exited abnormally, restart it now.";
                }
            }
            started_ = false;
        }
    }

    void process_win::shell_run()
    {
        while (system(commandline().c_str()))
        {
            if (terminated_connection_.connected())
                terminated_();
            if (terminated_id_connection_.connected())
                terminated_id_(id_);
            if (auto_restart())
            {
                //LOG(WARNING) << "process " << commandline() << " exited abnormally, restart it now.";
            }
            else
            {
                //LOG(WARNING) << "process " << commandline() << " exited abnormally.";
                return;
            }
        }
        if (terminated_connection_.connected())
            terminated_();
        if (terminated_id_connection_.connected())
            terminated_id_(id_);
    }

    void process_win::shell_execute()
    {
        shell_execute_ = true;
        t_ = new boost::thread(boost::bind(&process_win::shell_run, this));  
    }

    void process_win::commandline( const std::string& val )
    {
        commandline_ = boost::trim_copy(val);
    }

    void process_win::arguments( std::vector<std::string>& args )
    {
        commandline_.clear();
        for (std::vector<std::string>::iterator it = args.begin();
            args.end() != it;
            ++it)
        {
            commandline_.append(*it);
            commandline_.append(" ");
        }
    }

    void process_win::register_terminated_slot( boost::function<void()> f )
    {
        terminated_.connect(f);
    }

    void process_win::register_terminated_id_slot( boost::function<void(std::string)> f )
    {
        terminated_id_connection_ = terminated_id_.connect(f);
    }

    void process_win::attach()
    {
        //LOG(INFO) << "attach to process " << pid_ << " with commandline " << commandline();
        t_ = new boost::thread(boost::bind(&process_win::do_attach, this));        
    }

    void process_win::do_attach()
    {
        if (!wait_for_process())
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

    bool process_win::wait_for_process()
    {
        if (pid_ != 0)
        {
            HANDLE process_handler_ = ::OpenProcess(SYNCHRONIZE, FALSE, pid_ );
            if (process_handler_)
            {
                ::WaitForSingleObject(process_handler_, INFINITE);
                DWORD exit_code = 0;
                BOOL ret = ::GetExitCodeProcess(process_handler_, &exit_code);
                ::CloseHandle(process_handler_);
                process_handler_ = NULL;
                if (terminated_connection_.connected())
                    terminated_();
                if (terminated_id_connection_.connected())
                    terminated_id_(id_);

                if (ret && exit_code == 0)
                    return true;
            }
        }

        return false;
    }
} // namespace utility
#endif
