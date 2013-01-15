/********************************************************************
    created:  2013/01/07
    created:  7:1:2013   15:42
    filename: CustomApplication.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CUSTOMAPPLICATION_H__
#define HEADER_GUARD___CUSTOMAPPLICATION_H__

#include <core/IApplication.h>
#include "VisualizerManager.h"
#include "ServiceManager.h"
#include "SourceManager.h"
#include "DataSourceManager.h"
#include "DataProcessorManager.h"
#include "MemoryDataManager.h"
#include "StreamDataManager.h"
#include "FileDataManager.h"

namespace core {

//TODO
//stworzyæ odpowiedni¹ klasê - API i wrapper
class DataSinkManager
{

};

class CustomApplication : public plugin::IApplication
{
public:

	CustomApplication(plugin::IPath * pathInterface, core::ILog * log,
		plugin::IDataManagerReader * dataManagerReader,
		plugin::IStreamDataManagerReader * streamDataManagerReader,
		plugin::IFileDataManagerReader * fileDataManagerReader,
		plugin::IDataHierarchyManagerReader * dataHierarchyManagerReader,
		plugin::IExtensionManagerReader * dataExtensionManagerReader,
		plugin::IPluginStorage * pluginStorage,
		plugin::IServiceManager * serviceManager);

	//! \return Interfejs œcie¿ek aplikacji
	virtual plugin::IPath * pathInterface();
	//! \return Interfejs do logowania
	virtual core::ILog * log();
	//! \return Interfejs do czytania danych
	virtual plugin::IDataManagerReader * dataManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual plugin::IStreamDataManagerReader * streamDataManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual plugin::IFileDataManagerReader * fileDataManagerReader();
	//! \return Interfejs do czytania opisu zarejestrowanych danych
	virtual plugin::IDataHierarchyManagerReader * dataHierarchyManagerReader();
	//! \return Interfejs do czytania opisu zarejestrowanych rozszerzeñ plików przez parsery
	virtual plugin::IExtensionManagerReader * dataExtensionManagerReader();
	//! \return Interfejs do dedykowanej przestrzeni sk³¹dowania danych
	virtual plugin::IPluginStorage * pluginStorage();
	//! \return Interfejs dostêpu do serwisów
	virtual plugin::IServiceManager * serviceManager();

private:

	plugin::IPath * pathInterface_;
	core::ILog * log_;
	plugin::IDataManagerReader * dataManagerReader_;
	plugin::IStreamDataManagerReader * streamDataManagerReader_;
	plugin::IFileDataManagerReader * fileDataManagerReader_;
	plugin::IDataHierarchyManagerReader * dataHierarchyManagerReader_;
	plugin::IExtensionManagerReader * dataExtensionManagerReader_;
	plugin::IPluginStorage * pluginStorage_;
	plugin::IServiceManager * serviceManager_;
};

class StreamDataManager;

class ExtendedCustomApplication : public plugin::IApplication
{
public:

	ExtendedCustomApplication(plugin::IPath * pathInterface, MemoryDataManager * memoryDataManager,
		StreamDataManager * streamDataManager, FileDataManager * fileDataManager,
		plugin::IPluginStorage * pluginStorage, core::ILog * log, VisualizerManager * visualizerManager,
		ServiceManager * serviceManager, SourceManager * sourceManager,
		DataSourceManager * dataSourceManager, DataProcessorManager * dataProcessorManager,
		DataSinkManager * dataSinkManager);

	//! \return Interfejs œcie¿ek aplikacji
	virtual plugin::IPath * pathInterface();
	//! \return Interfejs do czytania danych
	virtual plugin::IDataManagerReader * dataManagerReader();
	//! \return Interfejs do czytania danych
	MemoryDataManager * memoryDataManager();

	StreamDataManager * streamDataManager();

	FileDataManager * fileDataManager();
	//! \return Interfejs do dedykowanej przestrzeni sk³¹dowania danych
	virtual plugin::IPluginStorage * pluginStorage();
	//! \return Interfejs do logowania
	virtual core::ILog * log();
	//! \return Interfejs do logowania
	core::ILog * logPrototype();

	//! \return Interfejs dostêpu do serwisów
	virtual ServiceManager * serviceManager();

	SourceManager * sourceManager();
	DataSourceManager * dataSourceManager();
	DataProcessorManager * dataProcessorManager();
	DataSinkManager * dataSinkManager();
	VisualizerManager * visualizerManager();

private:

	plugin::IPath * pathInterface_;
	MemoryDataManager * memoryDataManager_;
	StreamDataManager * streamDataManager_;
	FileDataManager * fileDataManager_;
	plugin::IPluginStorage * pluginStorage_;
	core::ILog * log_;
	VisualizerManager * visualizerManager_;
	ServiceManager * serviceManager_;
	SourceManager * sourceManager_;
	DataSourceManager * dataSourceManager_;
	DataProcessorManager * dataProcessorManager_;
	DataSinkManager * dataSinkManager_;
};

}

#endif	//	HEADER_GUARD___CUSTOMAPPLICATION_H__
