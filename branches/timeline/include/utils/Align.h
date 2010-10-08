/********************************************************************
	created:  2010/07/15
	created:  15:7:2010   8:08
	filename: Align.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__ALIGN_H__
#define __HEADER_GUARD__ALIGN_H__

#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Wyr�wnuje adres do zadanego podzielnika.
/**
 *	Wyr�wnuje adres do zadanego podzielnika. Podzielnik powinien by� pot�g�
 *  dw�jki.
 *  \param addr Adres.
 *  \param alignTo Podzielnik.
 */
#define UTILS_ALIGN(addr, alignTo) ( (addr&(alignTo-1)) ? ((addr&(~(alignTo-1)))+alignTo) : addr )

//! Wyr�wnuje adres do zadanego podzielnika.
//! \param addr Adres
//! \param alignTo Podzielnik
template <class T, class U>
inline T align(T addr, U alignTo)
{
    return UTILS_ALIGN(addr, alignTo);
}

//! Domy�lny align danych.
/** Sta�a value w tej odmianie oraz w specjalizacjach m�wi do jakiego adresu
 *  typ o zadanym rozmiarze ma by� wyr�wnany. 
 */
template <int Size> struct Align { enum { value = 4 }; };
//! Align dla rozmiaru 1.
template <> struct Align<1> { enum { value = 1}; };
//! Align dla rozmiaru 2.
template <> struct Align<2> { enum { value = 2}; };
//! Align dla rozmiaru 3.
template <> struct Align<3> { enum { value = 4}; };

//! Pomocniczy typ, zamiast rozmiaru przyjmuje typ
template <class T> struct AlignType { 
    enum { value = Align<sizeof(T)>::value }; 
};

//! Typ wyr�wnuj�cy adres przy zadanym wyr�wnaniu
template <int Addr, int AlignBytes> struct AlignTo {
    // Liczba bajt�w musi byc potega dw�jki!
    UTILS_STATIC_ASSERT((AlignBytes & (AlignBytes-1)) == 0 );
    //! Warunek: je�eli ostatnie bajty nie s� wyzerowane to je zeruj� i dodaj� bajty alignu;
    //! w przeciwnym wypadku warto�� jest ju� wyalignowana.
    enum { value = UTILS_ALIGN(Addr, AlignBytes) };
};

//! Typ wyr�wnuj�cy adres dla zadanego typu.
template <int Addr, class T> struct AlignToType {
    enum {
        value = AlignTo< Addr, AlignType<T>::value >::value
    };
};

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__ALIGN_H__