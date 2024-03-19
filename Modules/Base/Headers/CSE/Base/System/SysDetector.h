// System Dectector
// This file lists OS that CSE is now supported or will be supported in the future.

#if !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#define CSE_OS_WIN32
#define CSE_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#define CSE_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#define CSE_OS_LINUX
#else
#error "CSpaceEngine has not been ported to this OS."
#endif

#if defined(CSE_OS_WIN32) || defined(CSE_OS_WIN64)
#define CSE_OS_WIN
#endif

#if defined(CSE_OS_WIN)
#undef CSE_OS_UNIX
#elif !defined(CSE_OS_UNIX)
#define CSE_OS_UNIX
#endif
