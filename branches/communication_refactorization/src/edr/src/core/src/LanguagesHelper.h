/********************************************************************
    created:  2014/02/18
    created:  18:2:2014   16:52
    filename: LanguagesHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGESHELPER_H__
#define HEADER_GUARD_CORE__LANGUAGESHELPER_H__

#include <string>
#include <QtGui/QIcon>

namespace core
{
	//! Klasa pomocnicza przy �adowaniu j�zyk�w
	class LanguagesHelper
	{
	public:

		//! Metoda inicjuj�ca obs�ug� j�zyk�w
		//! Musi by� wywo�ana przed innymi odwo�aniami do metod tej klasy
		//! Nale�y j� wo�a� tylko raz
		static void init();
		//! \param language Nazwa j�zyka po angielsku - z du�ej litery
		//! \return Nazwa j�zyka w tym j�zyku, natywnie
		static const std::string nativeLanguageName(const std::string & language);
		//! \param language Nazwa j�zyka po angielsku - z du�ej litery
		//! \return Ikona z flag� kraju dla danego j�zyka
		static const QIcon nationalFlagForLanguage(const std::string & language);
		//! \param code Kod j�zyka wg ISO639
		//! \return Nazwa j�zyka po angielsku odpowiadaj�ca danemu kodowi lub pusty ci�g je�li nierozpoznano
		static std::string languageFromISO639Code(const std::string & code);
		//! \param locale Opis locale w formie lang_country wg ISO639 (j�zyk) i ISO 3166 (kraj)
		//! J�zyk ma�ymi literami a kraj du�ymi
		//! \return Nazwa j�zyka po angielsku dla zdekodowanego kodu j�zyka lub pusty ci�g je�eli nierozpoznano
		static std::string extractLanguageISO639Code(const std::string & locale);
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGESHELPER_H__
