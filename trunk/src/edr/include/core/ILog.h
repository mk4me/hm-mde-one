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
		LogSeverityError,   //B³¹d
		LogSeverityWarning, //Ostrze¿enie
		LogSeverityInfo,    //Informacja
		LogSeverityDebug    //Testowanie
	};

    //! Interfejs do logowania informacji w aplikacji (w konsoli aplikacji)
	class ILog
	{
	public:
        //! Metoda do logowania informacji
        //! \param severity Poziom istotnoœci logowanej informacji
        //! \param message Treœæ logowanej informacji (wraz ze znakami formatuj¹cymi)
        //! \param filename Plik Ÿród³owy z którego pochodzi logowana informacja
        //! \param lineNumber Numer lini w pliku Ÿród³owym, gdzie nastapi³o wywo³anie logowania informacji
		virtual void log(LogSeverity severity, const std::string& message, const std::string& filename, int lineNumber) = 0;
		virtual ~ILog() {}
	};
} // namespace core

extern std::stringstream __strstream_temp;
extern std::string __ilog_prefix;

//! Makra u³atwiaj¹ce logowanie informacji
//! Makro ustawiaj¹ce domyœlny logger informacji
#define DEFINE_DEFAULT_LOGGER(path) std::stringstream __strstream_temp; std::string __ilog_prefix(path);
//! Makro loguj¹ce informacjê testow¹
#define LOG_DEBUG(msg)   do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityDebug,   __strstream_temp.str(), __FILE__, __LINE__); } while (0)
//! Makro loguj¹ce b³¹d												  	   
#define LOG_ERROR(msg)	 do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityError,   __strstream_temp.str(), __FILE__, __LINE__); } while (0)
//! Makro loguj¹ce informacjê										  	   
#define LOG_INFO(msg)    do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityInfo,    __strstream_temp.str(), __FILE__, __LINE__); } while (0)
//! Makro loguj¹ce ostrze¿enia										   	   
#define LOG_WARNING(msg) do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityWarning, __strstream_temp.str(), __FILE__, __LINE__); } while (0)

//TODO
//przywrocic wczesniejsza funkcjonalnosc
#define LOG_DEBUG_NAMED(path, msg)			do { LOG_DEBUG(msg)   ;} while(0)
#define LOG_ERROR_NAMED(path, msg)			do { LOG_ERROR(msg)   ;} while(0)
#define LOG_INFO_NAMED(path, msg)			do { LOG_INFO(msg)    ;} while(0)
#define LOG_WARNING_NAMED(path, msg)		do { LOG_WARNING(msg) ;} while(0)
			
#define LOG_DEBUG_STATIC_NAMED(path, msg)   do { LOG_DEBUG(msg)   ;} while(0)
#define LOG_ERROR_STATIC_NAMED(path, msg)   do { LOG_ERROR(msg)   ;} while(0)
#define LOG_INFO_STATIC_NAMED(path, msg)    do { LOG_INFO(msg)    ;} while(0)
#define LOG_WARNING_STATIC_NAMED(path, msg) do { LOG_WARNING(msg) ;} while(0)

#include <core/PluginCommon.h>

#endif
