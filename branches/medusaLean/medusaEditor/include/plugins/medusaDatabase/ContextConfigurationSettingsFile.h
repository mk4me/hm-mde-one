/********************************************************************
    created:  2015/11/26 13:17
    filename: ContextConfigurationSettingsFile.h
    author:   Wojciech Knie�
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_medusaDatabase__ContextConfigurationSettingsFile_h__
#define HEADER_GUARD_medusaDatabase__ContextConfigurationSettingsFile_h__

#include <plugins/medusaDatabase/IHMDBSourceViewManager.h>
#include <plugins/medusaDatabase/Export.h>

namespace medusaDatabase {
	//! Klasa 
	class medusaDatabase_EXPORT ContextConfigurationSettingsFile
	{
	public:
		virtual ~ContextConfigurationSettingsFile() {}

	public:
		//! Wczytuje plik konfiguracyjny nadpisuj�c wpisy w przekazanej konfiguracji. Metoda nie zwraca b��du przy braku pliku. 
		//! \param path �cie�ka do pliku konfiguracyjnego
		//! \param defaultConf Nadpisywana konfiguracja
		//! \return nowa, zaktualizowana konfiguracja
		static IHMDBSourceViewManager::ContextConfiguration read(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& defaultConf = defaultConfig());
		//! Zapisuje podan� konfiguracj� do podanego pliku
		//! \param path �cie�ka, gdzie ma zosta� zapisana konfiguracja
		//! \param ccfg Zapisywana konfiguracja
		static void write(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& ccfg);
		//! \return Zwraca domy�ln� konfiguracj� stosowan� w PJ
		static IHMDBSourceViewManager::ContextConfiguration defaultConfig();
	};
}

#endif