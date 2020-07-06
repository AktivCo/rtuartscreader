// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comma_if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>


#ifndef PIMPL_NAME_PREFIX
#error "Need PIMPL_NAME_PREFIX defined"
#endif // PIMPL_NAME_PREFIX

#ifndef PIMPL_FUNCTIONS_DECLARATION_PATH
#error "Need PIMPL_FUNCTIONS_DECLARATION_PATH defined"
#endif // PIMPL_FUNCTIONS_DECLARATION_PATH

#define IMPL_TYPE BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_t)
#define G_DEFAULT_IMPL BOOST_PP_CAT(BOOST_PP_CAT(g_, PIMPL_NAME_PREFIX), _default_impl)
#define G_IMPL BOOST_PP_CAT(BOOST_PP_CAT(g_, PIMPL_NAME_PREFIX), _impl)


// Declare global default impl struct object filled with function_name_impl values
#define DEFINE_FUNCTION(DUMMY1, NAME, ...) .NAME = BOOST_PP_CAT(NAME, _impl),

IMPL_TYPE G_DEFAULT_IMPL = {
#include PIMPL_FUNCTIONS_DECLARATION_PATH
};

#undef DEFINE_FUNCTION

// Declare global current impl struct object pointer pointing to global default impl struct object
const IMPL_TYPE* G_IMPL = &G_DEFAULT_IMPL;

// Implement public functions, calling impl functions from the global impl struct object
#define EXPAND_ONE_ARG(Z, N, ARGS)              \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(N, 2)) \
    BOOST_PP_TUPLE_ELEM(Z, N, ARGS)             \
    arg##N

#define EXPAND_ONE_PARAM(Z, N, ARGS)            \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(N, 2)) \
    arg##N

#define EXPAND_ARGS(...) BOOST_PP_REPEAT_FROM_TO(2, BOOST_PP_TUPLE_SIZE(BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)), \
                                                 EXPAND_ONE_ARG, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))

#define EXPAND_PARAMS(...) BOOST_PP_REPEAT_FROM_TO(2, BOOST_PP_TUPLE_SIZE(BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)), \
                                                   EXPAND_ONE_PARAM, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))

#define R(...) BOOST_PP_TUPLE_ELEM(0, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))

#define NAME(...) BOOST_PP_TUPLE_ELEM(1, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))

#define DEFINE_FUNCTION(...)                                          \
    R(__VA_ARGS__)                                                    \
    NAME(__VA_ARGS__)(EXPAND_ARGS(__VA_ARGS__)) {                     \
        return G_IMPL->NAME(__VA_ARGS__)(EXPAND_PARAMS(__VA_ARGS__)); \
    }

#include PIMPL_FUNCTIONS_DECLARATION_PATH

#undef EXPAND_ONE_ARG
#undef EXPAND_ONE_PARAM
#undef EXPAND_ARGS
#undef EXPAND_PARAMS
#undef R
#undef NAME
#undef DEFINE_FUNCTION

// Implement PIMPL_NAME_PREFIX_set function
void BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_set)(const IMPL_TYPE* impl) {
    G_IMPL = impl;
}

// Implement PIMPL_NAME_PREFIX_impl_reset function
void BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_reset)() {
    G_IMPL = &G_DEFAULT_IMPL;
}

#undef IMPL_TYPE
#undef G_DEFAULT_IMPL
#undef G_IMPL