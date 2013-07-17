/********************************************************************
    created:  2013/01/10
    created:  10:1:2013   8:54
    filename: ApplicationCommon.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___APPLICATIONCOMMON_H__
#define HEADER_GUARD___APPLICATIONCOMMON_H__

#include <string>
#include "Log.h"
#include "Path.h"
#include <corelib/Filesystem.h>
#include "Application.h"
#include <utils/Macros.h>

namespace core {

//! Zmienna defininowana przez makro tworz�ce pluginy. Tylko do u�ytku wewn�trznego.
extern Application * __application;

//! Makro definiuj�ce zmienn� przechowuj�c� managery. Automatycznie u�ywane w pluginach.
#define DEFINE_CORE_APPLICATION_ACCESSOR namespace core { Application * __application = nullptr; }

//! \return Bie��ca instancja data managera readera. Rozwi�zanie w ten spos�b, w stosunku do
//! klasycznego upublicznienia tylko nag��wk�w funkcji i schowania definicji, pozwala
//! na rozwijanie, wi�c jest potencjalnie szybsze.
inline MemoryDataManager* getMemoryDataManager()
{
	return __application->memoryDataManager();
}

inline StreamDataManager* getStreamDataManager()
{
	return __application->streamDataManager();
}

inline FileDataManager* getFileDataManager()
{
	return __application->fileDataManager();
}

inline DataHierarchyManager* getDataHierarchyManager()
{
	return __application->dataHierarchyManager();
}

inline ParserManager* getParserManager()
{
	return __application->parserManager();
}

inline ServiceManager* getServiceManager()
{
	return __application->serviceManager();
}

inline SourceManager* getSourceManager()
{
	return __application->sourceManager();
}

inline VisualizerManager* getVisualizerManager()
{
	return __application->visualizerManager();
}

inline ThreadPool* getThreadPool()
{
	return __application->threadPool();
}

inline JobManager* getJobManager()
{
	return __application->jobManager();
}

//! \return Interfejs dost�pu do �cie�ek aplikacji
inline Path* getPathInterface()
{
	return __application->paths();
}

//! \return Interfejs logowania informacji
inline ILog* getLogInterface()
{
	return __application->logger();
}

//! \return Interfejs logowania informacji
inline ILog* getPrototypeLogInterface()
{
	return __application->loggerPrototype();
}

//! Pomocnica metoda upraszczaj�ca odwo�anie do katalog�w.
inline const Filesystem::Path& getResourcesPath()
{
	return getPathInterface()->getResourcesPath();
}

//! Pomocnica metoda upraszczaj�ca odwo�anie do katalog�w.
inline const Filesystem::Path& getApplicationDataPath()
{
	return getPathInterface()->getApplicationDataPath();
}

//! Pomocnica metoda upraszczaj�ca odwo�anie do katalog�w.
inline const Filesystem::Path& getUserDataPath()
{
	return getPathInterface()->getUserDataPath();
}

//! \return Pomocnicza metoda do pobierania �cie�ek.
inline Filesystem::Path getResourcePath(const Filesystem::Path& path)
{
	return getResourcesPath()/path;
}

//! \return Pomocnicza metoda do pobierania �cie�ek.
inline Filesystem::Path getApplicationDataPath(const Filesystem::Path& path)
{
	return getApplicationDataPath()/path;
}

//! \return Pomocnicza metoda do pobierania �cie�ek.
inline Filesystem::Path getUserDataPath(const Filesystem::Path& path)
{
	return getUserDataPath()/path;
}

//! \return Pomocnicza metoda do pobierania �cie�ek.
inline const Filesystem::Path & getPluginPath()
{
	return getPathInterface()->getPluginPath();
}

//! \return Pomocnicza metoda do pobierania �cie�ek.
inline core::Filesystem::Path getPluginPath(const core::Filesystem::Path & path)
{
	return getPluginPath()/path;
}

}


//! Makro loguj�ce informacj� testow�
#define CORE_LOG_DEBUG(msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->log(core::ILog::LogSeverityDebug, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce b��d
#define CORE_LOG_ERROR(msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->log(core::ILog::LogSeverityError, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce informacj�
#define CORE_LOG_INFO(msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->log(core::ILog::LogSeverityInfo, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce ostrze�enia
#define CORE_LOG_WARNING(msg)	do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->log(core::ILog::LogSeverityWarning, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)

//! Makro loguj�ce informacj� testow�
#define CORE_LOG_NAMED_DEBUG(name, msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityDebug, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce b��d
#define CORE_LOG_NAMED_ERROR(name, msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityError, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce informacj�
#define CORE_LOG_NAMED_INFO(name, msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityInfo, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce ostrze�enia
#define CORE_LOG_NAMED_WARNING(name, msg)	do { std::stringstream tmpMessage; tmpMessage << msg; core::getLogInterface()->subLog(name)->log(core::ILog::LogSeverityWarning, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)

//! Makro loguj�ce informacj� testow�
#define LOG_NAMED_DEBUG(name, msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getPrototypeLogInterface()->subLog(name)->log(core::ILog::LogSeverityDebug, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce b��d
#define LOG_NAMED_ERROR(name, msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getPrototypeLogInterface()->subLog(name)->log(core::ILog::LogSeverityError, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce informacj�
#define LOG_NAMED_INFO(name, msg)		do { std::stringstream tmpMessage; tmpMessage << msg; core::getPrototypeLogInterface()->subLog(name)->log(core::ILog::LogSeverityInfo, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)
//! Makro loguj�ce ostrze�enia
#define LOG_NAMED_WARNING(name, msg)	do { std::stringstream tmpMessage; tmpMessage << msg; core::getPrototypeLogInterface()->subLog(name)->log(core::ILog::LogSeverityWarning, tmpMessage.str(), __UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)

#endif	//	HEADER_GUARD___APPLICATIONCOMMON_H__
