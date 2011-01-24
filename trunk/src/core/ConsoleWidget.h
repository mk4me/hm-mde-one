#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

#include <QtGui/QWidget>
#include <core/Log.h>
#include <queue>

#include "ui_ConsoleWidget.h"
//#include "Logger.h"

//------------------------------------------------------------------------------

//! Wpis do konsoli.
struct ConsoleWidgetEntry {
    //! Poziom wa�no�ci komuniaktu.
    core::LogSeverity severity;
    //! Wiadomo��.
    QString message;
    //! Plik �r�d�owy.
    QString file;
    //! Linia w pliku.
    unsigned line;
    //! Moment zalogowania.
    QDate timestamp;
    //! Id w�tku loguj�cego.
    Qt::HANDLE theadId;
};

typedef CORE_SHARED_PTR(ConsoleWidgetEntry) ConsoleWidgetEntryPtr;
Q_DECLARE_METATYPE(ConsoleWidgetEntryPtr);
Q_DECLARE_METATYPE(ConsoleWidgetEntry);

//------------------------------------------------------------------------------

class ConsoleWidget: public QWidget, Ui::ConsoleWidget
{
    Q_OBJECT
private:
    //! Zakolejkowane wpisy.
    std::queue< ConsoleWidgetEntryPtr > queuedEntries;
    //! Muteks do kontroli kolejki wpis�w.
    QMutex queueMutex;

public:
    ConsoleWidget(void);
    ~ConsoleWidget(void);

public:
    //! \param entry Komunikat do zakolejkowania b�d� natychmiastowego zalogowania,
    //! w zale�no�ci od tego, czy mamy do czynienia z w�tkiem w�a�ciwym konsoli.
    void logOrQueueEntry(ConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(ConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do zakolejkowania.
    void queueEntry(ConsoleWidgetEntryPtr entry);

public slots: 
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(const ConsoleWidgetEntry& entry);
    //! Opr�nia kolejk� komunikat�w.
    void flushQueue();
};

//------------------------------------------------------------------------------

#endif