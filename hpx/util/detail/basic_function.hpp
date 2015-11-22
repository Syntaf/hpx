//  Copyright (c) 2011 Thomas Heller
//  Copyright (c) 2013 Hartmut Kaiser
//  Copyright (c) 2014 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_DETAIL_BASIC_FUNCTION_HPP
#define HPX_UTIL_DETAIL_BASIC_FUNCTION_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/serialization/access.hpp>
#include <hpx/runtime/serialization/serialize.hpp>
#include <hpx/traits/is_callable.hpp>
#include <hpx/util/detail/empty_function.hpp>
#include <hpx/util/detail/function_registration.hpp>
#include <hpx/util/detail/vtable/serializable_function_vtable.hpp>
#include <hpx/util/detail/vtable/vtable.hpp>
#include <hpx/util/move.hpp>
#include <hpx/util/safe_bool.hpp>

#include <boost/mpl/bool.hpp>

#include <type_traits>
#include <typeinfo>
#include <utility>

namespace hpx { namespace util { namespace detail
{
    template <typename VTablePtr, typename Sig>
    class function_base;

    ///////////////////////////////////////////////////////////////////////////
    template <typename F>
    static bool is_empty_function(F const&, std::false_type) BOOST_NOEXCEPT
    {
        return false;
    }

    template <typename F>
    static bool is_empty_function(F const& f, std::true_type) BOOST_NOEXCEPT
    {
        return f == 0;
    }

    template <typename F>
    static bool is_empty_function(F const& f) BOOST_NOEXCEPT
    {
        std::integral_constant<bool,
            std::is_pointer<F>::value
         || std::is_member_pointer<F>::value
        > is_pointer;
        return is_empty_function(f, is_pointer);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename VTablePtr, typename Sig, typename Function>
    struct is_compatible_function
    {
        typedef char(&no_type)[1];
        typedef char(&yes_type)[2];

        static no_type call(...);

        template <typename OtherVTablePtr>
        static typename std::conditional<
            std::is_convertible<OtherVTablePtr, VTablePtr>::value,
            yes_type, no_type
        >::type call(function_base<OtherVTablePtr, Sig>&&);

        static bool const value =
            sizeof(call(std::declval<Function>())) == sizeof(yes_type);

        typedef std::integral_constant<bool, value> type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename VTablePtr, typename R, typename ...Ts>
    class function_base<VTablePtr, R(Ts...)>
    {
        HPX_MOVABLE_BUT_NOT_COPYABLE(function_base);

        static VTablePtr const empty_table;

        template <typename OtherVTablePtr, typename OtherSig>
        friend class function_base;

    public:
        function_base() BOOST_NOEXCEPT
          : vptr(&empty_table)
          , object(0)
        {
            vtable::default_construct<empty_function<R(Ts...)> >(&object);
        }

        function_base(function_base&& other) BOOST_NOEXCEPT
          : vptr(other.vptr)
          , object(other.object) // move-construct
        {
            other.vptr = &empty_table;
            vtable::default_construct<empty_function<R(Ts...)> >(&other.object);
        }

        ~function_base()
        {
            vptr->delete_(&object);
        }

        function_base& operator=(function_base&& other) BOOST_NOEXCEPT
        {
            if (this != &other)
            {
                swap(other);
                other.reset();
            }
            return *this;
        }

        template <typename F>
        typename std::enable_if<
            !is_compatible_function<VTablePtr, R(Ts...), F>::value
        >::type assign(F&& f)
        {
            if (!is_empty_function(f))
            {
                typedef typename std::decay<F>::type target_type;

                VTablePtr const* f_vptr = get_table_ptr<target_type>();
                if (vptr == f_vptr)
                {
                    vtable::reconstruct<target_type>(&object, std::forward<F>(f));
                } else {
                    reset();
                    vtable::_destruct<empty_function<R(Ts...)> >(&object);

                    vptr = f_vptr;
                    vtable::construct<target_type>(&object, std::forward<F>(f));
                }
            } else {
                reset();
            }
        }

        template <typename F>
        typename std::enable_if<
            is_compatible_function<VTablePtr, R(Ts...), F>::value
        >::type assign(F&& other) // rvalues-only
        {
            reset();
            if (!other.empty())
            {
                vptr = other.vptr;
                object = other.object;

                other.vptr = &other.empty_table;
                vtable::default_construct<empty_function<R(Ts...)> >(&other.object);
            }
        }

        void reset() BOOST_NOEXCEPT
        {
            if (!vptr->empty)
            {
                vptr->delete_(&object);

                vptr = &empty_table;
                vtable::default_construct<empty_function<R(Ts...)> >(&object);
            }
        }

        void swap(function_base& f) BOOST_NOEXCEPT
        {
            std::swap(vptr, f.vptr);
            std::swap(object, f.object); // swap
        }

        bool empty() const BOOST_NOEXCEPT
        {
            return vptr->empty;
        }

#       ifdef HPX_HAVE_CXX11_EXPLICIT_CONVERSION_OPERATORS
        explicit operator bool() const BOOST_NOEXCEPT
        {
            return !empty();
        }
#       else
        operator typename util::safe_bool<function_base>
            ::result_type() const BOOST_NOEXCEPT
        {
            return util::safe_bool<function_base>()(!empty());
        }
#       endif

        std::type_info const& target_type() const BOOST_NOEXCEPT
        {
            return empty() ? typeid(void) : vptr->get_type();
        }

        template <typename T>
        T* target() BOOST_NOEXCEPT
        {
            BOOST_STATIC_ASSERT_MSG(
                (traits::is_callable<T(Ts...), R>::value)
              , "T shall be Callable with the function signature"
            );

            typedef typename std::decay<T>::type target_type;
            if (vptr->get_type() != typeid(target_type))
                return 0;

            return &vtable::get<target_type>(&object);
        }

        template <typename T>
        T const* target() const BOOST_NOEXCEPT
        {
            BOOST_STATIC_ASSERT_MSG(
                (traits::is_callable<T(Ts...), R>::value)
              , "T shall be Callable with the function signature"
            );

            typedef typename std::decay<T>::type target_type;
            if (vptr->get_type() != typeid(target_type))
                return 0;

            return &vtable::get<target_type>(&object);
        }

        BOOST_FORCEINLINE R operator()(Ts... vs) const
        {
            return vptr->invoke(&object, std::forward<Ts>(vs)...);
        }

    private:
        template <typename T>
        static VTablePtr const* get_table_ptr() BOOST_NOEXCEPT
        {
            return detail::get_vtable<VTablePtr, T>();
        }

    protected:
        VTablePtr const *vptr;
        mutable void *object;
    };

    template <typename VTablePtr, typename R, typename ...Ts>
    VTablePtr const function_base<VTablePtr, R(Ts...)>::empty_table =
        detail::construct_vtable<detail::empty_function<R(Ts...)> >();

    template <typename Sig, typename VTablePtr>
    static bool is_empty_function(function_base<VTablePtr, Sig> const& f) BOOST_NOEXCEPT
    {
        return f.empty();
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename VTablePtr, typename Sig, bool Serializable>
    class basic_function;

    template <typename VTablePtr, typename R, typename ...Ts>
    class basic_function<VTablePtr, R(Ts...), true>
      : public function_base<
            serializable_function_vtable<VTablePtr>
          , R(Ts...)
        >
    {
        HPX_MOVABLE_BUT_NOT_COPYABLE(basic_function);

        typedef serializable_function_vtable<VTablePtr> vtable_ptr;
        typedef function_base<vtable_ptr, R(Ts...)> base_type;

    public:
        typedef R result_type;

        basic_function() BOOST_NOEXCEPT
          : base_type()
        {}

        basic_function(basic_function&& other) BOOST_NOEXCEPT
          : base_type(static_cast<base_type&&>(other))
        {}

        basic_function& operator=(basic_function&& other) BOOST_NOEXCEPT
        {
            base_type::operator=(static_cast<base_type&&>(other));
            return *this;
        }

    private:
        friend class hpx::serialization::access;

        void load(serialization::input_archive& ar, const unsigned version)
        {
            this->reset();

            bool is_empty = false;
            ar >> is_empty;
            if (!is_empty)
            {
                std::string name;
                ar >> name;

                this->vptr = detail::get_table_ptr<vtable_ptr>(name);
                this->vptr->load_object(&this->object, ar, version);
            }
        }

        void save(serialization::output_archive& ar, const unsigned version) const
        {
            bool is_empty = this->empty();
            ar << is_empty;
            if (!is_empty)
            {
                std::string function_name = this->vptr->name;
                ar << function_name;

                this->vptr->save_object(&this->object, ar, version);
            }
        }

        HPX_SERIALIZATION_SPLIT_MEMBER()
    };

    template <typename VTablePtr, typename R, typename ...Ts>
    class basic_function<VTablePtr, R(Ts...), false>
      : public function_base<VTablePtr, R(Ts...)>
    {
        HPX_MOVABLE_BUT_NOT_COPYABLE(basic_function);

        typedef function_base<VTablePtr, R(Ts...)> base_type;

    public:
        typedef R result_type;

        basic_function() BOOST_NOEXCEPT
          : base_type()
        {}

        basic_function(basic_function&& other) BOOST_NOEXCEPT
          : base_type(static_cast<base_type&&>(other))
        {}

        basic_function& operator=(basic_function&& other) BOOST_NOEXCEPT
        {
            base_type::operator=(static_cast<base_type&&>(other));
            return *this;
        }
    };

    template <typename Sig, typename VTablePtr, bool Serializable>
    static bool is_empty_function(
        basic_function<VTablePtr, Sig, Serializable> const& f) BOOST_NOEXCEPT
    {
        return f.empty();
    }
}}}

#endif
