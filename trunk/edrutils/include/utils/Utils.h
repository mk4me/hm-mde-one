/********************************************************************
	created:    2010/05/14
	created:    14:5:2010     10:13
	filename: Utils.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_H__
#define __HEADER_GUARD__UTILS_H__

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
    ptr = NULL; \
    UTILS_MULTISTATEMENT_END

//! Zwalnia zadany wskaŸnik oraz zeruje jego wartoœæ.
//! \param ptr WskaŸnik do zwolnienia.
template <class T>
inline void deletePtr(T *& ptr)
{
    delete ptr;
    ptr = NULL;
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
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#   define UTILS_DEPRECATED(func) func
#endif



//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // __HEADER_GUARD__UTILS_H__
