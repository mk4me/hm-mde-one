/********************************************************************
    created:  2012/12/18
    created:  18:12:2012   14:09
    filename: LoggerHelpers.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___LOGGERHELPERS_H__
#define HEADER_GUARD___LOGGERHELPERS_H__

#include <dflib/IDFLogger.h>

namespace df {

	//! Implementacje podstawowych helper�w z domy�lnym logowanie na konsol�

	//! Helper loguj�cy dla �r�de�
	class SourceLoggerHelper : public IDFLoggerHelper
	{
	public:
		SourceLoggerHelper(ISourceNode * source, IDFLogger * logger);

		virtual void logMessage(const char * message, IDFLogger::LogSeverity severity);

	private:
		ISourceNode * source;
		IDFLogger * logger;
	};


	//! Helper loguj�cy dla sink�w
	class SinkLoggerHelper : public IDFLoggerHelper
	{
	public:
		SinkLoggerHelper(ISinkNode * sink, IDFLogger * logger);

		virtual void logMessage(const char * message, IDFLogger::LogSeverity severity);

	private:
		ISinkNode * sink;
		IDFLogger * logger;
	};

	//! Helper loguj�cy dla procesor�w
	class ProcessingLoggerHelper : public df::IDFLoggerHelper
	{
	public:
		ProcessingLoggerHelper(IProcessingNode * proc, IDFLogger * logger);

		virtual void logMessage(const char * message, IDFLogger::LogSeverity severity);

	private:
		IProcessingNode * proc;
		IDFLogger * logger;
	};

}

#endif	//	HEADER_GUARD___LOGGERHELPERS_H__
