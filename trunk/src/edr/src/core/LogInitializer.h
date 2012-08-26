/********************************************************************
    created:  2011/04/12
    created:  12:4:2011   18:15
    filename: LogInitializer.h
    author:	  Piotr Gwiazdowski

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LOGINITIALIZER_H__
#define HEADER_GUARD_CORE__LOGINITIALIZER_H__

//#include"Log.h"
#include <cstddef>
#include <core/ILog.h>
#include <log4cxx/logger.h>

class EDRConsoleWidget;

//! Obiekt logu. Powinien być tworzony w głównej binarce na samym początku maina.
class LogInitializer
{
public:
    //! \param configPath Ścieżka do pliku konfiguracyjnego.
    LogInitializer(const char* configPath);
    //!
    ~LogInitializer();

public:
    //! \param console Widget konsoli.
    //void setConsoleWidget(ConsoleWidget* widget);
    void setConsoleWidget(EDRConsoleWidget* widget);

private:
    /**
     *	Przeładowania operatorów new-delete. Prywatne, aby niemożliwe było
     *  tworzenie instancji typu na stercie.
     */
    void *operator new(std::size_t bytes);
    //! \see LogInitializer::operator new
    void *operator new[](std::size_t bytes);
    //! \see LogInitializer::operator new
    void operator delete(void* p);
    //! \see LogInitializer::operator new
    void operator delete[](void* p);
};


class EDRLog : public core::ILog
{
private:
	LogInitializer initializer;
	std::string path;
public:
	EDRLog(const std::string& path) :
	  initializer(path.c_str()),
	  path(path)
	  {
	  }

	virtual void log( core::LogSeverity severity, const std::string& message, const std::string& filename, int lineNumber )
	{
		std::string msg(message);
	/*	msg += " (";
		msg += filename;
		msg += ":";
		msg += lineNumber;
		msg += ")";*/

		switch(severity)
		{
			case core::LogSeverityDebug: LOG4CXX_DEBUG(log4cxx::Logger::getLogger(path.c_str()), msg); break;
			case core::LogSeverityError: LOG4CXX_ERROR(log4cxx::Logger::getLogger(path.c_str()), msg); break;
			case core::LogSeverityInfo: LOG4CXX_INFO(log4cxx::Logger::getLogger(path.c_str()), msg); break;
			case core::LogSeverityWarning: LOG4CXX_WARN(log4cxx::Logger::getLogger(path.c_str()), msg); break;
		}
	}

	void setConsoleWidget(EDRConsoleWidget* widget)
	{
		initializer.setConsoleWidget(widget);
	}
};
#endif  // HEADER_GUARD_CORE__LOGINITIALIZER_H__
