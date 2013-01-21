/********************************************************************
    created:  2011/05/04
    created:  4:5:2011   8:42
    filename: CoreConsoleWidget.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__CORECONSOLEWIDGET_H__
#define HEADER_GUARD_CORE__CORECONSOLEWIDGET_H__

#include "ui_CoreConsoleWidget.h"
#include <core/SmartPtr.h>
#include <core/ILog.h>
#include <queue>
#include <QtCore/QDate>
#include <QtCore/QMutex>

namespace coreUI {

//! Wpis do konsoli.
struct CoreConsoleWidgetEntry {
    //! Poziom ważności komuniaktu.
    core::ILog::LogSeverity severity;
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

typedef core::shared_ptr<CoreConsoleWidgetEntry> CoreConsoleWidgetEntryPtr;

Q_DECLARE_METATYPE(CoreConsoleWidgetEntryPtr);
Q_DECLARE_METATYPE(CoreConsoleWidgetEntry);

class CoreConsoleWidget : public QWidget, private Ui::CoreConsoleWidget
{
    Q_OBJECT

public:
    CoreConsoleWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    ~CoreConsoleWidget();

public:
    //! \param entry Komunikat do zakolejkowania bądź natychmiastowego zalogowania,
    //! w zależności od tego, czy mamy do czynienia z wątkiem właściwym konsoli.
    void logOrQueueEntry(CoreConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(CoreConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do zakolejkowania.
    void queueEntry(CoreConsoleWidgetEntryPtr entry);

private:
    void init();

public slots:

    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(const CoreConsoleWidgetEntry& entry);
    //! Opróżnia kolejkę komunikatów.
    void flushQueue();
    //! \param wrap Czy zawijać tekst?
    void setWordWrap(bool wrap);

private:
    //! Zakolejkowane wpisy.
    std::queue< CoreConsoleWidgetEntryPtr > queuedEntries;
    //! Muteks do kontroli kolejki wpisów.
    QMutex queueMutex;
};

}

#endif  //  HEADER_GUARD_CORE__CORECONSOLEWIDGET_H__

