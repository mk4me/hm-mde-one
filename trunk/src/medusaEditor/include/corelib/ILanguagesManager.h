/********************************************************************
    created:  2014/02/17
    created:  17:2:2014   18:38
    filename: ILanguagesManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ILANGUAGESMANAGER_H__
#define HEADER_GUARD_CORE__ILANGUAGESMANAGER_H__

#include <QtGui/QIcon>
#include <string>
#include <set>

namespace core
{
	//! Klasa zarzadzaj�ca j�zykiem aplikacji
	class ILanguagesManager
	{
	public:
		//! Lista agreguj�ca ci�gi znak�w
		typedef std::set<std::string> StringSet;
	public:
		//! Destruktor wirtualny
		virtual ~ILanguagesManager() {}
		//! \return Zwraca nazwe aktualnie ustawionego j�zyka po angielsku
		virtual const std::string currentLanguage() const = 0;
		//! \param language J�zyk aplikacji w wersji angielskiej
		virtual void setLanguage(const std::string & language) = 0;
		//! \return Zbi�r zarejestrowanych j�zyk�w po angielsku
		virtual const StringSet registeredLanguages() const = 0;
		//! \return Zbi�r zarejestrowanych modu��w aplikacji z t�umaczeniami
		//! Jako modu� rozumiemy pluginy, okno g��wne, core i qt jako framework okien
		virtual const StringSet registeredModules() const = 0;
		//! \param module Modul o ktorego j�zyki odpytujemy
		//! \return Lista jezykow tlumaczen dla danego modulu
		virtual const StringSet moduleLanguages(const std::string & module) const = 0;
		//! \return Nazwa danego j�zyka w tym j�zyku
		virtual const std::string nativeLanguageName(const std::string & language) const = 0;
		//! \param language Jezyk o ktory pytamy
		//! \return Czy tlumaczenie jezyka kompletne - czy wszystkie moduly maja zarejestrowane tlumaczenia tego jezyka?
		virtual const bool languageComplete(const std::string & language) const = 0;
		//! \return Ikona z flag� kraju (najwiekszego w ktorym dany jezyk jest glownym jezykiem)
		virtual const QIcon nationalFlagForLanguage(const std::string & language) const = 0;
	};
}

#endif	//	HEADER_GUARD_CORE__ILANGUAGESMANAGER_H__
