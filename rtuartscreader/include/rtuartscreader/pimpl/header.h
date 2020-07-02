#include <boost/preprocessor/cat.hpp>

#ifndef PIMPL_NAME_PREFIX
#error "Need PIMPL_NAME_PREFIX defined"
#endif // PIMPL_NAME_PREFIX

#ifndef PIMPL_FUNCTIONS_DECLARATION_PATH
#error "Need PIMPL_FUNCTIONS_DECLARATION_PATH defined"
#endif // PIMPL_FUNCTIONS_DECLARATION_PATH

#ifdef __cplusplus
extern "C" {
#endif

// Declare functions defined in PIMPL_FUNCTIONS_DECLARATION_PATH
#define DEFINE_FUNCTION(R, NAME, ...) R NAME(__VA_ARGS__);
#include PIMPL_FUNCTIONS_DECLARATION_PATH
#undef DEFINE_FUNCTION

// Declare pointer to functions defined in PIMPL_FUNCTIONS_DECLARATION_PATH
#define DEFINE_FUNCTION(R, NAME, ...) typedef R (*NAME##_fn)(__VA_ARGS__);
#include PIMPL_FUNCTIONS_DECLARATION_PATH
#undef DEFINE_FUNCTION

// Declare struct of pointers to functions defined in PIMPL_FUNCTIONS_DECLARATION_PATH
typedef struct BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl) {
#define DEFINE_FUNCTION(DUMMY1, NAME, ...) NAME##_fn NAME;
#include PIMPL_FUNCTIONS_DECLARATION_PATH
#undef DEFINE_FUNCTION
} BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_t);

// Declare function PIMPL_NAME_PREFIX_impl_set to set new value to the struct of pointers
void BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_set)(const BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_t) * impl);
// Declare function PIMPL_NAME_PREFIX_reset to set the global struct of pointers to default
void BOOST_PP_CAT(PIMPL_NAME_PREFIX, _impl_reset)();

#ifdef __cplusplus
}
#endif

#undef PIMPL_NAME_PREFIX
#undef PIMPL_FUNCTIONS_DECLARATION_PATH
