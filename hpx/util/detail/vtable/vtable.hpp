//  Copyright (c) 2011 Thomas Heller
//  Copyright (c) 2013 Hartmut Kaiser
//  Copyright (c) 2014-2015 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_DETAIL_VTABLE_VTABLE_HPP
#define HPX_UTIL_DETAIL_VTABLE_VTABLE_HPP

#include <hpx/config.hpp>

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace hpx { namespace util { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct construct_vtable {};

    template <typename VTable, typename T>
    static VTable const* get_vtable() BOOST_NOEXCEPT
    {
        static_assert(
            std::is_same<T, typename std::decay<T>::type>::value,
            "T shall have no cv-ref-qualifiers");

        static VTable const vtable = construct_vtable<T>();
        return &vtable;
    }

    ///////////////////////////////////////////////////////////////////////////
    struct vtable
    {
        template <typename T>
        BOOST_FORCEINLINE static T& get(void** v)
        {
            if (sizeof(T) <= sizeof(void*))
            {
                return *reinterpret_cast<T*>(v);
            } else {
                return **reinterpret_cast<T**>(v);
            }
        }

        template <typename T>
        BOOST_FORCEINLINE static T const& get(void* const* v)
        {
            if (sizeof(T) <= sizeof(void*))
            {
                return *reinterpret_cast<T const*>(v);
            } else {
                return **reinterpret_cast<T* const*>(v);
            }
        }

        template <typename T>
        BOOST_FORCEINLINE static void default_construct(void** v)
        {
            if (sizeof(T) <= sizeof(void*))
            {
                ::new (static_cast<void*>(v)) T;
            } else {
                *v = new T;
            }
        }

        template <typename T, typename Arg>
        BOOST_FORCEINLINE static void construct(void** v, Arg&& arg)
        {
            if (sizeof(T) <= sizeof(void*))
            {
                ::new (static_cast<void*>(v)) T(std::forward<Arg>(arg));
            } else {
                *v = new T(std::forward<Arg>(arg));
            }
        }

        template <typename T, typename Arg>
        BOOST_FORCEINLINE static void reconstruct(void** v, Arg&& arg)
        {
            _destruct<T>(v);
            construct<T, Arg>(v, std::forward<Arg>(arg));
        }

        template <typename T>
        BOOST_FORCEINLINE static std::type_info const& _get_type()
        {
            return typeid(T);
        }
        std::type_info const& (*get_type)();

        template <typename T>
        BOOST_FORCEINLINE static void _destruct(void** v)
        {
            get<T>(v).~T();
        }
        void (*destruct)(void**);

        template <typename T>
        BOOST_FORCEINLINE static void _delete(void** v)
        {
            if (sizeof(T) <= sizeof(void*))
            {
                _destruct<T>(v);
            } else {
                delete &get<T>(v);
            }
        }
        void (*delete_)(void**);

        template <typename T>
        BOOST_CONSTEXPR vtable(construct_vtable<T>) BOOST_NOEXCEPT
          : get_type(&vtable::template _get_type<T>)
          , destruct(&vtable::template _destruct<T>)
          , delete_(&vtable::template _delete<T>)
        {}
    };
}}}

#endif
