#include <loglib/Logger.h>
#include <iostream>

loglib::ILogPtr loglib::Logger::_log = utils::make_shared<loglib::CoutLogger>("utils");


loglib::CoutLogger::CoutLogger(const std::string& prefix) : 
	prefix(prefix)
{

}



void loglib::CoutLogger::log(LogSeverity severity, const std::string& message)
{
	std::string begining = prefix.empty() ? "" : prefix + ": ";
	switch (severity) {
	case ILog::Error:
		std::cerr << begining << message << std::endl;
		break;

	default:
		std::cout << begining << message << std::endl;
	}
}

void loglib::CoutLogger::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	std::stringstream ss;
	ss << fileName << ":" << funcName << "(" << lineNo << "): " << message;
	log(severity, ss.str());
}

loglib::ILogPtr loglib::CoutLogger::subLog(const std::string & name) const
{
	return utils::make_shared<CoutLogger>(name);
}
