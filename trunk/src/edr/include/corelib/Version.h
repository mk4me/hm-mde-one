/********************************************************************
    created:  2013/11/07
    created:  7:11:2013   18:47
    filename: Version.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm opisuj�ca wersje API i jego cechy (funkcjonalno�ci)
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VERSION_H__
#define HEADER_GUARD_CORE__VERSION_H__

#include <corelib/Export.h>
#include <string>

#define CORE_API_MAJOR 0	//! G��wna wersja API, zmiany tej warto�ci oznaczaj� brak kompatybilno�ci wstecz dla API i ABI
#define CORE_API_MINOR 2	//! Podrz�na wersja API, zmiany tej warto�ci powinny zachowywa� kompatybilno�� wstecz (+ ABI)
#define CORE_API_PATCH 0	//! Wersja poprawek dla danej wersji i podwersji - zachowuje kompatybilno�� wstecz

namespace core
{
	//! Klasa obs�uguj�ca opis wersji API aplikacji
	class CORELIB_EXPORT Version
	{
	public:

		//! \return G��wna wersja aplikacji
		static const int major();
		//! \return Podrz�dna wersja aplikacji
		static const int minor();
		//! \return Wersja poprawki dla danej wersji i podwersji
		static const int patch();
		//! \return Wersja w formie litera�u
		static const std::string version();
		//! \param major G��wna wersja
		//! \param minor Podrz�dna wersja
		//! \param patch Pacz wersji
		//! \return Wersja w formie litera�u
		static const std::string version(const int major, const int minor, const int patch);
		//! \param major G��wna wersja
		//! \param minor Podrz�dna wersja
		//! \param patch Pacz wersji
		//! \return Czy aplikacja ma conajmniej wersj� opisan� parametrami
		static const bool isAtLeast(const int major, const int minor, const int patch);
		//! \param feature Funkcjonalno�� aplikacji
		//! \return Czy aplikacja wspiera zadan� funkcjonalno��
		static const bool hasFeature(const std::string & feature);
	};
}

#endif	//	HEADER_GUARD_CORE__VERSION_H__
