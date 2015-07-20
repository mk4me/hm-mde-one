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
#define CORE_API_MINOR 6	//! Podrz�na wersja API, zmiany tej warto�ci powinny zachowywa� kompatybilno�� wstecz (+ ABI)
#define CORE_API_PATCH 0	//! Wersja poprawek dla danej wersji i podwersji - zachowuje kompatybilno�� wstecz

namespace core
{
	//! Klasa obs�uguj�ca opis wersji API aplikacji
	class CORELIB_EXPORT Version
	{
	public:

		//! Typ opisuj�cy numery sk�adowe wersji
		typedef unsigned int VersionNumberType;

	public:
		//! Klasa opisuj�ca dozwolone warto�ci dla parametr�w wersji
		class Traits
		{
		public:
			//! \return Maksymalna warto�� dla g��wnego numeru wersji
			static const VersionNumberType maxMajor();
			//! \return Minimalna warto�� dla g��wnego numeru wersji
			static const VersionNumberType minMajor();
			//! \param val Warto�� g��wnego numeru wersji kt�ra weryfikujemy
			//! \return Prawda je�li g��wny numer wersji spe�nia majorMin() <= val < majorMax()
			static const bool verifyMajor(const VersionNumberType val);
			//! \return Maksymalna warto�� dla podrz�dnego numeru wersji
			static const VersionNumberType maxMinor();
			//! \return Minimalna warto�� dla podrz�dnego numeru wersji
			static const VersionNumberType minMinor();
			//! \param val Warto�� podrz�dnego numeru wersji kt�ra weryfikujemy
			//! \return Prawda je�li podrz�dny numer wersji spe�nia majorMin() <= val < majorMax()
			static const bool verifyMinor(const VersionNumberType val);
			//! \return Maksymalna warto�� dla patcha numeru wersji
			static const VersionNumberType maxPatch();
			//! \return Minimalna warto�� dla patcha numeru wersji
			static const VersionNumberType minPatch();
			//! \param val Warto�� patcha numeru wersji kt�ra weryfikujemy
			//! \return Prawda je�li patch numer wersji spe�nia majorMin() <= val < majorMax()
			static const bool verifyPatch(const VersionNumberType val);
			//! \param version Wersja kt�r� weryfikujemy
			//! \return Czy wersja ma poprawne warto�ci
			static const bool verify(const Version & version);
		};

		//! \param major Numer g��wnej wersji
		//! \param minor Numer podrz�dny wersji
		//! \param patch Numer poprawaki dla wersji o podwersji
		Version(const VersionNumberType major = CORE_API_MAJOR,
			const VersionNumberType minor = CORE_API_MINOR,
			const VersionNumberType patch = CORE_API_PATCH);

		//! Destruktor
		virtual ~Version();

		//! \return Numer g��wnej wersji
		const VersionNumberType major() const;
		//! \return Numer podrz�dny wersji
		const VersionNumberType minor() const;
		//! \return Numer poprawaki dla wersji o podwersji
		const VersionNumberType patch() const;
		//! \param version Wersja kt�ra reprezentujemy jako ci�g znak�w
		//! \param separator Ci�g znak�w oddzielaj�cy poszczeg�lne sk�adowe wersji
		//! \return Wersja w formie litera�u - sk�adowe wersji oddzielone separatorem
		//! w formacie [major][separator][minor][separator][patch]
		static const std::string formatedVersion(const Version & version = Version(),
			const std::string & separator = std::string("."));
		//! \param version Wersja kt�ra sprawdzamy
		//! \return Czy versja spe�nia wymagane parametry
		const bool isAtLeast(const Version & version);
		//! \param version Wersja z kt�r� si� por�wnujemy
		//! \return Czy dana wersja jest mniejsza od version
		const bool operator<(const Version & version);
		//! \param version Wersja z kt�r� si� por�wnujemy
		//! \return Czy dana wersja jest r�wna version
		const bool operator==(const Version & version);
		//! \param version Wersja z kt�r� si� por�wnujemy
		//! \return Czy dana wersja jest mniejsza lub r�wna version
		const bool operator<=(const Version & version);
		//! \param version Wersja z kt�r� si� por�wnujemy
		//! \return Czy dana wersja jest wi�ksza od version
		const bool operator>(const Version & version);
		//! \param version Wersja z kt�r� si� por�wnujemy
		//! \return Czy dana wersja jest wi�ksza od version
		const bool operator>=(const Version & version);

	private:
		//! G��wna wersja
		const VersionNumberType major_;
		//! Podrz�dna wersja
		const VersionNumberType minor_;
		//! Pacz:)
		const VersionNumberType patch_;
	};

	//! Klasa reprezentuj�ca wewrsj� wraz z mo�liwo�ciami jakie ona ze sob� niesie
	class CORELIB_EXPORT ExtendedVersion : public Version
	{
	public:
		//! \param major Numer g��wnej wersji
		//! \param minor Numer podrz�dny wersji
		//! \param patch Numer poprawaki dla wersji o podwersji
		ExtendedVersion(const VersionNumberType major = 0,
			const VersionNumberType minor = 0,
			const VersionNumberType patch = 0);

		//! Destruktor
		virtual ~ExtendedVersion();

		//! \param featureName Nazwa mo�liwo�ci o kt�re pytamy
		//! \return Prawda je�li dana mo�liwo�� jest wspierana
		virtual const bool hasFeature(const std::string & featureName) const;
	};
}

#endif	//	HEADER_GUARD_CORE__VERSION_H__
