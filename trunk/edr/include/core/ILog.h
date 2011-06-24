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
	//! Poziom wa�no�ci wiadomo�ci.
	enum LogSeverity {
		LogSeverityError,   //B��d
		LogSeverityWarning, //Ostrze�enie
		LogSeverityInfo,    //Informacja
		LogSeverityDebug    //Testowanie
	};

    //! Interfejs do logowania informacji w aplikacji (w konsoli aplikacji)
	class ILog
	{
	public:
        //! Metoda do logowania informacji
        //! \param severity Poziom istotno�ci logowanej informacji
        //! \param message Tre�� logowanej informacji (wraz ze znakami formatuj�cymi)
        //! \param filename Plik �r�d�owy z kt�rego pochodzi logowana informacja
        //! \param lineNumber Numer lini w pliku �r�d�owym, gdzie nastapi�o wywo�anie logowania informacji
		virtual void log(LogSeverity severity, const std::string& message, const std::string& filename, int lineNumber) = 0;
	};
} // namespace core

extern std::stringstream __strstream_temp;
extern std::string __ilog_prefix;

//! Makra u�atwiaj�ce logowanie informacji
//! Makro ustawiaj�ce domy�lny logger informacji
#define DEFINE_DEFAULT_LOGGER(path) std::stringstream __strstream_temp; std::string __ilog_prefix(path);
//! Makro loguj�ce informacj� testow�
#define LOG_DEBUG(msg)   __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityDebug,   __strstream_temp.str(), __FILE__, __LINE__); 
//! Makro loguj�ce b��d
#define LOG_ERROR(msg)   __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityError,   __strstream_temp.str(), __FILE__, __LINE__); 
//! Makro loguj�ce informacj�
#define LOG_INFO(msg)    __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityInfo,    __strstream_temp.str(), __FILE__, __LINE__); 
//! Makro loguj�ce ostrze�enia
#define LOG_WARNING(msg) __strstream_temp.str(""); __strstream_temp << msg; core::getLogInterface()->log(core::LogSeverityWarning, __strstream_temp.str(), __FILE__, __LINE__); 

//TODO
//przywrocic wczesniejsza funkcjonalnosc
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
