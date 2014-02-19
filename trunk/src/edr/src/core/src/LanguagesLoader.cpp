#include "CorePCH.h"
#include "LanguagesLoader.h"
#include "LanguagesHelper.h"
#include "LanguagesManager.h"
#include "ApplicationCommon.h"
#include <corelib/BaseDataTypes.h>
#include <corelib/Filesystem.h>
#include <QtCore/QTranslator>

void registerTranslator(const std::string & locale, const std::string & path,
	const std::string & module, core::LanguagesManager * languagesManager)
{
	// probuje wyci¹gn¹æ jêzyk dla tego kodu
	auto lang = core::LanguagesHelper::extractLanguageISO639Code(locale);
	// jeœli siê uda³o to próbujemy dalej
	if(lang.empty() == false){

		// translator
		utils::shared_ptr<QTranslator> translator(new QTranslator);

		//probujemy ladowac tlumaczenia
		if(translator->load(path.c_str()) == true){
			languagesManager->registerTranslator(module, lang, translator);
			CORE_LOG_DEBUG("Registered " << lang << " translator for module " << module << " from " << path);
		}else{
			CORE_LOG_DEBUG("Could not load " << lang << " translator for module " << module << " from " << path);
		}
	}else{
		CORE_LOG_DEBUG("Could not decode language for module " << module << " and file " << path << " for delivered locale " << locale);
	}
}

using namespace core;

void LanguagesLoader::loadCoreTranslations(const std::string & path,
	LanguagesManager * languagesManager)
{
	auto coreTranslations = core::Filesystem::listFiles(path, true, ".qm");

	for(auto it = coreTranslations.begin(); it != coreTranslations.end(); ++it){
		//nazwa pliku
		auto file = core::Filesystem::Path(*it).stem().string();
		auto pos = file.find("qt_");
		//teraz szukam t³umaczeñ dla core/qt/widoków
		if(pos != std::string::npos){

			registerTranslator(file.substr(pos + 3, file.size() - pos - 3),
				*it, "qt", languagesManager);

		}else if( (pos = file.find("lang_")) != std::string::npos
			&& file.find("plugin") == std::string::npos && pos > 1){

			registerTranslator(file.substr(pos + 5, file.size() - pos - 5),
				*it, file.substr(0, pos - 1), languagesManager);

		}else{
			CORE_LOG_INFO("Skipping translation file: " << *it << " as not matching any core translation pattern");
		}
	}
}

void LanguagesLoader::loadPluginDefaultTranslation(const std::string & pluginName,
	const std::string & defaultLanguageCode,
	LanguagesManager * languagesManager)
{
	// probuje wyci¹gn¹æ jêzyk dla tego kodu
	auto lang = core::LanguagesHelper::extractLanguageISO639Code(defaultLanguageCode);

	// jeœli siê uda³o to próbujemy dalej
	if(lang.empty() == false){
		languagesManager->registerTranslator(pluginName, lang, utils::shared_ptr<QTranslator>());
		CORE_LOG_DEBUG("Registered default language " << lang << " for module " << pluginName);
	}else{
		CORE_LOG_DEBUG("Could not decode language for module " << pluginName << " from delivered code " << defaultLanguageCode);
	}
}

void LanguagesLoader::loadPluginTranslations(const std::string & path,
	const std::string & pluginName, LanguagesManager * languagesManager)
{
	auto pluginsTranslations = core::Filesystem::listFiles(path, true, ".qm");

	for(auto it = pluginsTranslations.begin(); it != pluginsTranslations.end(); ++it){
		//nazwa pliku
		auto file = core::Filesystem::Path(*it).stem().string();

		auto pos = file.find("lang_");

		if( pos != std::string::npos && file.find(pluginName) != std::string::npos
			&& pos > 1){

			registerTranslator(file.substr(pos + 5, file.size() - pos - 5),
				*it, file.substr(0, pos - 1), languagesManager);

		}else{
			CORE_LOG_DEBUG("Skipping translation file: " << *it << " as not matching plugin`s " << pluginName << " translation pattern");
		}
	}
}