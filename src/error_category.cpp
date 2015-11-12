//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file error_category.cpp

#include <hpx/config.hpp>
#include <hpx/error_category.hpp>
#include <hpx/error.hpp>

#include <boost/system/error_code.hpp>

#if !defined(BOOST_SYSTEM_NOEXCEPT)
#define BOOST_SYSTEM_NOEXCEPT BOOST_NOEXCEPT
#endif

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    /// \cond NODETAIL
    namespace detail
    {
        class hpx_category : public boost::system::error_category
        {
        public:
            const char* name() const BOOST_SYSTEM_NOEXCEPT
            {
                return "HPX";
            }

            std::string message(int value) const
            {
                if (value >= success && value < last_error)
                    return std::string("HPX(") + error_names[value] + ")"; //-V108

                return "HPX(unknown_error)";
            }
        };

        static hpx_category hpx_category_instance;
    }
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns generic HPX error category used for new errors.
    HPX_EXPORT boost::system::error_category const&
    get_hpx_category()
    {
        return detail::hpx_category_instance;
    }

    /// \cond NODETAIL
    namespace detail
    {
        // this doesn't add any text to the exception what() message
        class hpx_category_rethrow : public boost::system::error_category
        {
        public:
            const char* name() const BOOST_SYSTEM_NOEXCEPT
            {
                return "";
            }

            std::string message(int) const BOOST_NOEXCEPT
            {
                return "";
            }
        };

        static hpx_category_rethrow hpx_category_rethrow_instance;
    }
    /// \endcond

    /// \brief Returns generic HPX error category used for errors re-thrown
    ///        after the exception has been de-serialized.
    HPX_EXPORT boost::system::error_category const&
    get_hpx_rethrow_category()
    {
        return detail::hpx_category_rethrow_instance;
    }

    /// \cond NODETAIL
    namespace detail
    {
        struct lightweight_hpx_category : hpx_category {};

        static lightweight_hpx_category lightweight_hpx_category_instance;
    }

    HPX_EXPORT boost::system::error_category const&
    get_lightweight_hpx_category()
    {
        return detail::lightweight_hpx_category_instance;
    }
    /// \endcond
}
/// \endcond
