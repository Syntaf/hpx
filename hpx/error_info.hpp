//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file error_info.hpp

#ifndef HPX_ERROR_INFO_HPP
#define HPX_ERROR_INFO_HPP

#include <hpx/config.hpp>
#include <hpx/error.hpp>
#include <hpx/exception_fwd.hpp>

#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>

#include <string>

#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    /// \cond NODETAIL
    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        // types needed to add additional information to the thrown exceptions
        struct tag_throw_locality {};
        struct tag_throw_hostname {};
        struct tag_throw_pid {};
        struct tag_throw_shepherd {};
        struct tag_throw_thread_id {};
        struct tag_throw_thread_name {};
        struct tag_throw_file {};
        struct tag_throw_function {};
        struct tag_throw_stacktrace {};
        struct tag_throw_env {};
        struct tag_throw_config {};
        struct tag_throw_state {};
        struct tag_throw_auxinfo {};

        // Stores the information about the locality id the exception has been
        // raised on. This information will show up in error messages under the
        // [locality] tag.
        typedef boost::error_info<detail::tag_throw_locality, boost::uint32_t>
            throw_locality;

        // Stores the information about the hostname of the locality the exception
        // has been raised on. This information will show up in error messages
        // under the [hostname] tag.
        typedef boost::error_info<detail::tag_throw_hostname, std::string>
            throw_hostname;

        // Stores the information about the pid of the OS process the exception
        // has been raised on. This information will show up in error messages
        // under the [pid] tag.
        typedef boost::error_info<detail::tag_throw_pid, boost::int64_t>
            throw_pid;

        // Stores the information about the shepherd thread the exception has been
        // raised on. This information will show up in error messages under the
        // [shepherd] tag.
        typedef boost::error_info<detail::tag_throw_shepherd, std::size_t>
            throw_shepherd;

        // Stores the information about the HPX thread the exception has been
        // raised on. This information will show up in error messages under the
        // [thread_id] tag.
        typedef boost::error_info<detail::tag_throw_thread_id, std::size_t>
            throw_thread_id;

        // Stores the information about the HPX thread name the exception has been
        // raised on. This information will show up in error messages under the
        // [thread_name] tag.
        typedef boost::error_info<detail::tag_throw_thread_name, std::string>
            throw_thread_name;

        // Stores the information about the function name the exception has been
        // raised in. This information will show up in error messages under the
        // [function] tag.
        typedef boost::error_info<detail::tag_throw_function, std::string>
            throw_function;

        // Stores the information about the source file name the exception has
        // been raised in. This information will show up in error messages under
        // the [file] tag.
        typedef boost::error_info<detail::tag_throw_file, std::string>
            throw_file;

        // Stores the information about the source file line number the exception
        // has been raised at. This information will show up in error messages
        // under the [line] tag.
        using boost::throw_line;

        // Stores the information about the stack backtrace at the point the
        // exception has been raised at. This information will show up in error
        // messages under the [stack_trace] tag.
        typedef boost::error_info<detail::tag_throw_stacktrace, std::string>
            throw_stacktrace;

        // Stores the full execution environment of the locality the exception
        // has been raised in. This information will show up in error messages
        // under the [env] tag.
        typedef boost::error_info<detail::tag_throw_env, std::string>
            throw_env;

        // Stores the full HPX configuration information of the locality the
        // exception has been raised in. This information will show up in error
        // messages under the [config] tag.
        typedef boost::error_info<detail::tag_throw_config, std::string>
            throw_config;

        // Stores the current runtime state. This information will show up in
        // error messages under the [state] tag.
        typedef boost::error_info<detail::tag_throw_state, std::string>
            throw_state;

        // Stores additional auxiliary information (such as information about
        // the current parcel). This information will show up in error messages
        // under the [auxinfo] tag.
        typedef boost::error_info<detail::tag_throw_auxinfo, std::string>
            throw_auxinfo;
    }
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extract the diagnostic information embedded in the given
    /// exception and return a string holding a formatted message.
    ///
    /// The function \a hpx::diagnostic_information can be used to extract all
    /// diagnostic information stored in the given exception instance as a
    /// formatted string. This simplifies debug output as it composes the
    /// diagnostics into one, easy to use function call. This includes
    /// the name of the source file and line number, the sequence number of the
    /// OS-thread and the HPX-thread id, the locality id and the stack backtrace
    /// of the point where the original exception was thrown.
    ///
    /// \param e    The parameter \p e will be inspected for all diagnostic
    ///             information elements which have been stored at the point
    ///             where the exception was thrown. This parameter can be one
    ///             of the following types: \a hpx::exception or
    ///             \a hpx::error_code.
    ///
    /// \returns    The formatted string holding all of the available
    ///             diagnostic information stored in the given exception
    ///             instance.
    ///
    /// \throws     std#bad_alloc (if any of the required allocation operations
    ///             fail)
    ///
    /// \see        \a hpx::get_error_locality_id(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string diagnostic_information(hpx::exception const& e);

    /// \copydoc diagnostic_information(hpx::exception const& e)
    HPX_EXPORT std::string diagnostic_information(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string diagnostic_information(boost::exception const& e);
    HPX_EXPORT std::string diagnostic_information(boost::exception_ptr const& e);
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////
    // Extract elements of the diagnostic information embedded in the given
    // exception.

    /// \brief Return the error message of the thrown exception.
    ///
    /// The function \a hpx::get_error_what can be used to extract the
    /// diagnostic information element representing the error message as stored
    /// in the given exception instance.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \returns    The error message stored in the exception
    ///             If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error()
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_config(), \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string get_error_what(hpx::exception const& e);

    /// \copydoc get_error_locality_id(hpx::exception const& e)
    HPX_EXPORT std::string get_error_what(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_what(boost::exception const& e);
    HPX_EXPORT std::string get_error_what(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the locality id where the exception was thrown.
    ///
    /// The function \a hpx::get_error_locality_id can be used to extract the
    /// diagnostic information element representing the locality id as stored
    /// in the given exception instance.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \returns    The locality id of the locality where the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return
    ///             \a hpx::naming#invalid_locality_id.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT boost::uint32_t get_error_locality_id(hpx::exception const& e);

    /// \copydoc get_error_locality_id(hpx::exception const& e)
    HPX_EXPORT boost::uint32_t get_error_locality_id(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT boost::uint32_t get_error_locality_id(boost::exception const& e);
    HPX_EXPORT boost::uint32_t get_error_locality_id(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the locality id where the exception was thrown.
    ///
    /// The function \a hpx::get_error can be used to extract the
    /// diagnostic information element representing the error value code as
    /// stored in the given exception instance.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, or \a boost::exception_ptr.
    ///
    /// \returns    The error value code of the locality where the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return
    ///             \a hpx::naming#invalid_locality_id.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT error get_error(hpx::exception const& e);

    /// \copydoc get_error(hpx::exception const& e)
    HPX_EXPORT error get_error(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT error get_error(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the hostname of the locality where the exception was
    ///        thrown.
    ///
    /// The function \a hpx::get_error_host_name can be used to extract the
    /// diagnostic information element representing the host name as stored in
    /// the given exception instance.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \returns    The hostname of the locality where the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return and empty string.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information()
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error()
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string get_error_host_name(hpx::exception const& e);

    /// \copydoc get_error_host_name(hpx::exception const& e)
    HPX_EXPORT std::string get_error_host_name(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_host_name(boost::exception const& e);
    HPX_EXPORT std::string get_error_host_name(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the (operating system) process id of the locality where
    ///        the exception was thrown.
    ///
    /// The function \a hpx::get_error_process_id can be used to extract the
    /// diagnostic information element representing the process id as stored in
    /// the given exception instance.
    ///
    /// \returns    The process id of the OS-process which threw the exception
    ///             If the exception instance does not hold
    ///             this information, the function will return 0.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT boost::int64_t get_error_process_id(hpx::exception const& e);

    /// \copydoc get_error_process_id(hpx::exception const& e)
    HPX_EXPORT boost::int64_t get_error_process_id(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT boost::int64_t get_error_process_id(boost::exception const& e);
    HPX_EXPORT boost::int64_t get_error_process_id(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the environment of the OS-process at the point the
    ///        exception was thrown.
    ///
    /// The function \a hpx::get_error_env can be used to extract the
    /// diagnostic information element representing the environment of the
    /// OS-process collected at the point the exception was thrown.
    ///
    /// \returns    The environment from the point the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string get_error_env(hpx::exception const& e);

    /// \copydoc get_error_env(hpx::exception const& e)
    HPX_EXPORT std::string get_error_env(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_env(boost::exception const& e);
    HPX_EXPORT std::string get_error_env(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the function name from which the exception was thrown.
    ///
    /// The function \a hpx::get_error_function_name can be used to extract the
    /// diagnostic information element representing the name of the function
    /// as stored in the given exception instance.
    ///
    /// \returns    The name of the function from which the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id()
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string get_error_function_name(hpx::exception const& e);

    /// \copydoc get_error_function_name(hpx::exception const& e)
    HPX_EXPORT std::string get_error_function_name(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_function_name(boost::exception const& e);
    HPX_EXPORT std::string get_error_function_name(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the stack backtrace from the point the exception was thrown.
    ///
    /// The function \a hpx::get_error_backtrace can be used to extract the
    /// diagnostic information element representing the stack backtrace
    /// collected at the point the exception was thrown.
    ///
    /// \returns    The stack back trace from the point the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string get_error_backtrace(hpx::exception const& e);

    /// \copydoc get_error_backtrace(hpx::exception const& e)
    HPX_EXPORT std::string get_error_backtrace(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_backtrace(boost::exception const& e);
    HPX_EXPORT std::string get_error_backtrace(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the (source code) file name of the function from which
    ///        the exception was thrown.
    ///
    /// The function \a hpx::get_error_file_name can be used to extract the
    /// diagnostic information element representing the name of the source file
    /// as stored in the given exception instance.
    ///
    /// \returns    The name of the source file of the function from which the
    ///             exception was thrown. If the exception instance does
    ///             not hold this information, the function will return an empty
    ///             string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::string get_error_file_name(hpx::exception const& e);

    /// \copydoc get_error_file_name(hpx::exception const& e)
    HPX_EXPORT std::string get_error_file_name(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_file_name(boost::exception const& e);
    HPX_EXPORT std::string get_error_file_name(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the line number in the (source code) file of the function
    ///        from which the exception was thrown.
    ///
    /// The function \a hpx::get_error_line_number can be used to extract the
    /// diagnostic information element representing the line number
    /// as stored in the given exception instance.
    ///
    /// \returns    The line number of the place where the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return -1.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name()
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT int get_error_line_number(hpx::exception const& e);

    /// \copydoc get_error_line_number(hpx::exception const& e)
    HPX_EXPORT int get_error_line_number(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT int get_error_line_number(boost::exception const& e);
    HPX_EXPORT int get_error_line_number(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the sequence number of the OS-thread used to execute
    ///        HPX-threads from which the exception was thrown.
    ///
    /// The function \a hpx::get_error_os_thread can be used to extract the
    /// diagnostic information element representing the sequence number  of the
    /// OS-thread as stored in the given exception instance.
    ///
    /// \returns    The sequence number of the OS-thread used to execute the
    ///             HPX-thread from which the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return std::size(-1).
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::size_t get_error_os_thread(hpx::exception const& e);

    /// \copydoc get_error_os_thread(hpx::exception const& e)
    HPX_EXPORT std::size_t get_error_os_thread(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::size_t get_error_os_thread(boost::exception const& e);
    HPX_EXPORT std::size_t get_error_os_thread(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the unique thread id of the HPX-thread from which the
    ///        exception was thrown.
    ///
    /// The function \a hpx::get_error_thread_id can be used to extract the
    /// diagnostic information element representing the HPX-thread id
    /// as stored in the given exception instance.
    ///
    /// \returns    The unique thread id of the HPX-thread from which the
    ///             exception was thrown. If the exception instance
    ///             does not hold this information, the function will return
    ///             std::size_t(0).
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread()
    ///             \a hpx::get_error_thread_description(), \a hpx::get_error(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT std::size_t get_error_thread_id(hpx::exception const& e);

    /// \copydoc get_error_thread_id(hpx::exception const& e)
    HPX_EXPORT std::size_t get_error_thread_id(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::size_t get_error_thread_id(boost::exception const& e);
    HPX_EXPORT std::size_t get_error_thread_id(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return any additionally available thread description of the
    ///        HPX-thread from which the exception was thrown.
    ///
    /// The function \a hpx::get_error_thread_description can be used to extract the
    /// diagnostic information element representing the additional thread
    /// description as stored in the given exception instance.
    ///
    /// \returns    Any additionally available thread description of the
    ///             HPX-thread from which the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error(), \a hpx::get_error_state(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config()
    ///
    HPX_EXPORT std::string get_error_thread_description(hpx::exception const& e);

    /// \copydoc get_error_thread_description(hpx::exception const& e)
    HPX_EXPORT std::string get_error_thread_description(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_thread_description(boost::exception const& e);
    HPX_EXPORT std::string get_error_thread_description(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the HPX configuration information point from which the
    ///        exception was thrown.
    ///
    /// The function \a hpx::get_error_config can be used to extract the
    /// HPX configuration information element representing the full HPX
    /// configuration information as stored in the given exception instance.
    ///
    /// \returns    Any additionally available HPX configuration information
    ///             the point from which the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error(), \a hpx::get_error_state()
    ///             \a hpx::get_error_what(), \a hpx::get_error_thread_description()
    ///
    HPX_EXPORT std::string get_error_config(hpx::exception const& e);

    /// \copydoc get_error_config(hpx::exception const& e)
    HPX_EXPORT std::string get_error_config(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_config(boost::exception const& e);
    HPX_EXPORT std::string get_error_config(boost::exception_ptr const& e);
    /// \endcond

    /// \brief Return the HPX runtime state information at which the exception
    ///        was thrown.
    ///
    /// The function \a hpx::get_error_state can be used to extract the
    /// HPX runtime state information element representing the state the
    /// runtime system is currently in as stored in the given exception
    /// instance.
    ///
    /// \returns    The point runtime state at the point at which the exception
    ///             was thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_thread_description()
    ///
    HPX_EXPORT std::string get_error_state(hpx::exception const& e);

    /// \copydoc get_error_state(hpx::exception const& e)
    HPX_EXPORT std::string get_error_state(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_state(boost::exception const& e);
    HPX_EXPORT std::string get_error_state(boost::exception_ptr const& e);
    /// \endcond
}

#include <hpx/config/warnings_suffix.hpp>

#endif
