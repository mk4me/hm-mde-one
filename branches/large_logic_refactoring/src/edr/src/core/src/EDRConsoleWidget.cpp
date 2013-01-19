#include "CorePCH.h"
#include <QtCore/QThread>
#include <QtGui/QAction>
#include "EDRConsoleWidget.h"

using namespace core;

EDRConsoleWidget::EDRConsoleWidget(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags)
{
	setupUi(this);
    init();
}

EDRConsoleWidget::~EDRConsoleWidget()
{

}

void EDRConsoleWidget::init()
{
    qRegisterMetaType<EDRConsoleWidgetEntry>("EDRConsoleWidgetEntry");
    qRegisterMetaType<EDRConsoleWidgetEntryPtr>("EDRConsoleWidgetEntryPtr");

	setWindowTitle(tr("Console"));
}

void EDRConsoleWidget::logEntry( const EDRConsoleWidgetEntry& entry )
{
    QTextCharFormat fmt;

    if ( entry.severity == core::ILog::LogSeverityInfo ) {
        fmt.setForeground( QColor(0, 0, 0, 255) );
    } else if ( entry.severity == core::ILog::LogSeverityWarning ) {
        fmt.setForeground( QColor(255, 130, 0, 255) );
    } else if ( entry.severity == core::ILog::LogSeverityError ) {
        fmt.setForeground( QColor(255, 0, 0, 255) );
    } else {
        fmt.setForeground( QColor(100, 150, 50, 255) );
    }

    textEdit->setCurrentCharFormat(fmt);
    textEdit->append( entry.message );
}

void EDRConsoleWidget::logEntry( EDRConsoleWidgetEntryPtr entry )
{
    logEntry(*entry);
}

void EDRConsoleWidget::flushQueue()
{
    QMutexLocker locker(&queueMutex);
    while ( !queuedEntries.empty() ) {
        logEntry( *queuedEntries.front() );
        queuedEntries.pop();
    }
}

void EDRConsoleWidget::queueEntry( EDRConsoleWidgetEntryPtr entry )
{
    QMutexLocker locker(&queueMutex);
    queuedEntries.push(entry);
}

void EDRConsoleWidget::logOrQueueEntry( EDRConsoleWidgetEntryPtr entry )
{
	QThread* widgetThread = this->thread();
	QThread* currentThred = QThread::currentThread();
    if ( currentThred == widgetThread ) {
        logEntry(*entry);
    } else {
        queueEntry(entry);
    }
}

void EDRConsoleWidget::setWordWrap( bool wrap )
{
    textEdit->setWordWrapMode( wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap );
}
