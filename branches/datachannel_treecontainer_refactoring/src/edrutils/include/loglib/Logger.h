/********************************************************************
	created:	2015/07/21
	created:	11:54
	filename: 	Logger.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_LOGLIB__Logger_H__
#define HEADER_GUARD_LOGLIB__Logger_H__

#include <loglib/Export.h>
#include <loglib/ILog.h>
namespace  loglib {
	
	struct LOGLIB_EXPORT Logger
	{
	public:
		//! Ustawienie loga
		static void setLog(ILogPtr log) {
			_log = log;
		}

		static ILogPtr getLog()
		{
			return _log;
		}
	protected:
		//! globalny log 
		static ILogPtr _log;
	};

	class LOGLIB_EXPORT CoutLogger : public ILog
	{
	public:
		CoutLogger() {}
		CoutLogger(const std::string& prefix);
		virtual ~CoutLogger() {}
		virtual void log(LogSeverity severity, const std::string& message);
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo);
		virtual ILogPtr subLog(const std::string & name) const;
	private:
		std::string prefix;
	};
}

#define UTILS_LOG_DEBUG(msg)	   LOG_DEBUG(loglib::Logger::getLog(), msg)	
//! Makro loguj¹ce b³¹d					   
#define UTILS_LOG_ERROR(msg)	   LOG_ERROR(loglib::Logger::getLog(), msg)	
//! Makro loguj¹ce informacjê			   
#define UTILS_LOG_INFO(msg)		   LOG_INFO(loglib::Logger::getLog(), msg)	
//! Makro loguj¹ce ostrze¿enia			   
#define UTILS_LOG_WARNING(msg)	   LOG_WARNING(loglib::Logger::getLog(), msg)

#endif // HEADER_GUARD 
