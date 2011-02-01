/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   10:28
	filename: Log.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LOG_H__
#define __HEADER_GUARD__CORE_LOG_H__

// tymczasowe rozwi�zanie: u�ywamy strumieni OSG
#include <core/Config.h>
#include <osg/Notify>
#include <utils/Enum.h>
#include <utils/Utils.h>

class ConsoleWidget;

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Obiekt logu. Powinien by� tworzony w g��wnej binarce na samym pocz�tku maina.
class LogInitializer 
{
public:
    //! \param configPath �cie�ka do pliku konfiguracyjnego.
    LogInitializer(const char* configPath);
    //!
    ~LogInitializer();

public:
    //! \param console Widget konsoli.
    void setConsoleWidget(ConsoleWidget* widget);

private:
    /**
     *	Prze�adowania operator�w new-delete. Prywatne, aby niemo�liwe by�o
     *  tworzenie instancji typu na stercie.
     */     
    void *operator new(size_t bytes);
    //! \see LogInitializer::operator new
    void *operator new[](size_t bytes);
    //! \see LogInitializer::operator new
    void operator delete(void* p);
    //! \see LogInitializer::operator new
    void operator delete[](void* p);
};

//! Poziom wa�no�ci wiadomo�ci.
enum LogSeverity {
    LogSeverityError,
    LogSeverityWarning,
    LogSeverityInfo,
    LogSeverityDebug
};


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#ifdef CORE_ENABLE_LOG4CXX

#include <log4cxx/logger.h>
#include <utils/Utils.h>

//! Tutaj dzieje si� magia. Binarka kliencka za��czaj�c corelib (bibliotek� statyczn�)
//! musi gdzie� stworzy� t� zmienn�, bo inaczej b�d� b��dy linkera.
extern log4cxx::LoggerPtr __logger;

//! Makro tworz�ce instancj� loggera dla zadanej �cie�ki.
#define DEFINE_DEFAULT_LOGGER(path) log4cxx::LoggerPtr __logger(log4cxx::Logger::getLogger(path));

// Makra s�u��ce do logowania z u�yciem domy�lnego loggera.
#define LOG_DEBUG(msg)   LOG4CXX_DEBUG(__logger, msg)
#define LOG_ERROR(msg)   LOG4CXX_ERROR(__logger, msg)
#define LOG_INFO(msg)    LOG4CXX_INFO(__logger, msg)
#define LOG_WARNING(msg) LOG4CXX_WARN(__logger, msg)

// Makra do logowania z u�yciem loggera o zadanej �cie�ce
#define LOG_DEBUG_NAMED(path, msg)      LOG4CXX_DEBUG(log4cxx::Logger::getLogger(path), msg)
#define LOG_ERROR_NAMED(path, msg)      LOG4CXX_ERROR(log4cxx::Logger::getLogger(path), msg)
#define LOG_INFO_NAMED(path, msg)       LOG4CXX_INFO(log4cxx::Logger::getLogger(path), msg)
#define LOG_WARNING_NAMED(path, msg)    LOG4CXX_WARN(log4cxx::Logger::getLogger(path), msg)

// Zoptymalizowane makra do logowania z u�yciem loggera o zadanej �cie�e. Parametr "path" musi by�
// zawsze sta�y.
#define LOG_DEBUG_STATIC_NAMED(path, msg)   __LOG_STATIC_HELPER(LOG4CXX_DEBUG, path, msg)
#define LOG_ERROR_STATIC_NAMED(path, msg)   __LOG_STATIC_HELPER(LOG4CXX_ERROR, path, msg)
#define LOG_INFO_STATIC_NAMED(path, msg)    __LOG_STATIC_HELPER(LOG4CXX_INFO, path, msg)
#define LOG_WARNING_STATIC_NAMED(path, msg) __LOG_STATIC_HELPER(LOG4CXX_WARN, path, msg)

#define __LOG_STATIC_HELPER(operation, path, msg) UTILS_MULTISTATEMENT_BEGIN        \
    static log4cxx::LoggerPtr __staticLogger = log4cxx::Logger::getLogger(path);    \
    operation(__staticLogger, msg);                                                 \
    UTILS_MULTISTATEMENT_END

#else // CORE_ENABLE_LOG4CXX

#include <string>

//! Prefiks logowania w binarce.
extern std::string __loggerPath;
//! Definicja prefiksu logowania w binarce.
#define DEFINE_DEFAULT_LOGGER(path) std::string __loggerPath(path);

#define LOG_DEBUG(msg)    OSG_DEBUG<<__loggerPath<<": "<<msg<<std::endl
#define LOG_ERROR(msg)    OSG_FATAL<<__loggerPath<<": "<<msg<<std::endl
#define LOG_INFO(msg)     OSG_NOTICE<<__loggerPath<<": "<<msg<<std::endl
#define LOG_WARNING(msg)  OSG_WARN<<__loggerPath<<": "<<msg<<std::endl

// Makra do logowania z u�yciem loggera o zadanej �cie�ce
#define LOG_DEBUG_NAMED(path, msg)      OSG_DEBUG<<path<<": "<<msg<<std::endl
#define LOG_ERROR_NAMED(path, msg)      OSG_FATAL<<path<<": "<<msg<<std::endl
#define LOG_INFO_NAMED(path, msg)       OSG_NOTICE<<path<<": "<<msg<<std::endl
#define LOG_WARNING_NAMED(path, msg)    OSG_WARN<<path<<": "<<msg<<std::endl

// Zoptymalizowane makra do logowania z u�yciem loggera o zadanej �cie�e. Parametr "path" musi by�
// zawsze sta�y.
#define LOG_DEBUG_STATIC_NAMED(path, msg)   LOG_DEBUG_NAMED(path, msg)
#define LOG_ERROR_STATIC_NAMED(path, msg)   LOG_ERROR_NAMED(path, msg)
#define LOG_INFO_STATIC_NAMED(path, msg)    LOG_INFO_NAMED(path, msg)
#define LOG_WARNING_STATIC_NAMED(path, msg) LOG_WARNING_NAMED(path, msg)

#endif  // CORE_ENABLE_LOG4CXX



#endif  // __HEADER_GUARD__CORE_LOG_H__