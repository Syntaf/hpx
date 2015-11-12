//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file error_category.hpp

#ifndef HPX_ERROR_CATEGORY_HPP
#define HPX_ERROR_CATEGORY_HPP

#include <hpx/config.hpp>

#include <boost/system/error_code.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns generic HPX error category used for new errors.
    HPX_EXPORT boost::system::error_category const& get_hpx_category();

    /// \brief Returns generic HPX error category used for errors re-thrown
    ///        after the exception has been de-serialized.
    HPX_EXPORT boost::system::error_category const& get_hpx_rethrow_category();

    /// \cond NOINTERNAL
    HPX_EXPORT boost::system::error_category const& get_lightweight_hpx_category();
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Encode error category for new error_code.
    enum throwmode
    {
        plain = 0,
        rethrow = 1,
        lightweight = 0x80, // do not generate an exception for this error_code
        /// \cond NODETAIL
        lightweight_rethrow = lightweight | rethrow
        /// \endcond
    };

    /// \cond NOINTERNAL
    inline boost::system::error_category const& get_hpx_category(throwmode mode)
    {
        switch(mode) {
        case rethrow:
            return get_hpx_rethrow_category();

        case lightweight:
        case lightweight_rethrow:
            return get_lightweight_hpx_category();

        case plain:
        default:
            break;
        }
        return get_hpx_category();
    }
    /// \endcond
}

#endif
