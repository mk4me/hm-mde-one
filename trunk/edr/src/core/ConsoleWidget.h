#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

#include <QtGui/QWidget>
#include "Log.h"
#include <core/SmartPtr.h>
#include <queue>

#include "ui_ConsoleWidget.h"
//#include "Logger.h"

//------------------------------------------------------------------------------

//! Wpis do konsoli.
struct ConsoleWidgetEntry {
    //! Poziom wa¿noœci komuniaktu.
    core::LogSeverity severity;
    //! Wiadomoœæ.
    QString message;
    //! Plik Ÿród³owy.
    QString file;
    //! Linia w pliku.
    unsigned line;
    //! Moment zalogowania.
    QDate timestamp;
    //! Id w¹tku loguj¹cego.
    Qt::HANDLE theadId;
};

typedef core::shared_ptr<ConsoleWidgetEntry> ConsoleWidgetEntryPtr;

Q_DECLARE_METATYPE(ConsoleWidgetEntryPtr);
Q_DECLARE_METATYPE(ConsoleWidgetEntry);

//------------------------------------------------------------------------------

class ConsoleWidget: public QWidget, Ui::ConsoleWidget
{
    Q_OBJECT
private:
    //! Zakolejkowane wpisy.
    std::queue< ConsoleWidgetEntryPtr > queuedEntries;
    //! Muteks do kontroli kolejki wpisów.
    QMutex queueMutex;

public:
    ConsoleWidget(void);
    ~ConsoleWidget(void);

public:
    //! \param entry Komunikat do zakolejkowania b¹dŸ natychmiastowego zalogowania,
    //! w zale¿noœci od tego, czy mamy do czynienia z w¹tkiem w³aœciwym konsoli.
    void logOrQueueEntry(ConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(ConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do zakolejkowania.
    void queueEntry(ConsoleWidgetEntryPtr entry);

public slots: 
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(const ConsoleWidgetEntry& entry);
    //! Opró¿nia kolejkê komunikatów.
    void flushQueue();
    //! \param wrap Czy zawijaæ tekst?
    void setWordWrap(bool wrap);

};

//------------------------------------------------------------------------------

#endif