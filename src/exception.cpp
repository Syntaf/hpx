//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <hpx/exception.hpp>
#include <hpx/state.hpp>
#include <hpx/version.hpp>
#include <hpx/runtime.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/threads/threadmanager.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/util/backtrace.hpp>
#include <hpx/util/command_line_handling.hpp>

#if defined(BOOST_WINDOWS)
#  include <process.h>
#elif defined(BOOST_HAS_UNISTD_H)
#  include <unistd.h>
#endif

#include <boost/format.hpp>
#include <boost/atomic.hpp>

#include <stdexcept>
#include <algorithm>
#if defined(_POSIX_VERSION)
#include <iostream>
#endif
#include <sstream>

#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#elif !defined(BOOST_WINDOWS)
extern char **environ;
#endif

namespace hpx
{
    char const* get_runtime_state_name(state st);

    ///////////////////////////////////////////////////////////////////////////
    // For testing purposes we sometime expect to see exceptions, allow those
    // to go through without attaching a debugger.
    boost::atomic<bool> expect_exception_flag(false);

    bool expect_exception(bool flag)
    {
        return expect_exception_flag.exchange(flag);
    }
}

namespace hpx { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    std::string backtrace(std::size_t frames)
    {
        return util::trace_on_new_stack(frames);
    }

    std::string backtrace_direct(std::size_t frames)
    {
        return util::trace(frames);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  Figure out the size of the given environment
    inline std::size_t get_arraylen(char** array)
    {
        std::size_t count = 0;
        if (NULL != array) {
            while(NULL != array[count])
                ++count;   // simply count the environment strings
        }
        return count;
    }

    std::string get_execution_environment()
    {
        std::vector<std::string> env;

#if defined(BOOST_WINDOWS)
        std::size_t len = get_arraylen(_environ);
        env.reserve(len);
        std::copy(&_environ[0], &_environ[len], std::back_inserter(env));
#elif defined(linux) || defined(__linux) || defined(__linux__) \
                     || defined(__FreeBSD__) || defined(__AIX__)
        std::size_t len = get_arraylen(environ);
        env.reserve(len);
        std::copy(&environ[0], &environ[len], std::back_inserter(env));
#elif defined(__APPLE__)
        std::size_t len = get_arraylen(environ);
        env.reserve(len);
        std::copy(&environ[0], &environ[len], std::back_inserter(env));
#else
#error "Don't know, how to access the execution environment on this platform"
#endif

        std::sort(env.begin(), env.end());

        std::string retval = boost::str(boost::format("%d entries:\n") % env.size());
        for (std::string const& s : env)
        {
            retval += "  " + s + "\n";
        }
        return retval;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Exception>
    inline boost::shared_ptr<boost::exception>
    make_exception_ptr(Exception const& e)
    {
        return boost::static_pointer_cast<boost::exception>(
            boost::make_shared<Exception>(e));
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Exception>
    HPX_EXPORT boost::exception_ptr construct_exception(
        Exception const& e, std::string const& func,
        std::string const& file, long line, std::string const& back_trace,
        boost::uint32_t node, std::string const& hostname, boost::int64_t pid,
        std::size_t shepherd, std::size_t thread_id,
        std::string const& thread_name, std::string const& env,
        std::string const& config, std::string const& state_name,
        std::string const& auxinfo)
    {
        // create a boost::exception_ptr object encapsulating the Exception to
        // be thrown and annotate it with all the local information we have
        try {
            throw boost::enable_current_exception(
                boost::enable_error_info(e)
                    << hpx::detail::throw_stacktrace(back_trace)
                    << hpx::detail::throw_locality(node)
                    << hpx::detail::throw_hostname(hostname)
                    << hpx::detail::throw_pid(pid)
                    << hpx::detail::throw_shepherd(shepherd)
                    << hpx::detail::throw_thread_id(thread_id)
                    << hpx::detail::throw_thread_name(thread_name)
                    << hpx::detail::throw_function(func)
                    << hpx::detail::throw_file(file)
                    << hpx::detail::throw_line(static_cast<int>(line))
                    << hpx::detail::throw_env(env)
                    << hpx::detail::throw_config(config)
                    << hpx::detail::throw_state(state_name)
                    << hpx::detail::throw_auxinfo(auxinfo));
        }
        catch (...) {
            return boost::current_exception();
        }

        // need this return to silence a warning with icc
        HPX_ASSERT(false);
        return boost::exception_ptr();
    }

    template <typename Exception>
    HPX_EXPORT boost::exception_ptr construct_lightweight_exception(
        Exception const& e, std::string const& func, std::string const& file,
        long line)
    {
        // create a boost::exception_ptr object encapsulating the Exception to
        // be thrown and annotate it with all the local information we have
        try {
            throw boost::enable_current_exception(
                boost::enable_error_info(e)
                    << hpx::detail::throw_function(func)
                    << hpx::detail::throw_file(file)
                    << hpx::detail::throw_line(static_cast<int>(line)));
        }
        catch (...) {
            return boost::current_exception();
        }

        // need this return to silence a warning with icc
        HPX_ASSERT(false);
        return boost::exception_ptr();
    }

    template <typename Exception>
    HPX_EXPORT boost::exception_ptr construct_lightweight_exception(Exception const& e)
    {
        // create a boost::exception_ptr object encapsulating the Exception to
        // be thrown and annotate it with all the local information we have
        try {
            boost::throw_exception(e);
        }
        catch (...) {
            return boost::current_exception();
        }

        // need this return to silence a warning with icc
        HPX_ASSERT(false);
        return boost::exception_ptr();
    }

    template HPX_EXPORT boost::exception_ptr
        construct_lightweight_exception(hpx::thread_interrupted const&);

    ///////////////////////////////////////////////////////////////////////////
    template <typename Exception>
    inline bool is_of_lightweight_hpx_category(Exception const& e)
    {
        return false;
    }

    inline bool is_of_lightweight_hpx_category(hpx::exception const& e)
    {
        return e.get_error_code().category() == get_lightweight_hpx_category();
    }

    template <typename Exception>
    HPX_EXPORT boost::exception_ptr
    get_exception(Exception const& e, std::string const& func,
        std::string const& file, long line, std::string const& auxinfo)
    {
        if (is_of_lightweight_hpx_category(e))
            return construct_lightweight_exception(e, func, file, line);

        boost::int64_t pid = ::getpid();
        std::string back_trace(backtrace());

        std::string state_name("not running");
        std::string hostname;
        hpx::runtime* rt = get_runtime_ptr();
        if (rt)
        {
            state rts_state = rt->get_state();
            state_name = get_runtime_state_name(rts_state);

            if (rts_state >= state_initialized &&
                rts_state < state_stopped)
            {
                std::ostringstream strm;
                strm << get_runtime().here();
                hostname = strm.str();
            }
        }

        // if this is not a HPX thread we do not need to query neither for
        // the shepherd thread nor for the thread id
        error_code ec(lightweight);
        boost::uint32_t node = get_locality_id(ec);

        std::size_t shepherd = std::size_t(-1);
        threads::thread_id_type thread_id;
        std::string thread_name;

        threads::thread_self* self = threads::get_self_ptr();
        if (NULL != self)
        {
            if (threads::threadmanager_is(state_running))
                shepherd = hpx::get_worker_thread_num();

            thread_id = threads::get_self_id();
            thread_name = threads::get_thread_description(thread_id);
        }

        std::string env(get_execution_environment());
        std::string config(configuration_string());

        return construct_exception(e, func, file, line, back_trace, node,
            hostname, pid, shepherd, reinterpret_cast<std::size_t>(thread_id.get()),
            thread_name, env, config, state_name, auxinfo);
    }

    template <typename Exception>
    HPX_EXPORT void throw_exception(Exception const& e, std::string const& func,
        std::string const& file, long line)
    {
        if (!expect_exception_flag.load(boost::memory_order_relaxed) &&
            get_config_entry("hpx.attach_debugger", "") == "exception")
        {
            util::attach_debugger();
        }
        boost::rethrow_exception(get_exception(e, func, file, line));
    }

    ///////////////////////////////////////////////////////////////////////////
    template HPX_EXPORT boost::exception_ptr
        get_exception(hpx::exception const&, std::string const&,
        std::string const&, long, std::string const&);

    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(hpx::exception const&,
        std::string const&, std::string const&, long);

    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(boost::system::system_error const&,
        std::string const&, std::string const&, long);

    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::exception const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(hpx::detail::std_exception const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::bad_exception const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(hpx::detail::bad_exception const&,
        std::string const&, std::string const&, long);
#ifndef BOOST_NO_TYPEID
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::bad_typeid const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(hpx::detail::bad_typeid const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::bad_cast const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(hpx::detail::bad_cast const&,
        std::string const&, std::string const&, long);
#endif
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::bad_alloc const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(hpx::detail::bad_alloc const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::logic_error const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::runtime_error const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::out_of_range const&,
        std::string const&, std::string const&, long);
    template HPX_ATTRIBUTE_NORETURN HPX_EXPORT void
        throw_exception(std::invalid_argument const&,
        std::string const&, std::string const&, long);

    ///////////////////////////////////////////////////////////////////////////
    void assertion_failed(char const* expr, char const* function,
        char const* file, long line)
    {
        assertion_failed_msg(expr, expr, function, file, line);
    }

    void assertion_failed_msg(char const* msg, char const* expr,
        char const* function, char const* file, long line)
    {
        if (!expect_exception_flag.load(boost::memory_order_relaxed) &&
            get_config_entry("hpx.attach_debugger", "") == "exception")
        {
            util::attach_debugger();
        }

        bool threw = false;

        std::string str("assertion '" + std::string(msg) + "' failed");
        if (expr != msg)
            str += " (" + std::string(expr) + ")";

        try {
            boost::filesystem::path p(hpx::util::create_path(file));
            hpx::detail::throw_exception(
                hpx::exception(hpx::assertion_failure, str),
                function, p.string(), line);
        }
        catch (...) {
            threw = true;

            // If the runtime pointer is available, we can safely get the prefix
            // of this locality. If it's not available, then just terminate.
            runtime* rt = get_runtime_ptr();
            if (NULL != rt)  {
                rt->report_error(boost::current_exception());
            }
            else {
                std::cerr << "Runtime is not available, reporting error locally. "
                    << hpx::diagnostic_information(boost::current_exception())
                    << std::flush;
            }
        }

        // If the exception wasn't thrown, then print out the assertion message,
        // so that the program doesn't abort without any diagnostics.
        if (!threw) {
            std::cerr << "Runtime is not available, reporting error locally\n"
                         "{what}: " << str << std::endl;
        }
        std::abort();
    }

    ///////////////////////////////////////////////////////////////////////////
    // report an early or late exception and abort
    void report_exception_and_continue(boost::exception_ptr const& e)
    {
        if (!expect_exception_flag.load(boost::memory_order_relaxed) &&
            get_config_entry("hpx.attach_debugger", "") == "exception")
        {
            util::attach_debugger();
        }

        std::cerr << hpx::diagnostic_information(e) << std::endl;
    }

    void report_exception_and_continue(hpx::exception const& e)
    {
        if (!expect_exception_flag.load(boost::memory_order_relaxed) &&
            get_config_entry("hpx.attach_debugger", "") == "exception")
        {
            util::attach_debugger();
        }

        std::cerr << hpx::diagnostic_information(e) << std::endl;
    }

    void report_exception_and_terminate(boost::exception_ptr const& e)
    {
        report_exception_and_continue(e);
        std::abort();
    }

    void report_exception_and_terminate(hpx::exception const& e)
    {
        report_exception_and_continue(e);
        std::abort();
    }

    ///////////////////////////////////////////////////////////////////////////
    boost::exception_ptr access_exception(error_code const& e)
    {
        return e.exception_;
    }
}}

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    /// \brief throw an hpx::exception initialized from the given arguments
    HPX_EXPORT void throw_exception(error e, std::string const& msg,
        std::string const& func, std::string const& file, long line)
    {
        boost::filesystem::path p__(hpx::util::create_path(file));
        boost::rethrow_exception(detail::get_exception(hpx::exception(e, msg),
            func, p__.string(), line));
    }
}
