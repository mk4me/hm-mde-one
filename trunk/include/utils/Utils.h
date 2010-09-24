/********************************************************************
	created:    2010/05/14
	created:    14:5:2010     10:13
	filename: Utils.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_H__
#define __HEADER_GUARD__UTILS_H__

#include <cstring>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Sugerowany pocz�tek makra sk�adaj�cego si� z wielu instrukcji
#define UTILS_MULTISTATEMENT_BEGIN    do {
//! Sugerowany koniec makra sk�adaj�cego si� z wielu instrukcji
#define UTILS_MULTISTATEMENT_END        } while(0) /* brak ; na koncu! */

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

//! Zwalnia zadany wska�nik oraz zeruje jego warto��.
#define UTIL_DELETEPTR(ptr) UTILS_MULTISTATEMENT_BEGIN \
    delete ptr; \
    ptr = NULL; \
    UTILS_MULTISTATEMENT_END

//! Zwalnia zadany wska�nik oraz zeruje jego warto��.
//! \param ptr Wska�nik do zwolnienia.
template <class T>
inline void deletePtr(T *& ptr)
{
    delete ptr;
    ptr = NULL;
}

//------------------------------------------------------------------------------

//! Zwraca znak liczby. U�ywane do uproszczenia mno�enia.
//! \param value
//! \return 1 albo -1
template <class T>
inline T sign(T value)
{
    return value >= 0 ? T(1) : T(-1);
}

//------------------------------------------------------------------------------

struct NullType {};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // __HEADER_GUARD__UTILS_H__
