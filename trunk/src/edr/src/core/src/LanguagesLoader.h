/********************************************************************
    created:  2014/02/18
    created:  18:2:2014   12:12
    filename: LanguagesLoader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGESLOADER_H__
#define HEADER_GUARD_CORE__LANGUAGESLOADER_H__

#include <string>

namespace core
{
	class LanguagesManager;

	//! Klasa realizuj�ca �adowanie t�umacze� wg przyj�tego w aplikacji i skryptach CMake
	//! schematu nazewniczego
	//! TODO
	//! Wymaga formalnego opisania i doprecyzowania
	//! 
	//! TODO
	//! uwzglednic filtrowanie dla tlumaczen glownego okna - projektu ktory faktycznie startuje
	//! np. IMU, MEDUSA, MDE, old _view przy ladowaniu tlumaczen dla core
	class LanguagesLoader
	{
	public:
		//! \param paths [out] �cie�ki do t�umacze� kt�re nale�y zweryfikowa�,
		//! podczas ladowania usuwane sa sciezki zaladowane
		//! \param languageManager Manager t�umacze� zasilany znalezionymi t�umaczeniami
		static void loadCoreTranslations(std::vector<Filesystem::Path> & paths,
			LanguagesManager * languagesManager);

		//! \param paths [out] �cie�ki do t�umacze� kt�re nale�y zweryfikowa�,
		//! podczas ladowania usuwane sa sciezki zaladowane
		//! \param pluginName Nazwa pluginu kt�ra musi wyst�pi� w nazwie t�umaczenia
		//! \param languageManager Manager t�umacze� zasilany znalezionymi t�umaczeniami
		static void loadPluginTranslations(std::vector<Filesystem::Path> & paths,
			const std::string & pluginName,	LanguagesManager * languagesManager);

		//! \param pluginName Nazwa pluginu kt�ra musi wyst�pi� w nazwie t�umaczenia
		//! \param defaultLanguageCode Domy�lny j�zyk plugina, dla ktorego nie ma tlumaczenia
		//! \param languageManager Manager t�umacze� zasilany znalezionymi t�umaczeniami
		static void loadPluginDefaultTranslation(const std::string & pluginName,
			const std::string & defaultLanguageCode,
			LanguagesManager * languagesManager);
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGESLOADER_H__
