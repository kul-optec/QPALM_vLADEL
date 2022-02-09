#pragma once

#ifdef WIN32
#    ifdef __GNUC__
#        ifdef QPALM_CXX_EXPORTS
#            define QPALM_CXX_EXPORT __attribute__((dllexport))
#        elif QPALM_CXX_IMPORTS
#            define QPALM_CXX_EXPORT __attribute__((dllimport))
#        else
#            define QPALM_CXX_EXPORT
#        endif
#    else /* __GNUC__ */
#        ifdef QPALM_CXX_EXPORTS
#            define QPALM_CXX_EXPORT __declspec(dllexport)
#        elif QPALM_CXX_IMPORTS
#            define QPALM_CXX_EXPORT __declspec(dllimport)
#        else
#            define QPALM_CXX_EXPORT
#        endif
#    endif /* __GNUC__ */
#else /* WIN32 */
#    define QPALM_CXX_EXPORT __attribute__((visibility("default")))
#endif /* WIN32 */