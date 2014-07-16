/********************************************************************
    created:  2014/02/17
    created:  17:2:2014   18:45
    filename: LanguagesManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGESMANAGER_H__
#define HEADER_GUARD_CORE__LANGUAGESMANAGER_H__

#include <corelib/ILanguagesManager.h>
#include <corelib/BaseDataTypes.h>
#include <map>

class QTranslator;

namespace core
{
	class LanguagesManager : public ILanguagesManager
	{
	public:
		//! Wskaznik translatora
		typedef utils::shared_ptr<QTranslator> TranslatorPtr;

	private:
		//! Mapa lokalizacji do t�umaczy
		typedef std::map<std::string, TranslatorPtr> LocalTranslators;
		//! Mapa modu��w do ich t�umacze�
		typedef std::map<std::string, LocalTranslators> ModuleTranslators;

	public:
		//! Konstruktor domy�lny
		LanguagesManager();
		//! Destruktor wirtualny
		virtual ~LanguagesManager();
		//! \return Zwraca nazwe aktualnie ustawionego j�zyka po angielsku
		virtual const std::string currentLanguage() const;
		//! \param language J�zyk aplikacji w wersji angielskiej
		virtual void setLanguage(const std::string & language);
		//! \return Zbi�r zarejestrowanych j�zyk�w po angielsku
		virtual const StringSet registeredLanguages() const;
		//! \return Zbi�r zarejestrowanych modu��w aplikacji z t�umaczeniami
		//! Jako modu� rozumiemy pluginy, okno g��wne, core i qt jako framework okien
		virtual const StringSet registeredModules() const;
		//! \param module Modul o ktorego j�zyki odpytujemy
		//! \return Lista jezykow tlumaczen dla danego modulu
		virtual const StringSet moduleLanguages(const std::string & module) const;
		//! \return Nazwa danego j�zyka w tym j�zyku
		virtual const std::string nativeLanguageName(const std::string & language) const;
		//! \param language Jezyk o ktory pytamy
		//! \return Czy tlumaczenie jezyka kompletne?
		virtual const bool languageComplete(const std::string & language) const;
		//! \return Ikona z flag� kraju (najwiekszego w ktorym dany jezyk jest glownym jezykiem)
		virtual const QIcon nationalFlagForLanguage(const std::string & language) const;
		//! Metoda rejestruje translator dla zadanego modulu i j�zyka
		//! \param module Modu� aplikacji dla kt�rego rejestrujemy t�umacz
		//! \param language J�zyk dla kt�rego instalujemy t�umacz
		//! \param translator Instalowany t�umacz
		void registerTranslator(const std::string & module, const std::string & language,
			TranslatorPtr translator);

		//! \param module Modu� aplikacji dla kt�rego usuwamy wszystkie t�umacze
		void unregisterTranslators(const std::string & module);

	private:
		//! Wy��cza t�umaczenia aplikacji dla aktualnie ustawionego j�zyka
		void reloadCurrentLanguage(const std::string & language);

		//! \param language Jezyk ktory probuje usuna�
		void tryRemoveUnusedLanguage(const std::string & language);

	private:
		//! Zarejestrowane translatory
		ModuleTranslators translators;
		//! Aktualny j�zyk
		std::string currentLanguage_;
		//! Zarejestrowane jezyki
		StringSet languages_;
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGESMANAGER_H__
