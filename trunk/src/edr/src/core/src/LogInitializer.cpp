#include "CorePCH.h"
#include "LogInitializer.h"
#include "Log.h"
#include <QtCore/QThread>
#include <boost/foreach.hpp>
#include <osg/Notify>
#include <boost/algorithm/string.hpp>
#include <utils/Debug.h>
#include "EDRConsoleWidget.h"
#include <core/StringTools.h>

#include <core/IPath.h>

using namespace core;


void QtMessageHandler(QtMsgType type, const char *msg)
{
    switch (type) {
        case QtDebugMsg:
            LOG_DEBUG_STATIC_NAMED("qt", msg);
            break;
        case QtWarningMsg:
            LOG_WARNING_STATIC_NAMED("qt", msg);
            break;
        default:
            // pozostałe poziomy komunikatu są już tożsame errorowi
            LOG_ERROR_STATIC_NAMED("qt", msg);
            break;
    }
}

#ifdef CORE_ENABLE_LOG4CXX

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/synchronized.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/helpers/transcoder.h>
#include <queue>

using namespace log4cxx;

//------------------------------------------------------------------------------

/** Strumień przekierowywujący logowania do log4cxx */
class OsgNotifyHandlerLog4cxx : public osg::NotifyHandler
{
private:
    //! Faktyczny logger.
    log4cxx::LoggerPtr logger;

public:
    //! \param logger Faktyczny logger.
    OsgNotifyHandlerLog4cxx(LoggerPtr logger) :
    logger(logger)
    {}
    //! \param severity
    //! \param message
    void notify(osg::NotifySeverity severity, const char *message)
    {
        // z wiadomości usuwamy zbędne znaki (osg zawsze na koniec daje nową linię)
        std::string msg = message;
        boost::trim(msg);
        switch (severity) {
            case osg::ALWAYS:
                LOG4CXX_LOG(logger, Level::getAll(), msg)
                break;
            case osg::FATAL:
                LOG4CXX_ERROR(logger, msg);
                break;
            case osg::WARN:
                LOG4CXX_WARN(logger, msg);
                break;
            case osg::NOTICE:
            case osg::INFO:
                LOG4CXX_INFO(logger, msg);
                break;
            case osg::DEBUG_INFO:
            case osg::DEBUG_FP:
                LOG4CXX_DEBUG(logger, msg);
                break;
            default:
                UTILS_ASSERT(false, "Nieznany poziom osg. Wiadomość: %s", message);
                break;
        }
    }
};

//------------------------------------------------------------------------------

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
    console(NULL)
    {}
    //!
    virtual ~ConsoleWidgetAppender()
    {
        UTILS_ASSERT(console == NULL, "Wskaźnik powinien zostać jawnie wyzerowany metodą setConsole!");
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
    {}

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
            entry->message = QString::fromUtf16( reinterpret_cast<const ushort*>(buf.c_str()) );
        #else
            entry->message = QString::fromStdString(buf);
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

LogInitializer::LogInitializer( const char* configPath )
{
    ConsoleWidgetAppender::registerClass();
    // załadowanie parametów logowania
    PropertyConfigurator::configure(configPath);
    osg::setNotifyHandler( new OsgNotifyHandlerLog4cxx(Logger::getLogger( "osg" ) ));
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


/** Strumień przekierowywujący logowania do konsoli */
class OsgNotifyHandlerConsoleWidget : public osg::NotifyHandler
{
private:
    //! Konsola właściwa.
    EDRConsoleWidget* console;
    //!
    osg::ref_ptr<osg::NotifyHandler> defaultHandler;
    //! Kolejka wiadomości dla konsoli. Używana, gdy pojawiają się zdarzenia logowania,
    //! a konsoli jeszcze nie ma (inicjalizacja).
    std::queue<EDRConsoleWidgetEntryPtr> queuedEntries;
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
    OpenThreads::Mutex queueMutex;

public:
    //! \param console Konsola.
    OsgNotifyHandlerConsoleWidget(osg::NotifyHandler* handler) :
    console(NULL), defaultHandler(handler)
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
    void notify(osg::NotifySeverity severity, const char *message)
    {
        // standardowe drukowanie
        defaultHandler->notify(severity, message);

        // z wiadomości usuwamy zbędne znaki (osg zawsze na koniec daje nową linię)
        std::string msg = message;
        boost::trim(msg);

        EDRConsoleWidgetEntryPtr entry(new EDRConsoleWidgetEntry());
        if ( severity >= osg::DEBUG_INFO ) {
            entry->severity = core::LogSeverityDebug;
        } else if ( severity >= osg::NOTICE ) {
            entry->severity = core::LogSeverityInfo;
        } else if ( severity >= osg::WARN ) {
            entry->severity = core::LogSeverityWarning;
        } else {
            entry->severity = core::LogSeverityError;
        }
        entry->message = core::toQString(msg);
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


LogInitializer::LogInitializer( const char* configPath )
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

#endif // CORE_ENABLE_LOG4CXX
