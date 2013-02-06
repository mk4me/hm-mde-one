/********************************************************************
	created:	2011/06/21
	created:	21:6:2011   8:44
	filename: 	ILog.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ILOG_H__
#define HEADER_GUARD_CORE__ILOG_H__

#include <corelib/SmartPtr.h>

namespace core
{
	class ILog;
	typedef shared_ptr<ILog> LogPtr;

    //! Interfejs do logowania informacji w aplikacji (w konsoli aplikacji)
	class ILog
	{
	public:
		//! Poziom ważności wiadomości.
		enum LogSeverity {
			LogSeverityError,   //Błąd
			LogSeverityWarning, //Ostrzeżenie
			LogSeverityInfo,    //Informacja
			LogSeverityDebug    //Testowanie
		};

	public:

		virtual ~ILog() {}

        //! Metoda do logowania informacji
        //! \param severity Poziom istotności logowanej informacji
        //! \param message Treść logowanej informacji (wraz ze znakami formatującymi)
		virtual void log(LogSeverity severity, const std::string& message) = 0;
		//! Metoda do logowania informacji
		//! \param severity Poziom istotności logowanej informacji
		//! \param message Treść logowanej informacji (wraz ze znakami formatującymi)
		//! \param fileName Plik źródłowy z którego pochodzi wiadomość
		//! \param lineNo Numer lini w pliku źródłowym która wygenerowała wiadomość
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo) = 0;

		//! \param name Nazwa logera relatywna do nazwy aktualnego logera
		//! \return Nowy loger podpiety pod aktualny
		virtual LogPtr subLog(const std::string & name) const = 0;
	};
} // namespace core

#endif
