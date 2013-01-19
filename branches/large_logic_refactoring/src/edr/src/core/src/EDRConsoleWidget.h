/********************************************************************
    created:  2011/05/04
    created:  4:5:2011   8:42
    filename: EDRConsoleWidget.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRCONSOLEWIDGET_H__
#define HEADER_GUARD_CORE__EDRCONSOLEWIDGET_H__

#include "ui_EDRConsoleWidget.h"
#include <core/SmartPtr.h>
#include <core/ILog.h>
#include <queue>
#include <QtCore/QDate>
#include <QtCore/QMutex>

class QAction;

namespace core {

//! Wpis do konsoli.
struct EDRConsoleWidgetEntry {
    //! Poziom ważności komuniaktu.
    ILog::LogSeverity severity;
    //! Wiadomość.
    QString message;
    //! Plik źródłowy.
    QString file;
    //! Linia w pliku.
    unsigned line;
    //! Moment zalogowania.
    QDate timestamp;
    //! Id wątku logującego.
    Qt::HANDLE theadId;
};

typedef shared_ptr<EDRConsoleWidgetEntry> EDRConsoleWidgetEntryPtr;

Q_DECLARE_METATYPE(EDRConsoleWidgetEntryPtr);
Q_DECLARE_METATYPE(EDRConsoleWidgetEntry);

class EDRConsoleWidget : public QWidget, private Ui::EDRConsoleWidget
{
    Q_OBJECT

public:
    EDRConsoleWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    ~EDRConsoleWidget();

public:
    //! \param entry Komunikat do zakolejkowania bądź natychmiastowego zalogowania,
    //! w zależności od tego, czy mamy do czynienia z wątkiem właściwym konsoli.
    void logOrQueueEntry(EDRConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(EDRConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do zakolejkowania.
    void queueEntry(EDRConsoleWidgetEntryPtr entry);

private:
    void init();

public slots:

    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(const EDRConsoleWidgetEntry& entry);
    //! Opróżnia kolejkę komunikatów.
    void flushQueue();
    //! \param wrap Czy zawijać tekst?
    void setWordWrap(bool wrap);

private:
    //! Zakolejkowane wpisy.
    std::queue< EDRConsoleWidgetEntryPtr > queuedEntries;
    //! Muteks do kontroli kolejki wpisów.
    QMutex queueMutex;
};

}

#endif  //  HEADER_GUARD_CORE__EDRCONSOLEWIDGET_H__

