#ifndef INCLUDED_gsmbox_data_types_h
#define INCLUDED_gsmbox_data_types_h

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define GSMBOX_OS_DARWIN
#  define GSMBOX_OS_BSD4
#  ifdef __LP64__
#    define GSMBOX_OS_DARWIN64
#  else
#    define GSMBOX_OS_DARWIN32
#  endif
#elif defined(__CYGWIN__)
#  define GSMBOX_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#  define GSMBOX_OS_MSDOS
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define GSMBOX_OS_OS2EMX
#  else
#    define GSMBOX_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define GSMBOX_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define GSMBOX_OS_WINCE
#  else
#    define GSMBOX_OS_WIN32
#  endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define GSMBOX_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define GSMBOX_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define GSMBOX_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define GSMBOX_OS_ULTRIX
#elif defined(sinix)
#  define GSMBOX_OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#  define GSMBOX_OS_LINUX
#  if defined(__i386__)
#    define GSMBOX_OS_LINUX32
#  else
#    define GSMBOX_OS_LINUX64
#  endif
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define GSMBOX_OS_FREEBSD
#  define GSMBOX_OS_BSD4
#elif defined(__NetBSD__)
#  define GSMBOX_OS_NETBSD
#  define GSMBOX_OS_BSD4
#elif defined(__OpenBSD__)
#  define GSMBOX_OS_OPENBSD
#  define GSMBOX_OS_BSD4
#elif defined(__bsdi__)
#  define GSMBOX_OS_BSDI
#  define GSMBOX_OS_BSD4
#elif defined(__sgi)
#  define GSMBOX_OS_IRIX
#elif defined(__osf__)
#  define GSMBOX_OS_OSF
#elif defined(_AIX)
#  define GSMBOX_OS_AIX
#elif defined(__Lynx__)
#  define GSMBOX_OS_LYNX
#elif defined(__GNU__)
#  define GSMBOX_OS_HURD
#elif defined(__DGUX__)
#  define GSMBOX_OS_DGUX
#elif defined(__QNXNTO__)
#  define GSMBOX_OS_QNX6
#elif defined(__QNX__)
#  define GSMBOX_OS_QNX
#elif defined(_SEQUENT_)
#  define GSMBOX_OS_DYNIX
#elif defined(_SCO_DS) // SCO OpenServer 5 + GCC
#  define GSMBOX_OS_SCO
#elif defined(__USLC__) // all SCO platforms + UDK or OUDK
#  define GSMBOX_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) // Open UNIX 8 + GCC
#  define GSMBOX_OS_UNIXWARE
#elif defined(__INTEGRITY)
#  define GSMBOX_OS_INTEGRITY
#elif defined(__MAKEDEPEND__)
#else
#  error "OS not supported!"
#endif

#if defined(GSMBOX_OS_WIN32) || defined(GSMBOX_OS_WIN64) || defined(GSMBOX_OS_WINCE)
#  define GSMBOX_OS_WIN
#endif

#if defined(GSMBOX_OS_DARWIN)
#  define GSMBOX_OS_MAC  // GSMBOX_OS_MAC is mostly for compatibility, but also more clear
#  define GSMBOX_OS_MACX // GSMBOX_OS_MACX is only for compatibility.
#  if defined(GSMBOX_OS_DARWIN64)
#     define GSMBOX_OS_MAC64
#  elif defined(GSMBOX_OS_DARWIN32)
#     define GSMBOX_OS_MAC32
#  endif
#endif

#if defined(GSMBOX_OS_WIN)
  #if defined(_KERNEL_MODE)
  #define GSMBOX_KERNEL_MODE
  #endif
  #if defined(__cplusplus)
    #define GSMBOX_DllExport extern "C" __declspec( dllexport )
    #define GSMBOX_DllImport extern "C" __declspec( dllimport )
  #else
    #define GSMBOX_DllExport extern  __declspec( dllexport )
    #define GSMBOX_DllImport extern  __declspec( dllimport )
  #endif
  //#define GSMBOX_CALL __cdecl
  #define GSMBOX_CALL __stdcall
  #define GSMBOX_CALLBACK_CALL __stdcall
#else
  #if defined(__KERNEL__)
  #define GSMBOX_KERNEL_MODE
  #endif
  #if defined(__cplusplus)
    #define GSMBOX_DllExport extern "C" __attribute__ ((visibility("default")))
    #define GSMBOX_DllImport extern "C" __attribute__ ((visibility("default")))
  #else
    #define GSMBOX_DllExport extern __attribute__ ((visibility("default")))
    #define GSMBOX_DllImport extern __attribute__ ((visibility("default")))
  #endif
  #define GSMBOX_CALL
  #define GSMBOX_CALLBACK_CALL
#endif

#ifndef drv_bool
#ifndef __cplusplus
#ifdef GSMBOX_OS_WIN
  typedef unsigned char       bool;
#elif defined(GSMBOX_OS_LINUX) && !defined(GSMBOX_KERNEL_MODE)
  typedef unsigned char       bool;
#endif
#endif
#endif

#if defined(MSVC) || defined(_MSC_VER)
  typedef unsigned __int64  UINT64;
  typedef __int64        INT64;
  #ifndef __cplusplus
    #define true        1
    #define false       0
  #endif
  #ifdef GSMBOX_OS_WIN64
    #define UINT64ToPtr(p)    ((void*) p)
  #else
    #define UINT64ToPtr(p)    ((void*) (UINT32) p)
  #endif
#else

#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1200) //MSVC 6.0
  #pragma warning(disable: 4786)
#endif

#define GSMBOX_API GSMBOX_DllExport

#endif // INCLUDED_gsmbox_data_types_h
