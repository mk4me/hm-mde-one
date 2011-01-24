#include "CorePCH.h"
#include "ConsoleWidget.h"
#include <QtGui/QScrollBar>

ConsoleWidget::ConsoleWidget(void): 
  Ui::ConsoleWidget()
, QWidget()
{
    qRegisterMetaType<ConsoleWidgetEntry>("ConsoleWidgetEntry");
    qRegisterMetaType<ConsoleWidgetEntryPtr>("ConsoleWidgetEntryPtr");
    setupUi(this); 
}

ConsoleWidget::~ConsoleWidget(void)
{
}

// void ConsoleWidget::onEcho()
// {
//   QString text = lineEdit->text(); 
//   //std::string s = text.toUtf8(); 
//   //log << s;
//   textEdit->insertPlainText(text); 
//   textEdit->insertHtml("<br>"); 
// 
//   QScrollBar *sb = textEdit->verticalScrollBar();
//   sb->setValue(sb->maximum());
// 
//   lineEdit->clear(); 
// 
//   
// 
//   
// }

void ConsoleWidget::logEntry( const ConsoleWidgetEntry& entry )
{
    QTextCharFormat fmt;
    
    if ( entry.severity == core::LogSeverityInfo ) {
        fmt.setForeground( QColor(0, 0, 0, 255) );
    } else if ( entry.severity == core::LogSeverityWarning ) {
        fmt.setForeground( QColor(255, 255, 0, 255) );
    } else if ( entry.severity == core::LogSeverityError ) {
        fmt.setForeground( QColor(255, 0, 0, 255) );
    } else {
        fmt.setForeground( QColor(100, 150, 50, 255) );
    }

    textEdit->setCurrentCharFormat(fmt);
    textEdit->append( entry.message ); 
}

void ConsoleWidget::logEntry( ConsoleWidgetEntryPtr entry )
{
    logEntry(*entry);
}

void ConsoleWidget::flushQueue()
{
    QMutexLocker locker(&queueMutex);
    while ( !queuedEntries.empty() ) {
        logEntry( *queuedEntries.front() );
        queuedEntries.pop();
    }
}

void ConsoleWidget::queueEntry( ConsoleWidgetEntryPtr entry )
{
    QMutexLocker locker(&queueMutex);
    queuedEntries.push(entry);
    // QMetaObject::invokeMethod( console, "logEntry", Q_ARG(ConsoleWidgetEntryPtr, entry) );
}

void ConsoleWidget::logOrQueueEntry( ConsoleWidgetEntryPtr entry )
{
    if ( QThread::currentThread() == this->thread() ) {
        logEntry(*entry);
    } else {
        queueEntry(entry);
    }
}