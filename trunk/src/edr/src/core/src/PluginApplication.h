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
#include "DataManager.h"
#include "StreamDataManager.h"
#include "FileDataManager.h"

namespace core {

class PluginApplication : public IApplication
{
public:

	PluginApplication(const std::string & pluginName);
	virtual ~PluginApplication();

	//! \return Opis aplikacji
	IApplicationDescription * description();
	//! \return Manager do zarz�dzania t�umaczeniem aplikacji
	ILanguagesManager * languagesManager();
	//! \return Interfejs �cie�ek aplikacji
	virtual IPath * paths();
	//! \return Interfejs do logowania
	virtual loglib::ILog * log();
	//! \return Interfejs do czytania danych
	virtual IDataManagerReader * dataManagerReader();
	//! \return Interfejs do czytania informacji o parserach
	virtual IParserManagerReader * parserManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual IStreamManagerReader * streamDataManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual IFileManagerReader * fileDataManagerReader();
	//! \return Interfejs do czytania opisu zarejestrowanych danych
	virtual IRegisteredDataTypesManagerReader * registeredDataTypesManagerReader();
	//! \return Interfejs do czytania opisu zarejestrowanych danych
	virtual IDataHierarchyManagerReader * dataHierarchyManagerReader();
	//! \return Interfejs do dedykowanej przestrzeni sk��dowania danych
	virtual IPluginStorage * pluginStorage();
	//! \return Interfejs dost�pu do serwis�w
	virtual IServiceManager * serviceManager();
	//! \return Interfejs dost�pu do serwis�w
	virtual ISourceManager * sourceManager();
	//! \return Interfejs managera wizualizator�w
	virtual IVisualizerManager * visualizerManager();
	//! \return Interfejs managera wizualizator�w
	virtual IThreadPool * threadPool();
	//! \return Interfejs dost�pu do serwis�w
	virtual IJobManager * jobManager();

private:

	utils::shared_ptr<IPath> path_;
	utils::shared_ptr<loglib::ILog> log_;
};

class MainViewApplication : public IApplication
{
public:

	MainViewApplication();
	virtual ~MainViewApplication();

	//! \return Opis aplikacji
	virtual IApplicationDescription * description();
	//! \return Manager translacji aplikacji - pozwala zmienia� j�zyk
	virtual ILanguagesManager * languagesManager();
	//! \return Interfejs �cie�ek aplikacji
	virtual IPath * paths();
	//! \return Interfejs do logowania
	virtual loglib::ILog * log();
	//! \return Interfejs do czytania danych
	virtual IDataManagerReader * dataManagerReader();
	//! \return Interfejs do czytania informacji o parserach
	virtual IParserManagerReader * parserManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual IStreamManagerReader * streamDataManagerReader();
	//! \return Interfejs do czytania danych strumieniowych
	virtual IFileManagerReader * fileDataManagerReader();
	//! \return Interfejs do czytania opisu zarejestrowanych danych
	virtual IRegisteredDataTypesManagerReader * registeredDataTypesManagerReader();
	//! \return Interfejs do czytania opisu hierarchi danych
	virtual IDataHierarchyManagerReader * dataHierarchyManagerReader();
	//! \return Interfejs do dedykowanej przestrzeni sk��dowania danych
	virtual IPluginStorage * pluginStorage();
	//! \return Interfejs dost�pu do serwis�w
	virtual IServiceManager * serviceManager();
	//! \return Interfejs dost�pu do serwis�w
	virtual ISourceManager * sourceManager();
	//! \return Interfejs managera wizualizator�w
	virtual IVisualizerManager * visualizerManager();
	//! \return Interfejs managera wizualizator�w
	virtual ThreadPool * threadPool();
	//! \return Interfejs dost�pu do serwis�w
	virtual JobManager * jobManager();

private:
	//! \return Interfejs do logowania
	virtual loglib::ILog * exceptionLog();
};

}

#endif	//	HEADER_GUARD___PLUGINAPPLICATION_H__
