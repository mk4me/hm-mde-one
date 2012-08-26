/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   10:28
	filename: Log.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LOG_H__
#define __HEADER_GUARD__CORE_LOG_H__


// tymczasowe rozwiązanie: używamy strumieni OSG
#include <core/Config.h>
#include <string>

#include <core/ILog.h>
#ifndef CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------
#ifdef CORE_ENABLE_LOG4CXX

#include <log4cxx/logger.h>
#include <core/PluginCommon.h>
//! Tutaj dzieje się magia. Binarka kliencka załączając corelib (bibliotekę statyczną)
//! musi gdzieś stworzyć tę zmienną, bo inaczej będą błędy linkera.
extern log4cxx::LoggerPtr __logger;

#define __LOG_STATIC_HELPER(operation, path, msg) do {                              \
    static log4cxx::LoggerPtr __staticLogger = log4cxx::Logger::getLogger(path);    \
    operation(__staticLogger, msg);                                                 \
    } while (0)


#else // CORE_ENABLE_LOG4CXX
//
//#include <osg/Notify>
//#include <string>
//
////! Prefiks logowania w binarce.
//extern std::string __loggerPath;
////! Definicja prefiksu logowania w binarce.
//#define DEFINE_DEFAULT_LOGGER(path) std::string __loggerPath(path);
//
//#define LOG_DEBUG(msg)    OSG_DEBUG<<__loggerPath<<": "<<msg<<std::endl
//#define LOG_ERROR(msg)    OSG_FATAL<<__loggerPath<<": "<<msg<<std::endl
//#define LOG_INFO(msg)     OSG_NOTICE<<__loggerPath<<": "<<msg<<std::endl
//#define LOG_WARNING(msg)  OSG_WARN<<__loggerPath<<": "<<msg<<std::endl
//
//// Makra do logowania z użyciem loggera o zadanej ścieżce
//#define LOG_DEBUG_NAMED(path, msg)      OSG_DEBUG<<path<<": "<<msg<<std::endl
//#define LOG_ERROR_NAMED(path, msg)      OSG_FATAL<<path<<": "<<msg<<std::endl
//#define LOG_INFO_NAMED(path, msg)       OSG_NOTICE<<path<<": "<<msg<<std::endl
//#define LOG_WARNING_NAMED(path, msg)    OSG_WARN<<path<<": "<<msg<<std::endl
//
//// Zoptymalizowane makra do logowania z użyciem loggera o zadanej ścieże. Parametr "path" musi być
//// zawsze stały.
//#define LOG_DEBUG_STATIC_NAMED(path, msg)   LOG_DEBUG_NAMED(path, msg)
//#define LOG_ERROR_STATIC_NAMED(path, msg)   LOG_ERROR_NAMED(path, msg)
//#define LOG_INFO_STATIC_NAMED(path, msg)    LOG_INFO_NAMED(path, msg)
//#define LOG_WARNING_STATIC_NAMED(path, msg) LOG_WARNING_NAMED(path, msg)

#endif // CORE_ENABLE_LOG4CXX
//------------------------------------------------------------------------------
#else  // CORE_DISABLE_LOGGING

#define DEFINE_DEFAULT_LOGGER(path)

#define LOG_DEBUG(msg)    
#define LOG_ERROR(msg)    
#define LOG_INFO(msg)     
#define LOG_WARNING(msg)  
#define LOG_DEBUG_NAMED(path, msg)   
#define LOG_ERROR_NAMED(path, msg)   
#define LOG_INFO_NAMED(path, msg)    
#define LOG_WARNING_NAMED(path, msg) 
#define LOG_DEBUG_STATIC_NAMED(path, msg)  
#define LOG_ERROR_STATIC_NAMED(path, msg)  
#define LOG_INFO_STATIC_NAMED(path, msg)   
#define LOG_WARNING_STATIC_NAMED(path, msg)

#endif  // CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------

#endif  // __HEADER_GUARD__CORE_LOG_H__
