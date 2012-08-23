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
#include <sstream>

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
		virtual ~ILog() {}
	};
} // namespace core

extern std::stringstream __strstream_temp;
extern std::string __ilog_prefix;

//! Makra u�atwiaj�ce logowanie informacji
//! Makro ustawiaj�ce domy�lny logger informacji
#define DEFINE_DEFAULT_LOGGER(path) std::stringstream __strstream_temp; std::string __ilog_prefix(path);
//! Makro loguj�ce informacj� testow�
#define LOG_DEBUG(msg)   do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityDebug,   __strstream_temp.str(), __FILE__, __LINE__); } while (0)
//! Makro loguj�ce b��d
#define LOG_ERROR(msg)	 do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityError,   __strstream_temp.str(), __FILE__, __LINE__); } while (0)
//! Makro loguj�ce informacj�
#define LOG_INFO(msg)    do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityInfo,    __strstream_temp.str(), __FILE__, __LINE__); } while (0)
//! Makro loguj�ce ostrze�enia
#define LOG_WARNING(msg) do { __strstream_temp.str(""); __strstream_temp << "[" << __ilog_prefix << "] " << msg; core::getLogInterface()->log(core::LogSeverityWarning, __strstream_temp.str(), __FILE__, __LINE__); } while (0)

//TODO
//przywrocic wczesniejsza funkcjonalno��
#define LOG_DEBUG_NAMED(path, msg)			do { LOG_DEBUG(msg)   ;} while(0)
#define LOG_ERROR_NAMED(path, msg)			do { LOG_ERROR(msg)   ;} while(0)
#define LOG_INFO_NAMED(path, msg)			do { LOG_INFO(msg)    ;} while(0)
#define LOG_WARNING_NAMED(path, msg)		do { LOG_WARNING(msg) ;} while(0)

#define LOG_DEBUG_STATIC_NAMED(path, msg)   do { LOG_DEBUG(msg)   ;} while(0)
#define LOG_ERROR_STATIC_NAMED(path, msg)   do { LOG_ERROR(msg)   ;} while(0)
#define LOG_INFO_STATIC_NAMED(path, msg)    do { LOG_INFO(msg)    ;} while(0)
#define LOG_WARNING_STATIC_NAMED(path, msg) do { LOG_WARNING(msg) ;} while(0)

#endif
