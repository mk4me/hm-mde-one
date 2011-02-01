/********************************************************************
	created:  2010/08/05
	created:  5:8:2010   18:28
	filename: Debug.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_DEBUG_H__
#define __HEADER_GUARD__UTILS_DEBUG_H__

#include <utils/Config.h>

#if __WIN32__
#include <crtdbg.h>
#else
#include <cassert>
#endif

#include <sstream>
#include <boost/static_assert.hpp>
#include <utils/Utils.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

class Debug
{
private:
    //! Klasa bez instancji.
    Debug() {}

public:
    //!
    //! \param file
    //! \param line
    //! \param format
    //! \param
    static void print(const char * file, int line, const char * format, ...);
    //!
    //! \param file
    //! \param line
    //! \param format
    //! \param
    static void formatMessage(char * buffer, int bufferSize, const char * format, ...);
    //! Zaœlepka. Pomaga w budowaniu makr.
    //! \param buffer
    //! \param bufferSize
    static void formatMessage(char * buffer, int bufferSize);
};

/**
 *	Definicja funkcji wypisuj¹cej do konsoli w trybie debug.
 */
#if defined(_DEBUG) || defined(DEBUG)
# define UTILS_DEBUG_PRINT(format, ...) utils::Debug::print(__FILE__, __LINE__, format, __VA_ARGS__)
#else
# define UTILS_DEBUG_PRINT(...)
#endif

/**
 *	Definicja asercji.
 */
#if defined(_DEBUG) || defined(DEBUG)
#ifdef __WIN32__

#define UTILS_ASSERT(contition, ...) UTILS_MULTISTATEMENT_BEGIN                             \
if (!(contition)) {                                                                         \
    char __assert_buffer[256];                                                              \
    utils::Debug::formatMessage(__assert_buffer, sizeof(__assert_buffer)-1, __VA_ARGS__);   \
    if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, __assert_buffer)) {       \
        _CrtDbgBreak();                                                                     \
    }                                                                                       \
} UTILS_MULTISTATEMENT_END

#define UTILS_FAIL(...) UTILS_MULTISTATEMENT_BEGIN                                          \
char __assert_buffer[256];                                                                  \
utils::Debug::formatMessage(__assert_buffer, sizeof(__assert_buffer)-1, __VA_ARGS__);       \
if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, __assert_buffer)) {           \
    _CrtDbgBreak();                                                                         \
}                                                                                           \
UTILS_MULTISTATEMENT_END

#else // __WIN32__

#define UTILS_ASSERT(condition, msg, ...)   assert(condition)
#define UTILS_FAIL(msg, ...)                assert(false)

#endif // __WIN32__
#else
# define UTILS_ASSERT(...)
#endif

/**
 *	Definicja satycznej asercji.
 */
#define UTILS_STATIC_ASSERT(condition, ...) BOOST_STATIC_ASSERT(condition)

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__UTILS_DEBUG_H__
