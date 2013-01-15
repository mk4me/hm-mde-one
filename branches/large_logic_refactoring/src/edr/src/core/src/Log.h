/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   10:28
	filename: Log.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LOG_H__
#define __HEADER_GUARD__CORE_LOG_H__

#include <string>
#include <core/ILog.h>

namespace core {

class Log : public core::ILog
{
public:

	Log();
	virtual ~Log();

public:

	virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo);

	virtual void log(LogSeverity severity, const std::string& message);

	virtual core::LogPtr subLog(const std::string & name);

private:
	core::LogPtr logImpl;
};

}

#endif  // __HEADER_GUARD__CORE_LOG_H__
