//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#include <hpx/error.hpp>
#include <hpx/error_info.hpp>
#include <hpx/exception.hpp>
#include <hpx/runtime.hpp>
#include <hpx/version.hpp>

#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/format.hpp>

#include <cstddef>
#include <sstream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    ///////////////////////////////////////////////////////////////////////////
    // Extract the diagnostic information embedded in the given exception and
    // return a string holding a formatted message.
    std::string diagnostic_information(boost::exception const& e)
    {
        std::ostringstream strm;
        strm << "\n";

        std::string const* back_trace =
            boost::get_error_info<hpx::detail::throw_stacktrace>(e);
        if (back_trace && !back_trace->empty()) {
            // FIXME: add indentation to stack frame information
            strm << "{stack-trace}: " << *back_trace << "\n";
        }

        std::string const* env =
            boost::get_error_info<hpx::detail::throw_env>(e);
        if (env && !env->empty())
            strm << "{env}: " << *env;

        boost::uint32_t const* locality =
            boost::get_error_info<hpx::detail::throw_locality>(e);
        if (locality)
            strm << "{locality-id}: " << *locality << "\n";

        std::string const* hostname_ =
            boost::get_error_info<hpx::detail::throw_hostname>(e);
        if (hostname_ && !hostname_->empty())
            strm << "{hostname}: " << *hostname_ << "\n";

        boost::int64_t const* pid_ =
            boost::get_error_info<hpx::detail::throw_pid>(e);
        if (pid_ && -1 != *pid_)
            strm << "{process-id}: " << *pid_ << "\n";

        char const* const* func =
            boost::get_error_info<boost::throw_function>(e);
        if (func) {
            strm << "{function}: " << *func << "\n";
        }
        else {
            std::string const* s =
                boost::get_error_info<hpx::detail::throw_function>(e);
            if (s)
                strm << "{function}: " << *s << "\n";
        }

        char const* const* file =
            boost::get_error_info<boost::throw_file>(e);
        if (file) {
            strm << "{file}: " << *file << "\n";
        }
        else {
            std::string const* s =
                boost::get_error_info<hpx::detail::throw_file>(e);
            if (s)
                strm << "{file}: " << *s << "\n";
        }

        int const* line =
            boost::get_error_info<boost::throw_line>(e);
        if (line)
            strm << "{line}: " << *line << "\n";

        bool thread_info = false;
        char const* const thread_prefix = "{os-thread}: ";
        std::size_t const* shepherd =
            boost::get_error_info<hpx::detail::throw_shepherd>(e);
        if (shepherd && std::size_t(-1) != *shepherd) {
            strm << thread_prefix << *shepherd;
            thread_info = true;
        }

        std::string thread_name = runtime::get_thread_name();
        if (!thread_info)
            strm << thread_prefix;
        else
            strm << ", ";
        strm << thread_name << "\n";

        std::size_t const* thread_id =
            boost::get_error_info<hpx::detail::throw_thread_id>(e);
        if (thread_id && *thread_id)
            strm << (boost::format("{thread-id}: %016x\n") % *thread_id);

        std::string const* thread_description =
            boost::get_error_info<hpx::detail::throw_thread_name>(e);
        if (thread_description && !thread_description->empty())
            strm << "{thread-description}: " << *thread_description << "\n";

        std::string const* state =
            boost::get_error_info<hpx::detail::throw_state>(e);
        if (state)
            strm << "{state}: " << *state << "\n";

        std::string const* auxinfo =
            boost::get_error_info<hpx::detail::throw_auxinfo>(e);
        if (auxinfo)
            strm << "{auxinfo}: " << *auxinfo << "\n";

        // add full build information
        strm << full_build_string();

        // Try a cast to std::exception - this should handle boost.system
        // error codes in addition to the standard library exceptions.
        std::exception const* se = dynamic_cast<std::exception const*>(&e);
        if (se)
            strm << "{what}: " << se->what() << "\n";

        return strm.str();
    }

    std::string diagnostic_information(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return hpx::diagnostic_information(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string diagnostic_information(hpx::exception const& e)
    {
        return hpx::diagnostic_information(dynamic_cast<boost::exception const&>(e));
    }

    std::string diagnostic_information(hpx::error_code const& e)
    {
        return hpx::diagnostic_information(detail::access_exception(e));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Return the error message.
    std::string get_error_what(boost::exception const& e)
    {
        // Try a cast to std::exception - this should handle boost.system
        // error codes in addition to the standard library exceptions.
        std::exception const* se = dynamic_cast<std::exception const*>(&e);
        return se ? se->what() : std::string();
    }

    std::string get_error_what(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return hpx::get_error_what(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_what(hpx::exception const& e)
    {
        return get_error_what(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_what(hpx::error_code const& e)
    {
        // if this is a lightweight error_code, return canned response
        if (e.category() == hpx::get_lightweight_hpx_category())
            return e.message();

        // extract message from stored exception
        return get_error_what(detail::access_exception(e));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Return the locality where the exception was thrown.
    boost::uint32_t get_error_locality_id(boost::exception const& e)
    {
        boost::uint32_t const* locality =
            boost::get_error_info<hpx::detail::throw_locality>(e);
        if (locality)
            return *locality;
        return naming::invalid_locality_id;
    }

    boost::uint32_t get_error_locality_id(boost::exception_ptr const& e)
    {
        if (!e)
            return naming::invalid_locality_id;

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_locality_id(be);
        }
        catch (...) {
            return naming::invalid_locality_id;
        }
    }

    boost::uint32_t get_error_locality_id(hpx::exception const& e)
    {
        return get_error_locality_id(dynamic_cast<boost::exception const&>(e));
    }

    boost::uint32_t get_error_locality_id(hpx::error_code const& e)
    {
        return get_error_locality_id(detail::access_exception(e));
    }

    ///////////////////////////////////////////////////////////////////////////
    error get_error(hpx::exception const& e)
    {
        return static_cast<hpx::error>(e.get_error());
    }

    error get_error(hpx::error_code const& e)
    {
        return static_cast<hpx::error>(e.value());
    }

    error get_error(boost::exception_ptr const& e)
    {
        try {
            boost::rethrow_exception(e);
        }
        catch (hpx::thread_interrupted const&) {
            return hpx::thread_cancelled;
        }
        catch (hpx::exception const& he) {
            return he.get_error();
        }
        catch (boost::system::system_error const& e) {
            return static_cast<hpx::error>(e.code().value());
        }
        catch (...) {
            return unknown_error;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Return the host-name of the locality where the exception was thrown.
    std::string get_error_host_name(boost::exception const& e)
    {
        std::string const* hostname_ =
            boost::get_error_info<hpx::detail::throw_hostname>(e);
        if (hostname_ && !hostname_->empty())
            return *hostname_;
        return std::string();
    }

    std::string get_error_host_name(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_host_name(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_host_name(hpx::exception const& e)
    {
        return get_error_host_name(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_host_name(hpx::error_code const& e)
    {
        return get_error_host_name(detail::access_exception(e));
    }

    /// Return the (operating system) process id of the locality where the
    /// exception was thrown.
    boost::int64_t get_error_process_id(boost::exception const& e)
    {
        boost::int64_t const* pid_ =
            boost::get_error_info<hpx::detail::throw_pid>(e);
        if (pid_)
            return *pid_;
        return -1;
    }

    boost::int64_t get_error_process_id(boost::exception_ptr const& e)
    {
        if (!e) return -1;

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_process_id(be);
        }
        catch (...) {
            return -1;
        }
    }

    boost::int64_t get_error_process_id(hpx::exception const& e)
    {
        return get_error_process_id(dynamic_cast<boost::exception const&>(e));
    }

    boost::int64_t get_error_process_id(hpx::error_code const& e)
    {
        return get_error_process_id(detail::access_exception(e));
    }

    /// Return the function name from which the exception was thrown.
    std::string get_error_function_name(boost::exception const& e)
    {
        char const* const* func =
            boost::get_error_info<boost::throw_function>(e);
        if (func)
            return *func;

        std::string const* s =
            boost::get_error_info<hpx::detail::throw_function>(e);
        if (s)
            return *s;

        return std::string();
    }

    std::string get_error_function_name(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_function_name(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_function_name(hpx::exception const& e)
    {
        return get_error_function_name(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_function_name(hpx::error_code const& e)
    {
        return get_error_function_name(detail::access_exception(e));
    }

    /// Return the stack backtrace at the point the exception was thrown.
    std::string get_error_backtrace(boost::exception const& e)
    {
        std::string const* back_trace =
            boost::get_error_info<hpx::detail::throw_stacktrace>(e);
        if (back_trace && !back_trace->empty())
            return *back_trace;

        return std::string();
    }

    std::string get_error_backtrace(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_backtrace(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_backtrace(hpx::exception const& e)
    {
        return get_error_backtrace(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_backtrace(hpx::error_code const& e)
    {
        return get_error_backtrace(detail::access_exception(e));
    }

    /// Return the environment of the OS-process at the point the exception
    /// was thrown.
    std::string get_error_env(boost::exception const& e)
    {
        std::string const* env =
            boost::get_error_info<hpx::detail::throw_env>(e);
        if (env && !env->empty())
            return *env;

        return "<unknown>";
    }

    std::string get_error_env(boost::exception_ptr const& e)
    {
        if (!e) return "<unknown>";

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_env(be);
        }
        catch (...) {
            return "<unknown>";
        }
    }

    std::string get_error_env(hpx::exception const& e)
    {
        return get_error_env(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_env(hpx::error_code const& e)
    {
        return get_error_env(detail::access_exception(e));
    }

    /// Return the (source code) file name of the function from which the
    /// exception was thrown.
    std::string get_error_file_name(boost::exception const& e)
    {
        char const* const* file =
            boost::get_error_info<boost::throw_file>(e);
        if (file)
            return *file;

        std::string const* s =
            boost::get_error_info<hpx::detail::throw_file>(e);
        if (s)
            return *s;

        return "<unknown>";
    }

    std::string get_error_file_name(boost::exception_ptr const& e)
    {
        if (!e) return "<unknown>";

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_file_name(be);
        }
        catch (...) {
            return "<unknown>";
        }
    }

    std::string get_error_file_name(hpx::exception const& e)
    {
        return get_error_file_name(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_file_name(hpx::error_code const& e)
    {
        return get_error_file_name(detail::access_exception(e));
    }

    /// Return the line number in the (source code) file of the function from
    /// which the exception was thrown.
    int get_error_line_number(boost::exception const& e)
    {
        int const* line =
            boost::get_error_info<boost::throw_line>(e);
        if (line)
            return *line;
        return -1;
    }

    int get_error_line_number(boost::exception_ptr const& e)
    {
        if (!e) return -1;

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_line_number(be);
        }
        catch (...) {
            return -1;
        }
    }

    int get_error_line_number(hpx::exception const& e)
    {
        return get_error_line_number(dynamic_cast<boost::exception const&>(e));
    }

    int get_error_line_number(hpx::error_code const& e)
    {
        return get_error_line_number(detail::access_exception(e));
    }

    /// Return the sequence number of the OS-thread used to execute HPX-threads
    /// from which the exception was thrown.
    std::size_t get_error_os_thread(boost::exception const& e)
    {
        std::size_t const* shepherd =
            boost::get_error_info<hpx::detail::throw_shepherd>(e);
        if (shepherd && std::size_t(-1) != *shepherd)
            return *shepherd;
        return std::size_t(-1);
    }

    std::size_t get_error_os_thread(boost::exception_ptr const& e)
    {
        if (!e) return std::size_t(-1);

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_os_thread(be);
        }
        catch (...) {
            return std::size_t(-1);
        }
    }

    std::size_t get_error_os_thread(hpx::exception const& e)
    {
        return get_error_os_thread(dynamic_cast<boost::exception const&>(e));
    }

    std::size_t get_error_os_thread(hpx::error_code const& e)
    {
        return get_error_os_thread(detail::access_exception(e));
    }

    /// Return the unique thread id of the HPX-thread from which the exception
    /// was thrown.
    std::size_t get_error_thread_id(boost::exception const& e)
    {
        std::size_t const* thread_id =
            boost::get_error_info<hpx::detail::throw_thread_id>(e);
        if (thread_id && *thread_id)
            return *thread_id;
        return 0;
    }

    std::size_t get_error_thread_id(boost::exception_ptr const& e)
    {
        if (!e) return std::size_t(-1);

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_thread_id(be);
        }
        catch (...) {
            return std::size_t(-1);
        }
    }

    std::size_t get_error_thread_id(hpx::exception const& e)
    {
        return get_error_thread_id(dynamic_cast<boost::exception const&>(e));
    }

    std::size_t get_error_thread_id(hpx::error_code const& e)
    {
        return get_error_thread_id(detail::access_exception(e));
    }

    /// Return any addition thread description of the HPX-thread from which the
    /// exception was thrown.
    std::string get_error_thread_description(boost::exception const& e)
    {
        std::string const* thread_description =
            boost::get_error_info<hpx::detail::throw_thread_name>(e);
        if (thread_description && !thread_description->empty())
            return *thread_description;
        return std::string();
    }

    std::string get_error_thread_description(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_thread_description(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_thread_description(hpx::exception const& e)
    {
        return get_error_thread_description(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_thread_description(hpx::error_code const& e)
    {
        return get_error_thread_description(detail::access_exception(e));
    }

    /// Return the HPX configuration information point from which the
    /// exception was thrown.
    std::string get_error_config(boost::exception const& e)
    {
        std::string const* config_info =
            boost::get_error_info<hpx::detail::throw_config>(e);
        if (config_info && !config_info->empty())
            return *config_info;
        return std::string();
    }

    std::string get_error_config(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_config(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_config(hpx::exception const& e)
    {
        return get_error_config(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_config(hpx::error_code const& e)
    {
        return get_error_config(detail::access_exception(e));
    }

    /// Return the HPX runtime state information at which the exception was
    /// thrown.
    std::string get_error_state(boost::exception const& e)
    {
        std::string const* state_info =
            boost::get_error_info<hpx::detail::throw_state>(e);
        if (state_info && !state_info->empty())
            return *state_info;
        return std::string();
    }

    std::string get_error_state(boost::exception_ptr const& e)
    {
        if (!e) return std::string();

        try {
            boost::rethrow_exception(e);
        }
        catch (boost::exception const& be) {
            return get_error_state(be);
        }
        catch (...) {
            return std::string();
        }
    }

    std::string get_error_state(hpx::exception const& e)
    {
        return get_error_state(dynamic_cast<boost::exception const&>(e));
    }

    std::string get_error_state(hpx::error_code const& e)
    {
        return get_error_state(detail::access_exception(e));
    }
}
