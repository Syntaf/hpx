//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file exception.hpp

#if !defined(HPX_EXCEPTION_MAR_24_2008_0929AM)
#define HPX_EXCEPTION_MAR_24_2008_0929AM

#include <hpx/config.hpp>
#include <hpx/error.hpp>
#include <hpx/error_category.hpp>
#include <hpx/error_info.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/util/assert.hpp>
#include <hpx/util/logging.hpp>
#include <hpx/util/filesystem_compatibility.hpp>
#include <hpx/util/unused.hpp>

#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/throw_exception.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <exception>
#include <string>
#include <iosfwd>

#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    /// \cond NODETAIL
    namespace detail
    {
        HPX_EXPORT boost::exception_ptr access_exception(error_code const&);

        ///////////////////////////////////////////////////////////////////////
        struct command_line_error : std::logic_error
        {
            explicit command_line_error(char const* msg)
              : std::logic_error(msg)
            {}

            explicit command_line_error(std::string const& msg)
              : std::logic_error(msg)
            {}
        };
    } // namespace detail
    /// \endcond

    inline boost::system::error_code
    make_system_error_code(error e, throwmode mode = plain)
    {
        return boost::system::error_code(
            static_cast<int>(e), get_hpx_category(mode));
    }

    ///////////////////////////////////////////////////////////////////////////
    inline boost::system::error_condition
    make_error_condition(error e, throwmode mode)
    {
        return boost::system::error_condition(
            static_cast<int>(e), get_hpx_category(mode));
    }
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A hpx::error_code represents an arbitrary error condition.
    ///
    /// The class hpx::error_code describes an object used to hold error code
    /// values, such as those originating from the operating system or other
    /// low-level application program interfaces.
    ///
    /// \note Class hpx::error_code is an adjunct to error reporting by
    /// exception
    ///
    class error_code : public boost::system::error_code //-V690
    {
    public:
        /// Construct an object of type error_code.
        ///
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws nothing
        explicit error_code(throwmode mode = plain)
          : boost::system::error_code(make_system_error_code(success, mode))
        {}

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws nothing
        inline explicit error_code(error e, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param func   The name of the function where the error was raised.
        /// \param file   The file name of the code where the error was raised.
        /// \param line   The line number of the code line where the error was
        ///               raised.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws nothing
        inline error_code(error e, char const* func, char const* file,
            long line, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        inline error_code(error e, char const* msg, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param func   The name of the function where the error was raised.
        /// \param file   The file name of the code where the error was raised.
        /// \param line   The line number of the code line where the error was
        ///               raised.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        inline error_code(error e, char const* msg, char const* func,
                char const* file, long line, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        inline error_code(error e, std::string const& msg, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param func   The name of the function where the error was raised.
        /// \param file   The file name of the code where the error was raised.
        /// \param line   The line number of the code line where the error was
        ///               raised.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        inline error_code(error e, std::string const& msg, char const* func,
                char const* file, long line, throwmode mode = plain);

        /// Return a reference to the error message stored in the hpx::error_code.
        ///
        /// \throws nothing
        inline std::string get_message() const;

        /// \brief Clear this error_code object.
        /// The postconditions of invoking this method are
        /// * value() == hpx::success and category() == hpx::get_hpx_category()
        void clear()
        {
            this->boost::system::error_code::assign(success, get_hpx_category());
            exception_ = boost::exception_ptr();
        }

        /// Assignment operator for error_code
        ///
        /// \note This function maintains the error category of the left hand
        ///       side if the right hand side is a success code.
        inline error_code& operator=(error_code const& rhs);

    private:
        friend boost::exception_ptr detail::access_exception(error_code const&);
        friend class exception;
        friend error_code make_error_code(boost::exception_ptr const&);

        inline error_code(int err, hpx::exception const& e);
        inline explicit error_code(boost::exception_ptr const& e);

        boost::exception_ptr exception_;
    };

    /// @{
    /// \brief Returns a new error_code constructed from the given parameters.
    inline error_code
    make_error_code(error e, throwmode mode = plain)
    {
        return error_code(e, mode);
    }
    inline error_code
    make_error_code(error e, char const* func, char const* file, long line,
        throwmode mode = plain)
    {
        return error_code(e, func, file, line, mode);
    }

    /// \brief Returns error_code(e, msg, mode).
    inline error_code
    make_error_code(error e, char const* msg, throwmode mode = plain)
    {
        return error_code(e, msg, mode);
    }
    inline error_code
    make_error_code(error e, char const* msg, char const* func,
        char const* file, long line, throwmode mode = plain)
    {
        return error_code(e, msg, func, file, line, mode);
    }

    /// \brief Returns error_code(e, msg, mode).
    inline error_code
    make_error_code(error e, std::string const& msg, throwmode mode = plain)
    {
        return error_code(e, msg, mode);
    }
    inline error_code
    make_error_code(error e, std::string const& msg, char const* func,
        char const* file, long line, throwmode mode = plain)
    {
        return error_code(e, msg, func, file, line, mode);
    }
    inline error_code
    make_error_code(boost::exception_ptr const& e)
    {
        return error_code(e);
    }
    ///@}

    /// \brief Returns error_code(hpx::success, "success", mode).
    inline error_code
    make_success_code(throwmode mode = plain)
    {
        return error_code(mode);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A hpx::exception is the main exception type used by HPX to
    ///        report errors.
    ///
    /// The hpx::exception type is the main exception type  used by HPX to
    /// report errors. Any exceptions thrown by functions in the HPX library
    /// are either of this type or of a type derived from it. This implies that
    /// it is always safe to use this type only in catch statements guarding
    /// HPX library calls.
    class HPX_EXCEPTION_EXPORT exception : public boost::system::system_error
    {
    public:
        /// Construct a hpx::exception from a \a hpx::error.
        ///
        /// \param e    The parameter \p e holds the hpx::error code the new
        ///             exception should encapsulate.
        explicit exception(error e = success)
          : boost::system::system_error(make_error_code(e, plain))
        {
            HPX_ASSERT(e >= success && e < last_error);
            LERR_(error) << "created exception: " << this->what();
        }

        /// Construct a hpx::exception from a boost#system_error.
        explicit exception(boost::system::system_error const& e)
          : boost::system::system_error(e)
        {
            LERR_(error) << "created exception: " << this->what();
        }

        /// Construct a hpx::exception from a \a hpx::error and an error message.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the returned
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        exception(error e, char const* msg, throwmode mode = plain)
          : boost::system::system_error(make_system_error_code(e, mode), msg)
        {
            HPX_ASSERT(e >= success && e < last_error);
            LERR_(error) << "created exception: " << this->what();
        }

        /// Construct a hpx::exception from a \a hpx::error and an error message.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the returned
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        exception(error e, std::string const& msg, throwmode mode = plain)
          : boost::system::system_error(make_system_error_code(e, mode), msg)
        {
            HPX_ASSERT(e >= success && e < last_error);
            LERR_(error) << "created exception: " << this->what();
        }

        /// Destruct a hpx::exception
        ///
        /// \throws nothing
        ~exception() throw()
        {
        }

        /// The function \a get_error() returns the hpx::error code stored
        /// in the referenced instance of a hpx::exception. It returns
        /// the hpx::error code this exception instance was constructed
        /// from.
        ///
        /// \throws nothing
        error get_error() const throw()
        {
            return static_cast<error>(
                this->boost::system::system_error::code().value());
        }

        /// The function \a get_error_code() returns a hpx::error_code which
        /// represents the same error condition as this hpx::exception instance.
        ///
        /// \param mode   The parameter \p mode specifies whether the returned
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        error_code get_error_code(throwmode mode = plain) const throw()
        {
            HPX_UNUSED(mode);
            return error_code(this->boost::system::system_error::code().value(),
                *this);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A hpx::thread_interrupted is the exception type used by HPX to
    ///        interrupt a running HPX thread.
    ///
    /// The \a hpx::thread_interrupted type is the exception type used by HPX to
    /// interrupt a running thread.
    ///
    /// A running thread can be interrupted by invoking the interrupt() member
    /// function of the corresponding hpx::thread object. When the interrupted
    /// thread next executes one of the specified interruption points (or if it
    /// is currently blocked whilst executing one) with interruption enabled,
    /// then a hpx::thread_interrupted exception will be thrown in the interrupted
    /// thread. If not caught, this will cause the execution of the interrupted
    /// thread to terminate. As with any other exception, the stack will be
    /// unwound, and destructors for objects of automatic storage duration will
    /// be executed.
    ///
    /// If a thread wishes to avoid being interrupted, it can create an instance
    /// of \a hpx::this_thread::disable_interruption. Objects of this class disable
    /// interruption for the thread that created them on construction, and
    /// restore the interruption state to whatever it was before on destruction.
    ///
    /// \code
    ///     void f()
    ///     {
    ///         // interruption enabled here
    ///         {
    ///             hpx::this_thread::disable_interruption di;
    ///             // interruption disabled
    ///             {
    ///                 hpx::this_thread::disable_interruption di2;
    ///                 // interruption still disabled
    ///             } // di2 destroyed, interruption state restored
    ///             // interruption still disabled
    ///         } // di destroyed, interruption state restored
    ///         // interruption now enabled
    ///     }
    /// \endcode
    ///
    /// The effects of an instance of \a hpx::this_thread::disable_interruption can be
    /// temporarily reversed by constructing an instance of
    /// \a hpx::this_thread::restore_interruption, passing in the
    /// \a hpx::this_thread::disable_interruption object in question. This will restore
    /// the interruption state to what it was when the
    /// \a hpx::this_thread::disable_interruption
    /// object was constructed, and then disable interruption again when the
    /// \a hpx::this_thread::restore_interruption object is destroyed.
    ///
    /// \code
    ///     void g()
    ///     {
    ///         // interruption enabled here
    ///         {
    ///             hpx::this_thread::disable_interruption di;
    ///             // interruption disabled
    ///             {
    ///                 hpx::this_thread::restore_interruption ri(di);
    ///                 // interruption now enabled
    ///             } // ri destroyed, interruption disable again
    ///         } // di destroyed, interruption state restored
    ///         // interruption now enabled
    ///     }
    /// \endcode
    ///
    /// At any point, the interruption state for the current thread can be
    /// queried by calling \a hpx::this_thread::interruption_enabled().
    struct HPX_EXCEPTION_EXPORT thread_interrupted : std::exception {};

    /// \cond NODETAIL
    namespace detail
    {
        struct HPX_EXCEPTION_EXPORT std_exception : std::exception
        {
          private:
            std::string what_;

          public:
            explicit std_exception(std::string const& w)
              : what_(w)
            {}

            ~std_exception() throw() {}

            const char* what() const throw()
            {
                return what_.c_str();
            }
        };

        struct HPX_EXCEPTION_EXPORT bad_alloc : std::bad_alloc
        {
          private:
            std::string what_;

          public:
            explicit bad_alloc(std::string const& w)
              : what_(w)
            {}

            ~bad_alloc() throw() {}

            const char* what() const throw()
            {
                return what_.c_str();
            }
        };

        struct HPX_EXCEPTION_EXPORT bad_exception : std::bad_exception
        {
          private:
            std::string what_;

          public:
            explicit bad_exception(std::string const& w)
              : what_(w)
            {}

            ~bad_exception() throw() {}

            const char* what() const throw()
            {
                return what_.c_str();
            }
        };

#ifndef BOOST_NO_TYPEID
        struct HPX_EXCEPTION_EXPORT bad_cast : std::bad_cast
        {
          private:
            std::string what_;

          public:
            explicit bad_cast(std::string const& w)
              : what_(w)
            {}

            ~bad_cast() throw() {}

            const char* what() const throw()
            {
                return what_.c_str();
            }
        };

        struct HPX_EXCEPTION_EXPORT bad_typeid : std::bad_typeid
        {
          private:
            std::string what_;

          public:
            explicit bad_typeid(std::string const& w)
              : what_(w)
            {}

            ~bad_typeid() throw() {}

            const char* what() const throw()
            {
                return what_.c_str();
            }
        };
#endif

        // construct an exception, internal helper
        template <typename Exception>
        HPX_EXPORT boost::exception_ptr
            construct_exception(Exception const& e,
                std::string const& func, std::string const& file, long line,
                std::string const& back_trace = "", boost::uint32_t node = 0,
                std::string const& hostname = "", boost::int64_t pid = -1,
                std::size_t shepherd = ~0, std::size_t thread_id = 0,
                std::string const& thread_name = "",
                std::string const& env = "", std::string const& config = "",
                std::string const& state = "", std::string const& auxinfo = "");

        template <typename Exception>
        HPX_EXPORT boost::exception_ptr
            construct_lightweight_exception(Exception const& e);

        // HPX_ASSERT handler
        HPX_ATTRIBUTE_NORETURN HPX_EXPORT
        void assertion_failed(char const* expr, char const* function,
            char const* file, long line);

        // HPX_ASSERT_MSG handler
        HPX_ATTRIBUTE_NORETURN HPX_EXPORT
        void assertion_failed_msg(char const* msg, char const* expr,
            char const* function, char const* file, long line);

        // If backtrace support is enabled, this function returns the current
        // stack backtrace, otherwise it will return an empty string.
        HPX_EXPORT std::string backtrace(
            std::size_t frames = HPX_THREAD_BACKTRACE_ON_SUSPENSION_DEPTH);
        HPX_EXPORT std::string backtrace_direct(
            std::size_t frames = HPX_THREAD_BACKTRACE_ON_SUSPENSION_DEPTH);

        // Portably extract the current execution environment
        HPX_EXPORT std::string get_execution_environment();

        // Report an early or late exception and locally abort execution. There
        // isn't anything more we could do.
        HPX_EXPORT void report_exception_and_terminate(boost::exception_ptr const&);
        HPX_EXPORT void report_exception_and_terminate(hpx::exception const&);

        // Report an early or late exception and locally exit execution. There
        // isn't anything more we could do. The exception will be re-thrown
        // from hpx::init
        HPX_EXPORT void report_exception_and_continue(boost::exception_ptr const&);
        HPX_EXPORT void report_exception_and_continue(hpx::exception const&);
    }
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////
    // \cond NOINTERNAL
    inline error_code::error_code(error e, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight))
            exception_ = detail::get_exception(hpx::exception(e, "", mode));
    }

    inline error_code::error_code(error e, char const* func,
            char const* file, long line, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight)) {
            boost::filesystem::path p(hpx::util::create_path(file));
            exception_ = detail::get_exception(hpx::exception(e, "", mode),
                func, p.string(), line);
        }
    }

    inline error_code::error_code(error e, char const* msg, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight))
            exception_ = detail::get_exception(hpx::exception(e, msg, mode));
    }

    inline error_code::error_code(error e, char const* msg,
            char const* func, char const* file, long line, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight)) {
            boost::filesystem::path p(hpx::util::create_path(file));
            exception_ = detail::get_exception(hpx::exception(e, msg, mode),
                func, p.string(), line);
        }
    }

    inline error_code::error_code(error e, std::string const& msg,
            throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight))
            exception_ = detail::get_exception(hpx::exception(e, msg, mode));
    }

    inline error_code::error_code(error e, std::string const& msg,
            char const* func, char const* file, long line, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight)) {
            boost::filesystem::path p(hpx::util::create_path(file));
            exception_ = detail::get_exception(hpx::exception(e, msg, mode),
                func, p.string(), line);
        }
    }

    inline boost::exception_ptr get_exception_ptr(hpx::exception const& e)
    {
        try {
            throw e;
        }
        catch (...) {
            return boost::current_exception();
        }
    }

    inline error_code::error_code(int err, hpx::exception const& e)
    {
        this->boost::system::error_code::assign(err, get_hpx_category());
        try {
            throw e;
        }
        catch (...) {
            exception_ = get_exception_ptr(e);
        }
    }

    inline error_code::error_code(boost::exception_ptr const& e)
      : boost::system::error_code(make_system_error_code(get_error(e), rethrow)),
        exception_(e)
    {}

    ///////////////////////////////////////////////////////////////////////////
    inline std::string error_code::get_message() const
    {
        if (exception_) {
            try {
                boost::rethrow_exception(exception_);
            }
            catch (boost::exception const& be) {
                return dynamic_cast<std::exception const*>(&be)->what();
            }
        }
        return get_error_what(*this);   // provide at least minimal error text
    }

    ///////////////////////////////////////////////////////////////////////////
    inline error_code& error_code::operator=(error_code const& rhs)
    {
        if (this != &rhs) {
            if (rhs.value() == success) {
                // if the rhs is a success code, we maintain our throw mode
                this->boost::system::error_code::operator=(
                    make_success_code(
                        (category() == get_lightweight_hpx_category()) ?
                            hpx::lightweight : hpx::plain));
            }
            else {
                this->boost::system::error_code::operator=(rhs);
            }
            exception_ = rhs.exception_;
        }
        return *this;
    }

        // forwarder for HPX_ASSERT handler
    inline void assertion_failed(char const* expr, char const* function,
        char const* file, long line)
    {
        hpx::detail::assertion_failed(expr, function, file, line);
    }

    // forwarder for HPX_ASSERT_MSG handler
    inline void assertion_failed_msg(char const* msg, char const* expr,
        char const* function, char const* file, long line)
    {
        hpx::detail::assertion_failed_msg(msg, expr, function, file, line);
    }

    // For testing purposes we sometime expect to see exceptions, allow those
    // to go through without attaching a debugger.
    //
    // This should be used carefully as it disables the possible attaching of
    // a debugger for all exceptions, not only the expected ones.
    HPX_EXPORT bool expect_exception(bool flag = true);
    /// \endcond
}

/// \cond NOEXTERNAL
namespace boost
{
    namespace system
    {
        // make sure our errors get recognized by the Boost.System library
        template<> struct is_error_code_enum<hpx::error>
        {
            static const bool value = true;
        };

        template<> struct is_error_condition_enum<hpx::error>
        {
            static const bool value = true;
        };
    }
}
/// \endcond

#include <hpx/config/warnings_suffix.hpp>

#endif

