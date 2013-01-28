/********************************************************************
    created:  2013/01/07
    created:  7:1:2013   15:42
    filename: PluginApplication.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PLUGINAPPLICATION_H__
#define HEADER_GUARD___PLUGINAPPLICATION_H__

#include <corelib/IApplication.h>
#include "VisualizerManager.h"
#include "ServiceManager.h"
#include "SourceManager.h"
#include "MemoryDataManager.h"
#include "StreamDataManager.h"
#include "FileDataManager.h"

namespace core {

class PluginApplication : public IApplication
{
public:

	PluginApplication(const std::string & pluginName);

	//! \return Interfejs ścieżek aplikacji
	virtual IPath * pathInterface();
	//! \return Interfejs do logowania
	virtual ILog * log();
	//! \return Interfejs do czytania danych
	virtual IDataManagerReader * dataManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual IStreamManagerReader * streamDataManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual IFileManagerReader * fileDataManagerReader();
	//! \return Interfejs do czytania opisu zarejestrowanych danych
	virtual IDataHierarchyManagerReader * dataHierarchyManagerReader();
	//! \return Interfejs do dedykowanej przestrzeni skłądowania danych
	virtual IPluginStorage * pluginStorage();
	//! \return Interfejs dostępu do serwisów
	virtual IServiceManager * serviceManager();
	//! \return Interfejs managera wizualizatorów
	virtual IVisualizerManager * visualizerManager();

private:

	shared_ptr<IPath> path_;
	shared_ptr<ILog> log_;
};

}

#endif	//	HEADER_GUARD___PLUGINAPPLICATION_H__
