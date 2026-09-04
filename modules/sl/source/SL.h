/**
 * \file      sl/SL.h
 * \brief     Platform detection, basic type definitions and utility macros
 * \details   SL.h is included (directly or indirectly) by every SL class and
 *            defines the vocabulary the rest of the framework is written in.
 *            It provides four things:
 *
 *            - **Platform detection.** Exactly one of SL_OS_MACOS,
 *              SL_OS_MACIOS, SL_OS_WINDOWS, SL_OS_ANDROID, SL_OS_LINUX or
 *              SL_EMSCRIPTEN is defined from the compiler's own predefined
 *              macros; an unknown platform is a hard #error. Depending on the
 *              platform, SL_GLES, SL_GLES3 and SL_USE_DISCARD_STEREOMODES are
 *              derived from it, and the platform's system headers are included.
 *            - **Compiler detection.** One of SL_COMP_MSVC, SL_COMP_BORLANDC,
 *              SL_COMP_INTEL or SL_COMP_GNUC, together with the portable
 *              spellings SL_STDCALL and SL_DEPRECATED.
 *            - **Type aliases.** SLchar, SLint, SLfloat and friends mirror the
 *              OpenGL types so that vertex data can be handed to the GL without
 *              casting. Their vector forms are prefixed SLV (1D) and SLVV (2D).
 *            - **Macros.** Bit manipulation (SL_GETBIT, SL_SETBIT, SL_DELBIT,
 *              SL_TOGBIT) and logging and error reporting (SL_LOG,
 *              SL_LOG_DEBUG, SL_EXIT_MSG, SL_WARN_MSG), the latter forwarding
 *              to the Utils namespace.
 *
 *            The macros are not listed individually in this documentation:
 *            Doxygen is configured with SKIP_FUNCTION_MACROS, and the platform
 *            defines live inside conditional branches that the documentation
 *            build does not take. Read this header itself for those.
 * \date      October 2015
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#ifndef SL_H
#define SL_H

//////////////////////////////////////////////////////////
// Preprocessor constant definitions used in the SLProject
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/* Determine one of the following operating systems:
SL_OS_MACOS    :Apple Mac OSX
SL_OS_MACIOS   :Apple iOS
SL_OS_WINDOWS  :Microsoft desktop Windows XP, 7, 8, ...
SL_OS_ANDROID  :Goggle Android
SL_OS_LINUX    :Linux desktop OS

With the OS definition the following constants are defined:
SL_GLES : Any version of OpenGL ES
SL_GLES3: Supports only OpenGL ES3
SL_USE_DISCARD_STEREOMODES: The discard stereo modes can be used (SLCamera)
*/

#ifdef __APPLE__
#    include <TargetConditionals.h>
#    if TARGET_OS_IOS
#        define SL_OS_MACIOS
#        define SL_GLES
#        define SL_GLES3
#    else
#        define SL_OS_MACOS
#        if defined(_DEBUG)

#        endif
#    endif
#elif defined(ANDROID) || defined(ANDROID_NDK)
#    define SL_OS_ANDROID
#    define SL_GLES
#    define SL_GLES3
#elif defined(_WIN32)
#    define SL_OS_WINDOWS
#    define SL_USE_DISCARD_STEREOMODES
#    ifdef _DEBUG
#        define _GLDEBUG
#    endif
#    define STDCALL __stdcall
#elif defined(linux) || defined(__linux) || defined(__linux__)
#    define SL_OS_LINUX
#    define SL_USE_DISCARD_STEREOMODES
#    ifdef _DEBUG
#    endif
#elif defined(__EMSCRIPTEN__)
#    define SL_EMSCRIPTEN
#    define SL_USE_DISCARD_STEREOMODES
#    define SL_GLES
#    define SL_GLES3
#else
#    error "SL has not been ported to this OS"
#endif

//-----------------------------------------------------------------------------
/* One of the following constants defines the GUI system. It is not set here
but as a compile definition by CMake; see modules/sl/CMakeLists.txt.

SL_GUI_QT   :Qt on OSX, Windows, Linux or Android
SL_GUI_OBJC :ObjectiveC on iOS
SL_GUI_GLFW :GLFW on OSX, Windows or Linux (the only one CMake sets today)
SL_GUI_JAVA :Java on Android (queried by SLGLOVRWorkaround.h)
*/

//-----------------------------------------------------------------------------
#if defined(SL_OS_MACIOS)
#    include <chrono>
#    include <functional>
#    include <random>
#    include <sys/time.h>
#    include <thread>
#    include <CoreServices/CoreServices.h> // for system info
#    include <zlib.h>
#elif defined(SL_OS_MACOS)
#    include <chrono>
#    include <functional>
#    include <random>
#    include <ctime>
#    include <thread>
//#    include <CoreServices/CoreServices.h> // for system info
#    include <sys/sysctl.h> // for system info
#elif defined(SL_OS_ANDROID)
#    include <sys/time.h>
#    include <sys/system_properties.h>
#    include <chrono>
#    include <functional>
#    include <random>
#    include <sstream>
#    include <thread>
#elif defined(SL_OS_WINDOWS)
#    include <chrono>
#    include <functional>
#    include <random>
#    include <thread>
#    include <windows.h>
#elif defined(SL_OS_LINUX)
#    include <chrono>
#    include <functional>
#    include <random>
#    include <sstream>
#    include <sys/time.h>
#    include <thread>
#elif defined(SL_EMSCRIPTEN)
#    include <random>
#else
#    error "SL has not been ported to this OS"
#endif

#include <Utils.h>

//-----------------------------------------------------------------------------
using std::string;
using std::vector;

//-----------------------------------------------------------------------------
// Determine compiler
#if defined(__GNUC__)
#    undef _MSC_VER
#endif

#if defined(_MSC_VER)
#    define SL_COMP_MSVC
#    define SL_STDCALL __stdcall
#    define SL_DEPRECATED __declspec(deprecated)
#    define _CRT_SECURE_NO_DEPRECATE // visual 8 secure crt warning
#elif defined(__BORLANDC__)
#    define SL_COMP_BORLANDC
#    define SL_STDCALL Stdcall
#    define SL_DEPRECATED // @todo Does this compiler support deprecated attributes
#elif defined(__INTEL_COMPILER)
#    define SL_COMP_INTEL
#    define SL_STDCALL Stdcall
#    define SL_DEPRECATED // @todo does this compiler support deprecated attributes
#elif defined(__GNUC__)
#    define SL_COMP_GNUC
#    define SL_STDCALL
#    define SL_DEPRECATED __attribute__((deprecated))
#else
#    error "SL has not been ported to this compiler"
#endif

//-----------------------------------------------------------------------------
//! Redefinition of standard types for platform independence
/*! The SL types mirror the OpenGL types of the same name so that vertex
attribute data can be passed to the GL without casting. */
typedef string SLstring; //!< analog to std::string
#ifndef SL_OS_ANDROID
typedef std::wstring SLwstring; //!< analog to std::wstring, not available on Android
#endif
typedef char           SLchar;     //!< analog to GLchar (char is signed [-128 ... 127]!)
typedef unsigned char  SLuchar;    //!< analog to GLuchar
typedef signed long    SLlong;     //!< analog to GLlong
typedef unsigned long  SLulong;    //!< analog to GLulong
typedef signed char    SLbyte;     //!< analog to GLbyte
typedef unsigned char  SLubyte;    //!< analog to GLubyte
typedef short          SLshort;    //!< analog to GLshort
typedef unsigned short SLushort;   //!< analog to GLushort
typedef int            SLint;      //!< analog to GLint
typedef unsigned int   SLuint;     //!< analog to GLuint
typedef int            SLsizei;    //!< analog to GLsizei
typedef float          SLfloat;    //!< analog to GLfloat
typedef double         SLdouble;   //!< analog to GLdouble
typedef bool           SLbool;     //!< analog to GLbool
typedef unsigned int   SLenum;     //!< analog to GLenum
typedef unsigned int   SLbitfield; //!< analog to GLbitfield

// Fixed width integer types for data whose size must not vary by platform
typedef int8_t   SLint8;   //!< 8 bit signed integer
typedef uint8_t  SLuint8;  //!< 8 bit unsigned integer
typedef int16_t  SLint16;  //!< 16 bit signed integer
typedef uint16_t SLuint16; //!< 16 bit unsigned integer
typedef int32_t  SLint32;  //!< 32 bit signed integer
typedef uint32_t SLuint32; //!< 32 bit unsigned integer
typedef int64_t  SLint64;  //!< 64 bit signed integer
typedef uint64_t SLuint64; //!< 64 bit unsigned integer

//! All 1D vectors begin with SLV*
typedef vector<SLbool>   SLVbool;
typedef vector<SLbyte>   SLVbyte;
typedef vector<SLubyte>  SLVubyte;
typedef vector<SLchar>   SLVchar;
typedef vector<SLuchar>  SLVuchar;
typedef vector<SLshort>  SLVshort;
typedef vector<SLushort> SLVushort;
typedef vector<SLint>    SLVint;
typedef vector<SLuint>   SLVuint;
typedef vector<SLlong>   SLVlong;
typedef vector<SLulong>  SLVulong;
typedef vector<SLfloat>  SLVfloat;
typedef vector<SLstring> SLVstring;
typedef vector<size_t>   SLVsize_t;

//! All 2D vectors begin with SLVV*
typedef vector<vector<SLfloat>>  SLVVfloat;
typedef vector<vector<SLuchar>>  SLVVuchar;
typedef vector<vector<SLushort>> SLVVushort;
typedef vector<vector<SLuint>>   SLVVuint;
typedef vector<vector<SLchar>>   SLVVchar;
typedef vector<vector<SLshort>>  SLVVshort;
typedef vector<vector<SLint>>    SLVVint;

//-----------------------------------------------------------------------------
//! Returns the memory in bytes reserved by a vector
/*! Note that this reports the *capacity*, not the size: it is the memory the
vector currently occupies, which is what the scene statistics report. */
template<class T>
inline SLuint
SL_sizeOfVector(const T& vector)
{
    return (SLint)(vector.capacity() * sizeof(typename T::value_type));
}
//-----------------------------------------------------------------------------
//! Bit manipulation macros for ones that forget it always
/*! VAR is the bit field, VAL the bit mask to test or modify. The arguments are
not parenthesised, so pass plain variables rather than expressions.
SL_TOGBIT expands to a bare if/else and must not be used as the body of an
unbraced if, or it will bind to the wrong else. */
#define SL_GETBIT(VAR, VAL) VAR& VAL
#define SL_SETBIT(VAR, VAL) VAR |= VAL
#define SL_DELBIT(VAR, VAL) VAR &= ~VAL
#define SL_TOGBIT(VAR, VAL) \
    if ((VAR) & (VAL)) \
        (VAR) &= ~(VAL); \
    else \
        (VAR) |= VAL
//-----------------------------------------------------------------------------
//! Silences the unused-parameter warning in XCode
/*! \warning Defective and unused: the parameter is named r while the body
casts x, so any use fails to compile unless an unrelated x is in scope. It has
no callers anywhere in the project. Fix the argument name or delete it before
using it. */
#define UNUSED_PARAMETER(r) ((void)(x))

//-----------------------------------------------------------------------------
//! Some debugging and error handling macros
/*! All four tag their output with "SLProject" and forward to the Utils
namespace. SL_LOG_DEBUG compiles to nothing outside debug builds. SL_EXIT_MSG
terminates the application; SL_WARN_MSG only reports. Both pass __LINE__ and
__FILE__ so the origin appears in the message. */
#define SL_LOG(...) Utils::log("SLProject", __VA_ARGS__)
#if _DEBUG
#    define SL_LOG_DEBUG(...) Utils::log("SLProject", __VA_ARGS__)
#else
#    define SL_LOG_DEBUG(...) \
        { \
        }
#endif

#define SL_EXIT_MSG(message) Utils::exitMsg("SLProject", (message), __LINE__, __FILE__)
#define SL_WARN_MSG(message) Utils::warnMsg("SLProject", (message), __LINE__, __FILE__)
//-----------------------------------------------------------------------------
#endif
