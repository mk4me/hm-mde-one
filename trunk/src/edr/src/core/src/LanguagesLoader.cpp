#include "CorePCH.h"
#include "LanguagesLoader.h"
#include "LanguagesHelper.h"
#include "LanguagesManager.h"
#include "ApplicationCommon.h"
#include <corelib/BaseDataTypes.h>
#include <corelib/Filesystem.h>
#include <QtCore/QTranslator>

void registerTranslator(const std::string & locale, const core::Filesystem::Path & path,
	const std::string & module, core::LanguagesManager * languagesManager)
{
	// probuje wyci¹gn¹æ jêzyk dla tego kodu
	auto lang = core::LanguagesHelper::extractLanguageISO639Code(locale);
	// jeœli siê uda³o to próbujemy dalej
	if(lang.empty() == false){

		// translator
		utils::shared_ptr<QTranslator> translator(new QTranslator);

		//probujemy ladowac tlumaczenia
		if(translator->load(path.string().c_str()) == true){
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

void LanguagesLoader::loadCoreTranslations(std::vector<Filesystem::Path> & paths,
	LanguagesManager * languagesManager)
{
	std::vector<int> toErase;

	for(auto i = 0; i < paths.size(); ++i){
		//nazwa pliku
		auto file = paths[i].stem().string();
		auto pos = file.find("qt_");
		//teraz szukam t³umaczeñ dla core/qt/widoków
		if(pos != std::string::npos){

			registerTranslator(file.substr(pos + 3, file.size() - pos - 3),
				paths[i], "qt", languagesManager);

			toErase.push_back(i);

		}else if( (pos = file.find("lang_")) != std::string::npos
			&& file.find("plugin") == std::string::npos && pos > 1){

			registerTranslator(file.substr(pos + 5, file.size() - pos - 5),
				paths[i], file.substr(0, pos - 1), languagesManager);

			toErase.push_back(i);
		}else{
			CORE_LOG_INFO("Skipping translation file: " << paths[i] << " as not matching any core translation pattern");
		}
	}

	for(int j = toErase.size()-1; j > -1; --j){
		paths.erase(paths.begin()+toErase[j]);
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

void LanguagesLoader::loadPluginTranslations(std::vector<Filesystem::Path> & paths,
	const std::string & pluginName, LanguagesManager * languagesManager)
{
	std::vector<int> toErase;

	for(auto i = 0; i < paths.size(); ++i){
		//nazwa pliku
		auto file = paths[i].stem().string();

		auto pos = file.find("lang_");

		if( pos != std::string::npos && file.find(pluginName) != std::string::npos
			&& pos > 1){

			registerTranslator(file.substr(pos + 5, file.size() - pos - 5),
				paths[i], file.substr(0, pos - 1), languagesManager);

			toErase.push_back(i);

		}else{
			CORE_LOG_DEBUG("Skipping translation file: " << paths[i] << " as not matching plugin`s " << pluginName << " translation pattern");
		}
	}

	for(int j = toErase.size()-1; j > -1; --j){
		paths.erase(paths.begin()+toErase[j]);
	}
}