//  Copyright (c) 2011 Thomas Heller
//  Copyright (c) 2013 Hartmut Kaiser
//  Copyright (c) 2014-2015 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_DETAIL_VTABLE_CALLABLE_VTABLE_HPP
#define HPX_UTIL_DETAIL_VTABLE_CALLABLE_VTABLE_HPP

#include <hpx/config.hpp>
#include <hpx/util/detail/vtable/vtable.hpp>
#include <hpx/util/invoke.hpp>

#include <utility>

namespace hpx { namespace util { namespace detail
{
    template <typename Sig>
    struct callable_vtable;

    template <typename R, typename ...Ts>
    struct callable_vtable<R(Ts...)>
    {
        template <typename T>
        BOOST_FORCEINLINE static R _invoke(void** f, Ts&&... vs)
        {
            return util::invoke<R>(
                vtable::get<T>(f), std::forward<Ts>(vs)...);
        }
        R (*invoke)(void**, Ts&&...);

        template <typename T>
        BOOST_CONSTEXPR callable_vtable(construct_vtable<T>) BOOST_NOEXCEPT
          : invoke(&callable_vtable::template _invoke<T>)
        {}
    };
}}}

#endif
