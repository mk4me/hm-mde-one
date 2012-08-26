﻿/********************************************************************
    created:  2011/04/21
    created:  21:4:2011   7:59
    filename: PluginCommon.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGINCOMMON_H__
#define HEADER_GUARD_CORE__PLUGINCOMMON_H__

//#include <core/IWorkflowManager.h>
#include <core/StringTools.h>
#include <core/IPath.h>

namespace core 
{

	class ILog;
	class IDataManagerReader;
	class IVisualizerManager;
	class IServiceManager;
	class ISourceManager;
	class IDataProcessorManager;
	class IDataSourceManager;

    //! Struktura przechowująca informacje o managerach. Tylko do wewnętrznego użytku.
    struct InstanceInfo
    {
        IDataManagerReader* dataManagerReader;
        IVisualizerManager* visualizerManager;
        IServiceManager* serviceManager;
		ISourceManager* sourceManager;
		IPath* pathInterface;
		ILog* logInterface;
        IDataProcessorManager* dataProcessorManager;
        IDataSourceManager* dataSourceManager;
        //IWorkflowManager* workflowManager;
    };

    //! Zmienna defininowana przez makro tworzące pluginy. Tylko do użytku wewnętrznego.
    extern InstanceInfo __instanceInfo;

    //! Makro definiujące zmienną przechowującą managery. Automatycznie używane w pluginach.
    #define CORE_DEFINE_INSTANCE_INFO namespace core { InstanceInfo __instanceInfo = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr/*, nullptr*/ }; }

    //! \return Bieżąca instancja data managera readera. Rozwiązanie w ten sposób, w stosunku do
    //! klasycznego upublicznienia tylko nagłówków funkcji i schowania definicji, pozwala
    //! na rozwijanie, więc jest potencjalnie szybsze.
    inline IDataManagerReader* getDataManagerReader()
    {
        return __instanceInfo.dataManagerReader;
    }

    //! \return Interfejs dostępu do ścieżek aplikacji
	inline IPath* getPathInterface()
	{
		return __instanceInfo.pathInterface;
	}

    //! \return Interfejs logowania informacji
	inline ILog* getLogInterface()
	{
		return __instanceInfo.logInterface;
	}

    ////!
    //inline IVisualizerManager* getVisualizerManager()
    //{
    //    return __instanceInfo.visualizerManager;
    //}

    //!
    inline IServiceManager* getServiceManager()
    {
        return __instanceInfo.serviceManager;
    }

    ////!
    //inline IDataProcessorManager* getDataProcessorManager()
    //{
    //    return __instanceInfo.dataProcessorManager;
    //}

    ////!
    //inline IDataSourceManager* getDataSourceManager()
    //{
    //    return __instanceInfo.dataSourceManager;
    //}

    ////!
    //inline IWorkflowManager* getWorkflowManager()
    //{
    //    return __instanceInfo.workflowManager;
    //}

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const Filesystem::Path& getResourcesPath()
    {
        return getPathInterface()->getResourcesPath();
    }

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const Filesystem::Path& getApplicationDataPath()
    {
        return getPathInterface()->getApplicationDataPath();
    }

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const Filesystem::Path& getUserDataPath()
    {
        return getPathInterface()->getUserDataPath();
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline toString_t getResourceString(const std::string& str)
    {
        return toString_t(toStdString(getResourcesPath()/str));
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline toString_t getApplicationDataString(const std::string& str)
    {
        return toString_t(toStdString(getApplicationDataPath()/str));
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline toString_t getUserDataString(const std::string& str)
    {
        return toString_t(toStdString(getUserDataPath()/str));
    }

} // namespace core


#endif  // HEADER_GUARD_CORE__PLUGINCOMMON_H__
