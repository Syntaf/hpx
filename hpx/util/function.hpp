//  Copyright (c) 2011 Thomas Heller
//  Copyright (c) 2013 Hartmut Kaiser
//  Copyright (c) 2014-2015 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_FUNCTION_HPP
#define HPX_UTIL_FUNCTION_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>
#include <hpx/traits/is_callable.hpp>
#include <hpx/traits/needs_automatic_registration.hpp>
#include <hpx/util/detail/basic_function.hpp>
#include <hpx/util/detail/pp_strip_parens.hpp>
#include <hpx/util/detail/vtable/function_vtable.hpp>
#include <hpx/util/detail/vtable/vtable.hpp>

#include <boost/preprocessor/cat.hpp>

#include <type_traits>
#include <utility>

namespace hpx { namespace util
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Sig, bool Serializable = true>
    class function;

    template <typename R, typename ...Ts, bool Serializable>
    class function<R(Ts...), Serializable>
      : public detail::basic_function<
            detail::function_vtable<R(Ts...)>
          , R(Ts...), Serializable
        >
    {
        typedef detail::function_vtable<R(Ts...)> vtable_ptr;
        typedef detail::basic_function<vtable_ptr, R(Ts...), Serializable> base_type;

    public:
        typedef typename base_type::result_type result_type;

        function() BOOST_NOEXCEPT
          : base_type()
        {}

        function(function const& other)
          : base_type()
        {
            detail::vtable::_destruct<
                detail::empty_function<R(Ts...)>
            >(&this->object);

            this->vptr = other.vptr;
            if (!this->vptr->empty)
            {
                this->vptr->copy(&this->object, &other.object);
            }
        }

        function(function&& other) BOOST_NOEXCEPT
          : base_type(static_cast<base_type&&>(other))
        {}

        template <typename F, typename FD = typename std::decay<F>::type,
            typename Enable = typename std::enable_if<
                !std::is_same<FD, function>::value
             && traits::is_callable<FD&(Ts...), R>::value
            >::type>
        function(F&& f)
          : base_type()
        {
            static_assert(
                std::is_constructible<FD, FD const&>::value,
                "F shall be CopyConstructible");
            assign(std::forward<F>(f));
        }

        function& operator=(function const& other)
        {
            if (this != &other)
            {
                reset();
                detail::vtable::_destruct<
                    detail::empty_function<R(Ts...)>
                >(&this->object);

                this->vptr = other.vptr;
                if (!this->vptr->empty)
                {
                    this->vptr->copy(&this->object, &other.object);
                }
            }
            return *this;
        }

        function& operator=(function&& other) BOOST_NOEXCEPT
        {
            base_type::operator=(static_cast<base_type&&>(other));
            return *this;
        }

        template <typename F, typename FD = typename std::decay<F>::type,
            typename Enable = typename std::enable_if<
                !std::is_same<FD, function>::value
             && traits::is_callable<FD&(Ts...), R>::value
            >::type>
        function& operator=(F&& f)
        {
            static_assert(
                std::is_constructible<FD, FD const&>::value,
                "F shall be CopyConstructible");
            assign(std::forward<F>(f));
            return *this;
        }

        using base_type::operator();
        using base_type::assign;
        using base_type::reset;
        using base_type::empty;
        using base_type::target_type;
        using base_type::target;
    };

    template <typename Sig, bool Serializable>
    static bool is_empty_function(
        function<Sig, Serializable> const& f) BOOST_NOEXCEPT
    {
        return f.empty();
    }

    ///////////////////////////////////////////////////////////////////////////
#   ifdef HPX_HAVE_CXX11_ALIAS_TEMPLATES

    template <typename Sig>
    using function_nonser = function<Sig, false>;

#   else

    template <typename T>
    class function_nonser;

    template <typename R, typename ...Ts>
    class function_nonser<R(Ts...)>
      : public function<R(Ts...), false>
    {
        typedef function<R(Ts...), false> base_type;

    public:
        function_nonser() BOOST_NOEXCEPT
          : base_type()
        {}

        function_nonser(function_nonser const& other)
          : base_type(static_cast<base_type const&>(other))
        {}

        function_nonser(function_nonser&& other) BOOST_NOEXCEPT
          : base_type(static_cast<base_type&&>(other))
        {}

        template <typename F, typename FD = typename std::decay<F>::type,
            typename Enable = typename std::enable_if<
                !std::is_same<FD, function_nonser>::value
             && traits::is_callable<FD&(Ts...), R>::value
            >::type>
        function_nonser(F&& f)
          : base_type(std::forward<F>(f))
        {}

        function_nonser& operator=(function_nonser const& other)
        {
            base_type::operator=(static_cast<base_type const&>(other));
            return *this;
        }

        function_nonser& operator=(function_nonser&& other) BOOST_NOEXCEPT
        {
            base_type::operator=(static_cast<base_type&&>(other));
            return *this;
        }

        template <typename F, typename FD = typename std::decay<F>::type,
            typename Enable = typename std::enable_if<
                !std::is_same<FD, function_nonser>::value
             && traits::is_callable<FD&(Ts...), R>::value
            >::type>
        function_nonser& operator=(F&& f)
        {
            base_type::operator=(std::forward<F>(f));
            return *this;
        }
    };

    template <typename Sig>
    static bool is_empty_function(
        function_nonser<Sig> const& f) BOOST_NOEXCEPT
    {
        return f.empty();
    }

#   endif /*HPX_HAVE_CXX11_ALIAS_TEMPLATES*/
}}

///////////////////////////////////////////////////////////////////////////////
#define HPX_CONTINUATION_REGISTER_FUNCTION_FACTORY(VTable, Name)              \
    static ::hpx::util::detail::function_registration<VTable> const           \
        BOOST_PP_CAT(Name, _function_factory_registration) =                  \
            ::hpx::util::detail::function_registration<VTable>();             \
/**/

#define HPX_DECLARE_GET_FUNCTION_NAME(VTable, Name)                           \
    namespace hpx { namespace util { namespace detail {                       \
        template<> HPX_ALWAYS_EXPORT                                          \
        char const* get_function_name<VTable>();                              \
    }}}                                                                       \
/**/

#define HPX_UTIL_REGISTER_FUNCTION_DECLARATION(Sig, Functor, Name)            \
    namespace hpx { namespace util { namespace detail {                       \
        typedef                                                               \
            serializable_function_registration<                               \
                function_vtable<Sig>                                          \
              , std::decay<HPX_UTIL_STRIP(Functor)>::type                     \
            >                                                                 \
            BOOST_PP_CAT(BOOST_PP_CAT(__,                                     \
                BOOST_PP_CAT(hpx_function_serialization_, Name)), _type);     \
    }}}                                                                       \
    HPX_DECLARE_GET_FUNCTION_NAME(                                            \
        BOOST_PP_CAT(BOOST_PP_CAT(hpx::util::detail::__,                      \
            BOOST_PP_CAT(hpx_function_serialization_, Name)), _type)          \
        , Name)                                                               \
    namespace hpx { namespace traits {                                        \
        template <>                                                           \
        struct needs_automatic_registration<                                  \
            BOOST_PP_CAT(BOOST_PP_CAT(util::detail::__,                       \
                BOOST_PP_CAT(hpx_function_serialization_, Name)), _type)>     \
          : boost::mpl::false_                                                \
        {};                                                                   \
    }}                                                                        \
/**/

#define HPX_DEFINE_GET_FUNCTION_NAME(VTable, Name)                            \
    namespace hpx { namespace util { namespace detail {                       \
        template<> HPX_ALWAYS_EXPORT                                          \
        char const* get_function_name<VTable>()                               \
        {                                                                     \
            return BOOST_PP_STRINGIZE(Name);                                  \
        }                                                                     \
    }}}                                                                       \
/**/

#define HPX_UTIL_REGISTER_FUNCTION(Sig, Functor, Name)                        \
    HPX_CONTINUATION_REGISTER_FUNCTION_FACTORY(                               \
        BOOST_PP_CAT(BOOST_PP_CAT(hpx::util::detail::__,                      \
            BOOST_PP_CAT(hpx_function_serialization_, Name)), _type)          \
      , Name)                                                                 \
    HPX_DEFINE_GET_FUNCTION_NAME(                                             \
        BOOST_PP_CAT(BOOST_PP_CAT(hpx::util::detail::__,                      \
            BOOST_PP_CAT(hpx_function_serialization_, Name)), _type)          \
      , Name)                                                                 \
/**/

#endif
