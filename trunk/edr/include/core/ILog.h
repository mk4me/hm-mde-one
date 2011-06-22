/********************************************************************
	created:	2011/06/21
	created:	21:6:2011   8:44
	filename: 	ILog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ILOG_H__
#define HEADER_GUARD_CORE__ILOG_H__

#include <string>

namespace core 
{
	//! Poziom wa¿noœci wiadomoœci.
	enum LogSeverity {
		LogSeverityError,
		LogSeverityWarning,
		LogSeverityInfo,
		LogSeverityDebug
	};

	class ILog
	{
	public:
		virtual void log(LogSeverity severity, const std::string& message, const std::string& filename, int lineNumber) = 0;
	};
} // namespace core

extern std::stringstream __strstream_temp;
extern std::string __ilog_prefix;
#define DEFINE_DEFAULT_LOGGER(path) std::stringstream __strstream_temp; std::string __ilog_prefix(path);
#define LOG_DEBUG(msg)   __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityDebug,   __strstream_temp.str(), __FILE__, __LINE__); 
#define LOG_ERROR(msg)   __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityError,   __strstream_temp.str(), __FILE__, __LINE__); 
#define LOG_INFO(msg)    __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityInfo,    __strstream_temp.str(), __FILE__, __LINE__); 
#define LOG_WARNING(msg) __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityWarning, __strstream_temp.str(), __FILE__, __LINE__); 

// todo : przywrocic wczesniejsza funkcjonalnosc
#define LOG_DEBUG_NAMED(path, msg)			{ LOG_DEBUG(msg)  }
#define LOG_ERROR_NAMED(path, msg)			{ LOG_ERROR(msg)  }
#define LOG_INFO_NAMED(path, msg)			{ LOG_INFO(msg)   }
#define LOG_WARNING_NAMED(path, msg)		{ LOG_WARNING(msg)}

#define LOG_DEBUG_STATIC_NAMED(path, msg)   { LOG_DEBUG(msg)  }
#define LOG_ERROR_STATIC_NAMED(path, msg)   { LOG_ERROR(msg)  }
#define LOG_INFO_STATIC_NAMED(path, msg)    { LOG_INFO(msg)   }
#define LOG_WARNING_STATIC_NAMED(path, msg) { LOG_WARNING(msg)}

#include <core/PluginCommon.h>

#endif
