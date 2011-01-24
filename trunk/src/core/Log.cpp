#include "CorePCH.h"
#include <core/Log.h>

#include <boost/foreach.hpp>
#include <osg/Notify>
#include <boost/algorithm/string.hpp>
#include <utils/Debug.h>
#include "ConsoleWidget.h"

using namespace core;

#ifdef CORELIB_LOG4CXX_ENABLED

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/synchronized.h>
#include <queue>

using namespace log4cxx;

//------------------------------------------------------------------------------

/** Strumieñ przekierowywuj¹cy logowania do log4cxx */
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
        // z wiadomoœci usuwamy zbêdne znaki (osg zawsze na koniec daje now¹ liniê)
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
                UTILS_ASSERT(false, "Nieznany poziom osg. Wiadomoœæ: %s", message);
                break;
        }
    }
};

//------------------------------------------------------------------------------

class ConsoleWidgetAppender : public AppenderSkeleton
{
private:
    //! Konsola w³aœciwa.
    ConsoleWidget* console;
    //! Kolejka wiadomoœci dla konsoli. U¿ywana, gdy pojawiaj¹ siê zdarzenia logowania,
    //! a konsoli jeszcze nie ma (inicjalizacja).
    std::queue<ConsoleWidgetEntryPtr> queuedEntries;

public:
    DECLARE_LOG4CXX_OBJECT(ConsoleWidgetAppender)
    BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(ConsoleWidgetAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
    END_LOG4CXX_CAST_MAP()

public:
    //! Zeruj¹cy konstruktor
    ConsoleWidgetAppender() :
    console(NULL)
    {}
    //!
    virtual ~ConsoleWidgetAppender()
    {
        UTILS_ASSERT(console == NULL, "WskaŸnik powinien zostaæ jawnie wyzerowany metod¹ setConsole!");
    }

public:
    //! \param console Widget konsoli.
    void setConsole(ConsoleWidget* console)
    {
        // korzytamy z muteksa z klasy bazowej !
        helpers::synchronized sync(mutex);
        this->console = console;
        // opró¿niamy kolejkê komunikatów
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

        ConsoleWidgetEntryPtr entry(new ConsoleWidgetEntry());
        //entry.message = QString::fromWCharArray( event->getMessage().c_str() );
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
        entry->message = QString::fromUtf16( reinterpret_cast<const ushort*>(buf.c_str()) );
        entry->file = QString::fromAscii( event->getLocationInformation().getFileName() );
        entry->line = event->getLocationInformation().getLineNumber();
        entry->timestamp = QDate::currentDate();
        entry->theadId = QThread::currentThreadId();
        if ( console ) {
            // mamy konsolê - wys³amy jej komunikat
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
    // za³adowanie parametów logowania
    PropertyConfigurator::configure(configPath);
    osg::setNotifyHandler( new OsgNotifyHandlerLog4cxx(Logger::getLogger( "osg" ) ));
}

LogInitializer::~LogInitializer()
{
    // koniecznie trzeba przywróciæ, inaczej bêdzie b³¹d
    osg::setNotifyHandler( new osg::StandardNotifyHandler() );
}


void core::LogInitializer::setConsoleWidget( ConsoleWidget* widget )
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

LogInitializer::LogInitializer( const char* configPath )
{
    // nic nie musimy robiæ
}

LogInitializer::~LogInitializer()
{
    // nic nie musimy robiæ
}


void core::LogInitializer::setConsoleWidget( ConsoleWidget* widget )
{

}

#endif


