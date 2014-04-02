/********************************************************************
    created:  2011/04/21
    created:  21:4:2011   7:59
    filename: PluginCommon.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGINCOMMON_H__
#define HEADER_GUARD_CORE__PLUGINCOMMON_H__

#include <corelib/Filesystem.h>
#include <corelib/IApplication.h>
#include <corelib/IPath.h>
#include <corelib/ILog.h>
#include <utils/Macros.h>
#include <corelib/IMemoryDataManager.h>

namespace plugin 
{
    //! Zmienna defininowana przez makro tworzące pluginy. Tylko do użytku wewnętrznego.
    extern core::IApplication * __coreApplication;

    //! Makro definiujące zmienną przechowującą managery. Automatycznie używane w pluginach.
    #define PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR namespace plugin { core::IApplication * __coreApplication = nullptr; }

    //! \return Bieżąca instancja data managera readera. Rozwiązanie w ten sposób, w stosunku do
    //! klasycznego upublicznienia tylko nagłówków funkcji i schowania definicji, pozwala
    //! na rozwijanie, więc jest potencjalnie szybsze.
    inline core::IDataManagerReader* getDataManagerReader()
    {
        return __coreApplication->dataManagerReader();
    }

    inline core::IMemoryDataManagerHierarchy* getHierarchyManagerReader()
    {
        return dynamic_cast<core::IMemoryDataManagerHierarchy*>(__coreApplication->dataManagerReader());
    }

	inline core::IServiceManager* getServiceManager()
	{
		return __coreApplication->serviceManager();
	}
	 
	 inline core::IFileManagerReader * getFileDataManagerReader()
	 {
		 return __coreApplication->fileDataManagerReader();
	 }

	inline core::IDataHierarchyManagerReader* getDataHierachyManagerReader()
	{
		return __coreApplication->dataHierarchyManagerReader();
	}

	inline core::ISourceManager* getSourceManager()
	{
		return __coreApplication->sourceManager();
	}

	inline core::IParserManagerReader* getParserManagerReader()
	{
		return __coreApplication->parserManagerReader();
	}

    //! \return Interfejs dostępu do ścieżek aplikacji
	inline core::IPath* getPaths()
	{
		return __coreApplication->paths();
	}

	//! \return Interfejs dostępu do ścieżek aplikacji
	inline core::IVisualizerManager* getVisualizerManager()
	{
		return __coreApplication->visualizerManager();
	}

	//! \return Interfejs dostępu do puli wątków
	inline core::IThreadPool* getThreadPool()
	{
		return __coreApplication->threadPool();
	}

	//! \return Interfejs dostępu do managera zadań
	inline core::IJobManager* getJobManager()
	{
		return __coreApplication->jobManager();
	}

    //! \return Interfejs logowania informacji
	inline core::ILog* getLogInterface()
	{
		return __coreApplication->log();
	}

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const core::Filesystem::Path& getResourcesPath()
    {
        return getPaths()->getResourcesPath();
    }

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const core::Filesystem::Path& getApplicationDataPath()
    {
        return getPaths()->getApplicationDataPath();
    }

	//! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const core::Filesystem::Path& getUserApplicationDataPath()
    {
        return getPaths()->getUserApplicationDataPath();
    }

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const core::Filesystem::Path& getUserDataPath()
    {
        return getPaths()->getUserDataPath();
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline core::Filesystem::Path getResourcePath(const core::Filesystem::Path& path)
    {
        return getResourcesPath()/path;
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline core::Filesystem::Path getApplicationDataPath(const core::Filesystem::Path& path)
    {
        return getApplicationDataPath()/path;
    }

	    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline core::Filesystem::Path getUserApplicationDataPath(const core::Filesystem::Path& path)
    {
        return getUserApplicationDataPath()/path;
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline core::Filesystem::Path getUserDataPath(const core::Filesystem::Path& path)
    {
        return getUserDataPath()/path;
    }

	//! \return Pomocnicza metoda do pobierania ścieżek.
	inline const core::Filesystem::Path & getPluginPath()
	{
		return getPaths()->getPluginPath();
	}

	//! \return Pomocnicza metoda do pobierania ścieżek.
	inline core::Filesystem::Path getPluginPath(const core::Filesystem::Path & path)
	{
		return getPluginPath()/path;
	}

} // namespace core

//! Makro logujące informację testową
#define PLUGIN_LOG_DEBUG(msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->log(core::ILog::LogSeverityDebug, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro logujące błąd
#define PLUGIN_LOG_ERROR(msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->log(core::ILog::LogSeverityError, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro logujące informację
#define PLUGIN_LOG_INFO(msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->log(core::ILog::LogSeverityInfo, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro logujące ostrzeżenia
#define PLUGIN_LOG_WARNING(msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->log(core::ILog::LogSeverityWarning, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)

//! Makro logujące informację testową
#define PLUGIN_LOG_NAMED_DEBUG(name, msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityDebug, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro logujące błąd
#define PLUGIN_LOG_NAMED_ERROR(name, msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityError, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro logujące informację
#define PLUGIN_LOG_NAMED_INFO(name, msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityInfo, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro logujące ostrzeżenia
#define PLUGIN_LOG_NAMED_WARNING(name, msg)	do { std::stringstream tmpMessage; tmpMessage << msg; plugin::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityWarning, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)

#endif  // HEADER_GUARD_CORE__PLUGINCOMMON_H__
