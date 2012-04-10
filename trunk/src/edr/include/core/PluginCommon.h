/********************************************************************
    created:  2011/04/21
    created:  21:4:2011   7:59
    filename: PluginCommon.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGINCOMMON_H__
#define HEADER_GUARD_CORE__PLUGINCOMMON_H__

#include <core/IWorkflowManager.h>
#include <core/IDataManager.h>
#include <core/IVisualizerManager.h>
#include <core/IServiceManager.h>
#include <core/ISourceManager.h>
#include <core/IDataProcessorManager.h>
#include <core/IDataSourceManager.h>
#include <core/StringTools.h>
#include <core/IPath.h>
#include <core/ILog.h>
#include <core/Filesystem.h>

namespace core 
{
    //! Struktura przechowuj¹ca informacje o managerach. Tylko do wewnêtrznego u¿ytku.
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
        IWorkflowManager* workflowManager;
    };

    //! Zmienna defininowana przez makro tworz¹ce pluginy. Tylko do u¿ytku wewnêtrznego.
    extern InstanceInfo __instanceInfo;

    //! Makro definiuj¹ce zmienn¹ przechowuj¹c¹ managery. Automatycznie u¿ywane w pluginach.
    #define CORE_DEFINE_INSTANCE_INFO namespace core { InstanceInfo __instanceInfo = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; }

    //! \return Bie¿¹ca instancja data managera readera. Rozwi¹zanie w ten sposób, w stosunku do
    //! klasycznego upublicznienia tylko nag³ówków funkcji i schowania definicji, pozwala
    //! na rozwijanie, wiêc jest potencjalnie szybsze.
    inline IDataManagerReader* getDataManagerReader()
    {
        return __instanceInfo.dataManagerReader;
    }

    //! \return Interfejs dostêpu do œcie¿ek aplikacji
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

    //! Pomocnica metoda upraszczaj¹ca odwo³anie do katalogów.
    inline const Filesystem::Path& getResourcesPath()
    {
        return getPathInterface()->getResourcesPath();
    }

    //! Pomocnica metoda upraszczaj¹ca odwo³anie do katalogów.
    inline const Filesystem::Path& getApplicationDataPath()
    {
        return getPathInterface()->getApplicationDataPath();
    }

    //! Pomocnica metoda upraszczaj¹ca odwo³anie do katalogów.
    inline const Filesystem::Path& getUserDataPath()
    {
        return getPathInterface()->getUserDataPath();
    }

    //! \return Pomocnicza metoda do pobierania œcie¿ek.
    inline toString_t getResourceString(const std::string& str)
    {
        return toString_t(toStdString(getResourcesPath()/str));
    }

    //! \return Pomocnicza metoda do pobierania œcie¿ek.
    inline toString_t getApplicationDataString(const std::string& str)
    {
        return toString_t(toStdString(getApplicationDataPath()/str));
    }

    //! \return Pomocnicza metoda do pobierania œcie¿ek.
    inline toString_t getUserDataString(const std::string& str)
    {
        return toString_t(toStdString(getUserDataPath()/str));
    }

} // namespace core


#endif  // HEADER_GUARD_CORE__PLUGINCOMMON_H__