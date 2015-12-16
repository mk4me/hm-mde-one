/********************************************************************
    created:  2015/11/26 13:17
    filename: ContextConfigurationSettingsFile.h
    author:   Wojciech Knieæ
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__ContextConfigurationSettingsFile_h__
#define HEADER_GUARD_HMDBCOMMUNICATION__ContextConfigurationSettingsFile_h__

#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/Export.h>

namespace hmdbCommunication {
	class HMDBCOMMUNICATION_EXPORT ContextConfigurationSettingsFile
	{
	public:
		virtual ~ContextConfigurationSettingsFile() {}

	public:
		static IHMDBSourceViewManager::ContextConfiguration read(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& defaultConf = defaultConfig());
		static void write(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& ccfg);

		static IHMDBSourceViewManager::ContextConfiguration defaultConfig();
	};
}

#endif