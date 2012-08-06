/********************************************************************
	created:    2010/05/14
	created:    14:5:2010     10:13
	filename: Utils.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_H__
#define HEADER_GUARD__UTILS_H__

#include <utils/Config.h>
#include <boost/lexical_cast.hpp>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

// Sugerowany pocz¹tek makra sk³adaj¹cego siê z wielu instrukcji
#define UTILS_MULTISTATEMENT_BEGIN  do {

// Sugerowany koniec makra sk³adaj¹cego siê z wielu instrukcji
#ifdef _MSC_VER
#   define UTILS_MULTISTATEMENT_END     \
    __pragma(warning(push))             \
    __pragma(warning(disable: 4127))    \
    } while(0) /* brak ; na koncu! */   \
    __pragma(warning(pop))
#else
#   define UTILS_MULTISTATEMENT_END    } while(0) /* brak ; na koncu! */
#endif

//------------------------------------------------------------------------------

//
template <class T>
inline T clamp(const T& v, const T& min, const T& max)
{
    return ( v < min ) ? min : ((v > max) ? max : v);
}

//------------------------------------------------------------------------------

//! Zeruje zadany obiekt.
//! \param object Obiekt do wyzerowania.
template <class T>
inline void zero(T & object)
{
    memset(&object, 0, sizeof(T));
}

//------------------------------------------------------------------------------

//! Zwalnia zadany wskaŸnik oraz zeruje jego wartoœæ.
#define UTIL_DELETEPTR(ptr) UTILS_MULTISTATEMENT_BEGIN \
    delete ptr; \
    ptr = nullptr; \
    UTILS_MULTISTATEMENT_END

//! Zwalnia zadany wskaŸnik oraz zeruje jego wartoœæ.
//! \param ptr WskaŸnik do zwolnienia.
template <class T>
inline void deletePtr(T *& ptr)
{
    delete ptr;
    ptr = nullptr;
}

//------------------------------------------------------------------------------

//! Zwraca znak liczby. U¿ywane do uproszczenia mno¿enia.
//! \param value
//! \return 1 albo -1
template <class T>
inline T sign(T value)
{
    return value >= 0 ? T(1) : T(-1);
}

//------------------------------------------------------------------------------

//! Zwraca znak liczby. U¿ywane do uproszczenia mno¿enia.
//! \param value
//! \return 1 albo -1
template <class T, size_t Size>
inline size_t length(const T (&fixedArray)[Size])
{
  return Size;
}


//------------------------------------------------------------------------------

struct NullType {};

//------------------------------------------------------------------------------

//! Konwersja danego typu do ³añcucha.
//! \param source
template <class T>
inline std::string toString(const T& source)
{
    return boost::lexical_cast<std::string>(source);
}

//------------------------------------------------------------------------------

/**
 *	Definicja ostrze¿enia o przestarza³ej funkcjonalnoœci
 */
#ifdef __GNUC__
#   define UTILS_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#   define UTILS_DEPRECATED(func) __declspec(deprecated) func
#else
#   pragma basicMessage("WARNING: You need to implement DEPRECATED for this compiler")
#   define UTILS_DEPRECATED(func) func
#endif

//------------------------------------------------------------------------------

/**
 *	Makra zapamiêtuj¹ce/odtwarzaj¹ce stan warningów.
 */
#if defined(_MSC_VER)
#   define UTILS_PUSH_WARNINGS  __pragma(warning(push))
#   define UTILS_POP_WARNINGS   __pragma(warning(pop))
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING __pragma(warning(disable: 4251))
#elif defined(__GNUC__) && defined(UTILS_CXX0X)
#   define DO_PRAGMA(x) _Pragma (#x)
#   define UTILS_PUSH_WARNINGS  DO_PRAGMA(GCC diagnostic push)
#   define UTILS_POP_WARNINGS   DO_PRAGMA(GCC diagnostic pop)
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING
#else
#   pragma basicMessage("WARNING: You need to implement _Pragma for this compiler")
#   define UTILS_PUSH_WARNINGS
#   define UTILS_POP_WARNINGS
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING
#endif


//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__UTILS_H__
