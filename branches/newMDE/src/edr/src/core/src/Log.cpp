#include "CorePCH.h"
#include "Log.h"

#include "Config.h"

#ifndef CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------
#ifdef CORE_ENABLE_LOG4CXX

#include <log4cxx/logger.h>

class LogImpl : public core::ILog
{
public:

	LogImpl() : logger(log4cxx::Logger::getRootLogger()) {}

private:

	LogImpl(const std::string & name) : name(name), logger(log4cxx::Logger::getLogger(name)) {}

public:

	virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
	{
		log4cxx::spi::LocationInfo location(fileName.c_str(), funcName.c_str(), lineNo);
		switch(severity)
		{
		case LogSeverityDebug:
			logger->debug(message, location);
			break;

		case LogSeverityError:
			logger->error(message, location);
			break;

		case LogSeverityInfo:
			logger->info(message, location);
			break;

		case LogSeverityWarning:
			logger->warn(message, location);
			break;

		default:
			UTILS_ASSERT((false), "Nieznany poziom logowania");
			break;
		}
	}

	virtual void log(LogSeverity severity, const std::string& message)
	{
		switch(severity)
		{
		case LogSeverityDebug:
			logger->debug(message);
			break;

		case LogSeverityError:
			logger->error(message);
			break;

		case LogSeverityInfo:
			logger->info(message);
			break;

		case LogSeverityWarning:
			logger->warn(message);
			break;

		default:
			UTILS_ASSERT((false), "Nieznany poziom logowania");
			break;
		}		
	}

	virtual core::LogPtr subLog(const std::string & name) const
	{
		return core::LogPtr(new LogImpl(this->name.empty() == true ? name : this->name + "." + name));
	}

private:
	log4cxx::LoggerPtr logger;
	const std::string name;
};


#else // CORE_ENABLE_LOG4CXX

#include <osg/Notify>

class LogImpl : public core::ILog
{
public:

	LogImpl() : name("root"), prefix("[root]->") {}

private:

	LogImpl(const std::string & name) : name(name), prefix("[" + name + "]->") {}

public:

	virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
	{
		std::stringstream msg;
		msg << "[file: " << fileName << "][function: " << funcName << "][line: " <<lineNo << "]->" << message;
		log(severity, msg.str());
	}

	virtual void log(LogSeverity severity, const std::string& message)
	{
		switch(severity)
		{
		case LogSeverityDebug:
			OSG_DEBUG << prefix << message << std::endl;
			break;

		case LogSeverityError:
			OSG_FATAL << prefix << message << std::endl;
			break;

		case LogSeverityInfo:
			OSG_NOTICE << prefix << message << std::endl;
			break;

		case LogSeverityWarning:
			OSG_WARN << prefix << message << std::endl;
			break;

		default:
			UTILS_ASSERT((false), "Nieznany poziom logowania");
			break;
		}		
	}

	virtual core::LogPtr subLog(const std::string & name) const
	{
		return core::LogPtr(new LogImpl(this->name.empty() == true ? name : this->name + "." + name));
	}

private:
	const std::string name;
	const std::string prefix;
};

#endif // CORE_ENABLE_LOG4CXX

namespace core {

Log::Log() : logImpl(new LogImpl)
{

}

Log::~Log()
{

}

void Log::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	logImpl->log(severity, message, funcName, fileName, lineNo);
}

void Log::log(LogSeverity severity, const std::string& message)
{
	logImpl->log(severity, message);
}

core::LogPtr Log::subLog(const std::string & name) const
{
	return logImpl->subLog(name);
}

}

//------------------------------------------------------------------------------
#else  // CORE_DISABLE_LOGGING

namespace core {

Log::Log() {}

void Log::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
		
}

void Log::log(LogSeverity severity, const std::string& message)
{
		
}

core::LogPtr Log::subLog(const std::string & name) const
{
	static const core::LogPtr ret = core::LogPtr(new Log);
	return ret;
}

}

#endif  // CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------


