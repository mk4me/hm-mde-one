/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:49
    filename: IDFLogger.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDFLOGGER_H__
#define HEADER_GUARD___IDFLOGGER_H__

namespace df {

	//! Forward declarations
	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;
	class IInputPin;
	class IOutputPin;
	class IModelReader;

	//! Interfejs loguj�cy przebieg przetwarzania danych
	class IDFLogger
	{
	public:
		//! Poziom istotno�ci komunikatow�
		enum LogSeverity
		{
			DFLOG_INFO,		//! Wiadomo��
			DFLOG_DEBUG,	//! Diagnostyczne
			DFLOG_ERROR,	//! B��d
			DFLOG_WARNING	//! Ostrze�enie
		};

	public:
		//! Loguje info �r�d�a danych
		//! \param node W�ze� �r�d�owy
		//! \param message Komunikat
		//! \param severity Poziom istotno�ci
		virtual void logMessage(ISourceNode * source, const char * message, LogSeverity severity) = 0;

		//! Loguje info odbiornika danych
		//! \param node W�ze� odbiornika
		//! \param message Komunikat
		//! \param severity Poziom istotno�ci
		virtual void logMessage(ISinkNode * sink, const char * message, LogSeverity severity) = 0;

		//! Loguje info procesora danych
		//! \param node Procesor danych
		//! \param message Komunikat
		//! \param severity Poziom istotno�ci
		virtual void logMessage(IProcessingNode * processor, const char * message, LogSeverity severity) = 0;

		//! Loguje info modelu
		//! \param model Model danych
		//! \param message Komunikat
		//! \param severity Poziom istotno�ci
		virtual void logMessage(IModelReader * model, const char * message, LogSeverity severity) = 0;

		//! Loguje info pinu wejsciowego
		//! \param pin Pin wej�ciowy
		//! \param message Komunikat
		//! \param severity Poziom istotno�ci
		virtual void logMessage(IInputPin * inputPin, const char * message, LogSeverity severity) = 0;

		//! Loguje info pinu wyjsciowego
		//! \param pin Pin wyj�ciowy
		//! \param message Komunikat
		//! \param severity Poziom istotno�ci
		virtual void logMessage(IOutputPin * inputPin, const char * message, LogSeverity severity) = 0;

	};

	//! Interfejs pomocniczy przy logowaniu
	class IDFLoggerHelper
	{
	public:
		//! Logowanie wiadomo�ci
		//! \param message Wiadomo��
		//! severity Poziom istotno�ci
		virtual void logMessage(const char * message, IDFLogger::LogSeverity severity) = 0;
	};
}

#endif	//	HEADER_GUARD___IDFLOGGER_H__
