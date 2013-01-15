#include "CorePCH.h"
#include "LogInitializer.h"
#include <QtCore/QThread>
#include <osg/Notify>
#include <boost/algorithm/string.hpp>
#include <utils/Debug.h>
#include "EDRConsoleWidget.h"

using namespace core;

#ifndef CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------
#ifdef CORE_ENABLE_LOG4CXX

#include <boost/foreach.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/synchronized.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/helpers/transcoder.h>
#include <queue>

using namespace log4cxx;

void QtMessageHandler(QtMsgType type, const char *msg)
{
    switch (type) {
        case QtDebugMsg:
            LOG4CXX_DEBUG("qt", msg);
            break;
        case QtWarningMsg:
            LOG4CXX_WARN("qt", msg);
            break;
        default:
            // pozostałe poziomy komunikatu są już tożsame errorowi
            LOG4CXX_ERROR("qt", msg);
            break;
    }
}

/** Strumień przekierowywujący logowania do naszego logera */
class OsgNotifyHandlerLog4cxx : public osg::NotifyHandler
{
public:	
	  //! \param severity
	  //! \param message
	  virtual void notify(osg::NotifySeverity severity, const char *message)
	  {
		  // z wiadomości usuwamy zbędne znaki (osg zawsze na koniec daje nową linię)
		  std::string msg(message);
		  boost::trim(msg);
		  switch (severity) {
		  case osg::FATAL:
			  LOG4CXX_ERROR("osg", message);
			  break;
		  case osg::WARN:
			  LOG4CXX_WARN("osg", message);
			  break;
		  case osg::ALWAYS:
		  case osg::NOTICE:
		  case osg::INFO:
			  LOG4CXX_INFO("osg", message);
			  break;
		  case osg::DEBUG_INFO:
		  case osg::DEBUG_FP:
			  LOG4CXX_DEBUG("osg", message);
			  break;
		  default:
			  UTILS_ASSERT(false, "Nieznany poziom osg. Wiadomość: %s", message);
			  break;
		  }
	  }
};

class ConsoleWidgetAppender : public AppenderSkeleton
{
private:
    //! Konsola właściwa.
    EDRConsoleWidget* console;
    //! Kolejka wiadomości dla konsoli. Używana, gdy pojawiają się zdarzenia logowania,
    //! a konsoli jeszcze nie ma (inicjalizacja).
    std::queue<EDRConsoleWidgetEntryPtr> queuedEntries;

public:
    DECLARE_LOG4CXX_OBJECT(ConsoleWidgetAppender)
    BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(ConsoleWidgetAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
    END_LOG4CXX_CAST_MAP()

public:
    //! Zerujący konstruktor
    ConsoleWidgetAppender() :
    console(nullptr)
    {}
    //!
    virtual ~ConsoleWidgetAppender()
    {
        UTILS_ASSERT(console == nullptr, "Wskaźnik powinien zostać jawnie wyzerowany metodą setConsole!");
    }

public:
    //! \param console Widget konsoli.
    void setConsole(EDRConsoleWidget* console)
    {
        // korzytamy z muteksa z klasy bazowej !
        helpers::synchronized sync(mutex);
        this->console = console;
        // opróżniamy kolejkę komunikatów
        if ( console ) {
            for ( ; !queuedEntries.empty(); queuedEntries.pop() ) {
                console->logOrQueueEntry(queuedEntries.front());
            }
        }
    }

// Appender
public:
    virtual void close()
	{

	}

    virtual bool requiresLayout() const
    {
        return true;
    }

// AppenderSkeleton
protected:
    virtual void append(const spi::LoggingEventPtr& event, helpers::Pool& p)
    {
        LogString buf;
        layout->format(buf, event, p);

        EDRConsoleWidgetEntryPtr entry(new EDRConsoleWidgetEntry());
        int level = event->getLevel()->toInt();
        if ( level <= Level::DEBUG_INT ) {
            entry->severity = core::LogSeverityDebug;
        } else if ( level <= Level::INFO_INT ) {
            entry->severity = core::LogSeverityInfo;
        } else if ( level <= Level::WARN_INT ) {
            entry->severity = core::LogSeverityWarning;
        } else {
            entry->severity = core::LogSeverityError;
        }

        // rev - jakie flagi/ustawienia powoduja roznice? trzeba znalezc uniwersalne rozwiazanie
        // jeśli Utf16 jest ustawiane w konfiguracji, to nie można tego tak zahardcodować

        #ifdef __WIN32__
			#if LOG4CXX_LOGCHAR_IS_WCHAR
				entry->message = QString::fromUtf16( reinterpret_cast<const ushort*>(buf.c_str()) );
			#endif

			#if LOG4CXX_LOGCHAR_IS_UTF8
				entry->message = QString::fromUtf8( reinterpret_cast<const ushort*>(buf.c_str()) );
			#endif
			
			#if LOG4CXX_LOGCHAR_IS_UNICHAR
				entry->message = QString::fromStdString(buf);
			#endif
            
        #else
			
			#if LOG4CXX_LOGCHAR_IS_WCHAR
				#if WCHAR_MAX > 0xFFFFu
					entry->message = QString::fromUtf16( reinterpret_cast<const ushort*>(buf.c_str()) );
				#else
					entry->message = QString::fromUtf8( reinterpret_cast<const ushort*>(buf.c_str()) );
				#endif
			#endif

			#if LOG4CXX_LOGCHAR_IS_UTF8
				entry->message = QString::fromUtf8( reinterpret_cast<const ushort*>(buf.c_str()) );
			#endif

			#if LOG4CXX_LOGCHAR_IS_UNICHAR
				entry->message = QString::fromStdString(buf);
			#endif
        #endif

        entry->file = QString::fromAscii( event->getLocationInformation().getFileName() );
        entry->line = event->getLocationInformation().getLineNumber();
        entry->timestamp = QDate::currentDate();
        entry->theadId = QThread::currentThreadId();
        if ( console ) {
            // mamy konsolę - wysłamy jej komunikat
            console->logOrQueueEntry(entry);
        } else {
            // nie ma konsoli - kolejkujemy
            queuedEntries.push(entry);
        }
    }
};

IMPLEMENT_LOG4CXX_OBJECT(ConsoleWidgetAppender)
typedef log4cxx::helpers::ObjectPtrT<ConsoleWidgetAppender> ConsoleWidgetAppenderPtr;

//------------------------------------------------------------------------------

LogInitializer::LogInitializer( const core::Filesystem::Path & configPath )
{
    ConsoleWidgetAppender::registerClass();
    // załadowanie parametów logowania
    PropertyConfigurator::configure(configPath.string());
    osg::setNotifyHandler( new OsgNotifyHandlerLog4cxx());
    qInstallMsgHandler(QtMessageHandler);
}

LogInitializer::~LogInitializer()
{
    // koniecznie trzeba przywrócić, inaczej będzie błąd
    osg::setNotifyHandler( new osg::StandardNotifyHandler() );
}

void LogInitializer::setConsoleWidget( EDRConsoleWidget* widget )
{
    // pobranie wszystkich appenderów dla konsoli
    LoggerList loggers;
    Logger::getRootLogger()->getLoggerRepository()->getCurrentLoggers().swap(loggers);
    loggers.push_back( Logger::getRootLogger() );
    std::set<ConsoleWidgetAppenderPtr> consoleAppenders;
    BOOST_FOREACH(LoggerPtr logger, loggers) {
        AppenderList appenders = logger->getAllAppenders();
        BOOST_FOREACH(AppenderPtr appender, appenders) {
            if ( ConsoleWidgetAppenderPtr consoleAppender = appender ) {
                consoleAppenders.insert(consoleAppender);
            }
        }
    }
    // ustawienie konsoli
    BOOST_FOREACH(ConsoleWidgetAppenderPtr consoleAppender, consoleAppenders) {
        consoleAppender->setConsole(widget);
    }
}


#else // fallback do logowania OSG

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <osg/ref_ptr>

void QtMessageHandler(QtMsgType type, const char *msg)
{
	switch (type) {
	case QtDebugMsg:
		OSG_DEBUG << "[qt]->" << msg << std::endl;
		break;
	case QtWarningMsg:
		OSG_WARN << "[qt]->" << msg << std::endl;
		break;
	default:
		// pozostałe poziomy komunikatu są już tożsame errorowi
		OSG_FATAL << "[qt]->" << msg << std::endl;
		break;
	}
}

/** Strumień przekierowywujący logowania do konsoli */
class OsgNotifyHandlerConsoleWidget : public osg::NotifyHandler
{
private:
    //! Konsola właściwa.
    EDRConsoleWidget* console;

	osg::ref_ptr<osg::NotifyHandler> defaultHandler;
    //! Kolejka wiadomości dla konsoli. Używana, gdy pojawiają się zdarzenia logowania,
    //! a konsoli jeszcze nie ma (inicjalizacja).
    std::queue<EDRConsoleWidgetEntryPtr> queuedEntries;
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
    OpenThreads::Mutex queueMutex;

public:
    //! \param console Konsola.
    OsgNotifyHandlerConsoleWidget(osg::NotifyHandler* handler) :
    console(nullptr), defaultHandler(handler)
    {}

	//! \return Domyślny handler.
	osg::NotifyHandler* getDefaultHandler()
	{
		return defaultHandler.get();
	}

    //! \param console
    void setConsole(EDRConsoleWidget* console)
    {
        this->console = console;
        // opróżniamy kolejkę komunikatów
        if ( console ) {
            ScopedLock lock(queueMutex);
            for ( ; !queuedEntries.empty(); queuedEntries.pop() ) {
                console->logOrQueueEntry(queuedEntries.front());
            }
        }
    }

    //! \param severity
    //! \param message
    virtual void notify(osg::NotifySeverity severity, const char *message)
    {
        // standardowe drukowanie
        defaultHandler->notify(severity, message);

        // z wiadomości usuwamy zbędne znaki (osg zawsze na koniec daje nową linię)
        std::string msg(message);
        boost::trim(msg);

        EDRConsoleWidgetEntryPtr entry(new EDRConsoleWidgetEntry());
        if ( severity >= osg::DEBUG_INFO ) {
            entry->severity = core::ILog::LogSeverityDebug;
        } else if ( severity >= osg::NOTICE ) {
            entry->severity = core::ILog::LogSeverityInfo;
        } else if ( severity >= osg::WARN ) {
            entry->severity = core::ILog::LogSeverityWarning;
        } else {
            entry->severity = core::ILog::LogSeverityError;
        }
        entry->message = QString::fromStdString(msg);
        entry->line = -1;
        entry->timestamp = QDate::currentDate();
        entry->theadId = QThread::currentThreadId();
        if ( console ) {
            // mamy konsolę - wysłamy jej komunikat
            console->logOrQueueEntry(entry);
        } else {
            // nie ma konsoli - kolejkujemy
            ScopedLock lock(queueMutex);
            queuedEntries.push(entry);
        }
    }
};


LogInitializer::LogInitializer( const core::Filesystem::Path & configPath )
{
    qInstallMsgHandler(QtMessageHandler);
    osg::setNotifyHandler( new OsgNotifyHandlerConsoleWidget(osg::getNotifyHandler()) );
}

LogInitializer::~LogInitializer()
{
    OsgNotifyHandlerConsoleWidget* handler = dynamic_cast<OsgNotifyHandlerConsoleWidget*>(osg::getNotifyHandler());
    UTILS_ASSERT(handler);
    osg::setNotifyHandler( handler->getDefaultHandler() );
}


void LogInitializer::setConsoleWidget( EDRConsoleWidget* widget )
{
    OsgNotifyHandlerConsoleWidget* handler = dynamic_cast<OsgNotifyHandlerConsoleWidget*>(osg::getNotifyHandler());
    UTILS_ASSERT(handler);
    handler->setConsole(widget);
}

#endif

#else //CORE_DEFINE_DISABLE_LOGGING

void QtMessageHandler(QtMsgType type, const char *msg)
{
}

/** Strumień przekierowywujący logowania do konsoli */
class OsgNotifyHandlerConsoleWidget : public osg::NotifyHandler
{
public:
	//! \param console Konsola.
	OsgNotifyHandlerConsoleWidget()
	{

	}

	  //! \param severity
	  //! \param message
	  virtual void notify(osg::NotifySeverity severity, const char *message)
	  {
		  
	  }
};


LogInitializer::LogInitializer( const core::Filesystem::Path & configPath )
{
	qInstallMsgHandler(QtMessageHandler);
	osg::setNotifyHandler( new OsgNotifyHandlerConsoleWidget() );
}

LogInitializer::~LogInitializer()
{
	osg::setNotifyHandler( new osg::StandardNotifyHandler() );
}


void LogInitializer::setConsoleWidget( EDRConsoleWidget* widget )
{
	
}

#endif

