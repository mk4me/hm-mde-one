#include "CorePCH.h"
#include <QtCore/QThread>
#include "CoreConsoleWidget.h"

using namespace coreUI;
using namespace core;

CoreConsoleWidget::CoreConsoleWidget(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags)
{
	setupUi(this);
    init();
}

CoreConsoleWidget::~CoreConsoleWidget()
{

}

void CoreConsoleWidget::init()
{
    qRegisterMetaType<CoreConsoleWidgetEntry>("CoreConsoleWidgetEntry");
    qRegisterMetaType<CoreConsoleWidgetEntryPtr>("CoreConsoleWidgetEntryPtr");

	setWindowTitle(tr("Console"));
}

void CoreConsoleWidget::logEntry( const CoreConsoleWidgetEntry& entry )
{
    QTextCharFormat fmt;

    if ( entry.severity == ILog::LogSeverityInfo ) {
        fmt.setForeground( QColor(0, 0, 0, 255) );
    } else if ( entry.severity == ILog::LogSeverityWarning ) {
        fmt.setForeground( QColor(255, 130, 0, 255) );
    } else if ( entry.severity == ILog::LogSeverityError ) {
        fmt.setForeground( QColor(255, 0, 0, 255) );
    } else {
        fmt.setForeground( QColor(100, 150, 50, 255) );
    }

    textEdit->setCurrentCharFormat(fmt);
    textEdit->append( entry.message );
}

void CoreConsoleWidget::logEntry( CoreConsoleWidgetEntryPtr entry )
{
    logEntry(*entry);
}

void CoreConsoleWidget::flushQueue()
{
    QMutexLocker locker(&queueMutex);
    while ( !queuedEntries.empty() ) {
        logEntry( *queuedEntries.front() );
        queuedEntries.pop();
    }
}

void CoreConsoleWidget::queueEntry( CoreConsoleWidgetEntryPtr entry )
{
    QMutexLocker locker(&queueMutex);
    queuedEntries.push(entry);
}

void CoreConsoleWidget::logOrQueueEntry( CoreConsoleWidgetEntryPtr entry )
{
	QThread* widgetThread = this->thread();
	QThread* currentThred = QThread::currentThread();
    if ( currentThred == widgetThread ) {
        logEntry(*entry);
    } else {
        queueEntry(entry);
    }
}

void CoreConsoleWidget::setWordWrap( bool wrap )
{
    textEdit->setWordWrapMode( wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap );
}
