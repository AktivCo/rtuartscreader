#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>

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
#define DEFINE_FUNCTION(DUMMY1, NAME, DUMMY2) .NAME = BOOST_PP_CAT(NAME, _impl),

IMPL_TYPE G_DEFAULT_IMPL = {
#include PIMPL_FUNCTIONS_DECLARATION_PATH
};

#undef DEFINE_FUNCTION

// Declare global current impl struct object pointer pointing to global default impl struct object
const IMPL_TYPE* G_IMPL = &G_DEFAULT_IMPL;

// Implement public functions, calling impl functions from the global impl struct object
#define EXPAND_ONE_ARG(Z, N, ARGS)  \
    BOOST_PP_TUPLE_ELEM(Z, N, ARGS) \
    arg##N
#define EXPAND_ARGS(ARGS) BOOST_PP_ENUM(BOOST_PP_TUPLE_SIZE(ARGS), EXPAND_ONE_ARG, ARGS)

#define EXPAND_PARAMS(ARGS) BOOST_PP_ENUM_PARAMS(BOOST_PP_TUPLE_SIZE(ARGS), arg)

#define DEFINE_FUNCTION(R, NAME, ARGS)            \
    R NAME(EXPAND_ARGS(ARGS)) {                   \
        return G_IMPL->NAME(EXPAND_PARAMS(ARGS)); \
    }

#include PIMPL_FUNCTIONS_DECLARATION_PATH

#undef EXPAND_ONE_ARG
#undef EXPAND_ARGS
#undef EXPAND_PARAMS
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