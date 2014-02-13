/********************************************************************
    created:  2013/11/07
    created:  7:11:2013   18:47
    filename: Version.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm opisuj¹ca wersje API i jego cechy (funkcjonalnoœci)
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VERSION_H__
#define HEADER_GUARD_CORE__VERSION_H__

#include <corelib/Export.h>
#include <string>

#define CORE_API_MAJOR 0	//! G³ówna wersja API, zmiany tej wartoœci oznaczaj¹ brak kompatybilnoœci wstecz dla API i ABI
#define CORE_API_MINOR 2	//! Podrzêna wersja API, zmiany tej wartoœci powinny zachowywaæ kompatybilnoœæ wstecz (+ ABI)
#define CORE_API_PATCH 0	//! Wersja poprawek dla danej wersji i podwersji - zachowuje kompatybilnoœæ wstecz

namespace core
{
	//! Klasa obs³uguj¹ca opis wersji API aplikacji
	class CORELIB_EXPORT Version
	{
	public:

		//! \return G³ówna wersja aplikacji
		static const int major();
		//! \return Podrzêdna wersja aplikacji
		static const int minor();
		//! \return Wersja poprawki dla danej wersji i podwersji
		static const int patch();
		//! \return Wersja w formie litera³u
		static const std::string version();
		//! \param major G³ówna wersja
		//! \param minor Podrzêdna wersja
		//! \param patch Pacz wersji
		//! \return Wersja w formie litera³u
		static const std::string version(const int major, const int minor, const int patch);
		//! \param major G³ówna wersja
		//! \param minor Podrzêdna wersja
		//! \param patch Pacz wersji
		//! \return Czy aplikacja ma conajmniej wersjê opisan¹ parametrami
		static const bool isAtLeast(const int major, const int minor, const int patch);
		//! \param feature Funkcjonalnoœæ aplikacji
		//! \return Czy aplikacja wspiera zadan¹ funkcjonalnoœæ
		static const bool hasFeature(const std::string & feature);
	};
}

#endif	//	HEADER_GUARD_CORE__VERSION_H__
