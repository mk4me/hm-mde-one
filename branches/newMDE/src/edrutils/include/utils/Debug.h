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

#   ifdef __WIN32__
#       include <crtdbg.h>

#       define UTILS_ASSERT(condition, ...) UTILS_MULTISTATEMENT_BEGIN                      \
if (!(condition)) {                                                                         \
    if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #condition "\n" __VA_ARGS__)){ \
        _CrtDbgBreak();                                                                     \
    }                                                                                       \
} UTILS_MULTISTATEMENT_END

#       define UTILS_FAIL(...) UTILS_MULTISTATEMENT_BEGIN                                   \
if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "" __VA_ARGS__)) {            \
    _CrtDbgBreak();                                                                         \
}                                                                                           \
UTILS_MULTISTATEMENT_END

#   else // __WIN32__
#       include <cassert>
#       define UTILS_ASSERT(condition, ...)   assert(condition)
#       define UTILS_FAIL(...)                assert(false)
#   endif // __WIN32__

#else
#   define UTILS_ASSERT(...)
#   define UTILS_FAIL(...)
#endif

/**
 *	Definicja satycznej asercji.
 */
#ifdef UTILS_CXX0X
#   define UTILS_STATIC_ASSERT(cond, msg) static_assert((cond), msg)
#else
#   include <boost/static_assert.hpp>
//! UWAGA!!
//! Jeśli chcemy w ten sposób uzyskać statyczną asercję we wzorcu klasy
//! nie pozwalając na jej instancjonowanie z pewnych względów (np.
//! mechanizm ObjectWrapperów) umieszczając w jej wnętrzu
//! wywołanie typu: UTILS_STATIC_ASSERT((false), msg)
//! musimy uzależnić to wywołanie od przynajmniej jednego parametru wzorca
//! np.
//! template<class T> will_faill
//! {
//! 	UTILS_STATIC_ASSERT(sizeof(T) == 0, "Nie mozna utworzyc klasy wprost")
//! };
//! Odwołanie: http://www.boost.org/doc/libs/1_53_0/doc/html/boost_staticassert.html
#   define UTILS_STATIC_ASSERT(cond, msg) BOOST_STATIC_ASSERT_MSG((cond), msg)
#endif


#endif  // HEADER_GUARD__UTILS_DEBUG_H__
