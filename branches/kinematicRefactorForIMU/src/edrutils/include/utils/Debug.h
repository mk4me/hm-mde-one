/********************************************************************
	created:  2010/08/05
	created:  5:8:2010   18:28
	filename: Debug.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_DEBUG_H__
#define HEADER_GUARD__UTILS_DEBUG_H__

#include <utils/Config.h>
#include <utils/Macros.h>
#include <utils/Utils.h>
#include <sstream>
#include <boost/assert.hpp>


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
    //! Zaślepka. Pomaga w budowaniu makr.
    //! \param buffer
    //! \param bufferSize
    static void formatMessage(char * buffer, int bufferSize);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

/**
 *	Definicja funkcji wypisującej do konsoli w trybie debug.
 */
#ifdef UTILS_DEBUG
#   define UTILS_DEBUG_PRINT(format, ...) utils::Debug::print(__FILE__, __LINE__, format, __VA_ARGS__)
#else
#   define UTILS_DEBUG_PRINT(...)
#endif

/**
 *	Definicja asercji.
 */
#ifdef UTILS_DEBUG

#define UTILS_ASSERT(condition, ...) BOOST_ASSERT_MSG(#condition, "" ## COMA_AWARE_STRINGIZE(__VA_ARGS__))
#define UTILS_FAIL(...) UTILS_ASSERT(false, "" ## COMA_AWARE_STRINGIZE(__VA_ARGS__))

#else
#   define UTILS_ASSERT(...)
#   define UTILS_FAIL(...)
#endif

#endif  // HEADER_GUARD__UTILS_DEBUG_H__
