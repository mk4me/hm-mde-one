/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   10:20
	filename: 	DataFilterWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__DATAFILTERWIDGET_H__
#define HEADER_GUARD_HMM__DATAFILTERWIDGET_H__

#include <QtCore/QString>
#include <QtGui/QWidget>
#include <boost/date_time.hpp>
#include <boost/function.hpp>
#include <osg/Vec3>
#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"
#include "ui_filter.h"

//! widget przechowujący grupę filtrów
class DataFilterWidget : public QWidget, private Ui::FilterWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param name nazwa GUI
    //! \param pixmap ikona
    //! \param hmmWindow widok HMM
    DataFilterWidget(const QString& name, const QPixmap& pixmap, HmmMainWindow* hmmWindow);
	virtual ~DataFilterWidget() {}

public:
    //! Dodaje nowy filtr do grupy
    //! \param bigLabelText duża etykieta dla tekstu
    //! \param dataFilter filtr danych trafiających do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, PluginSubject::DataFilterPtr dataFilter, const QPixmap* icon = nullptr);
    //! Dodaje nowy filtr do grupy
    //! \param bigLabelText duża etykieta dla tekstu
    //! \param command akcja filtrująca dane trafiające do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, IFilterCommandPtr command, const QPixmap* icon = nullptr);
    //!  Dodaje nowy filtr do grupy
    //! \param entry gotowy, przygotowany wcześniej filtr
    void addFilter(FilterEntryWidget* entry);
    //! zamyka możliwość dodawania kolejnych filtrów do grupy
    void closeFilters();
    //! \return czy dana grupa jest aktualnie aktywna?
    bool getActive() const { return active; }
    //! ustawia aktywność grupy
    //! \param val aktywuje / deaktywuje grupę
    void setActive(bool val);
    //! \return obrazek reprezentujacy grupę
    const QPixmap& getPixmap() const { return *this->pictureLabel->pixmap(); }
    //! \return nazwa grupy filtrów
    QString getName() const { return this->label->text(); }
    //! usawia kolor grupy filtrów
    //! \param color nowy kolor
    void setColor(const QColor& color);
    //! \return liczba filtrów w grupie
    int getNumEntries() const { return entries.size(); }
    //! zwraca filtr 
    //! \param index ineks filtru w grupie
    //! \return filtr przydzielony do grupy
    const FilterEntryWidget* getEntry(int index) const;

protected:
    //! obsługuje zdarzenia kliknięcia myszki i wywołuje odpowiednie filtry
    //! \param object 
    //! \param event 
    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    //! zmieniła się aktywność grupy
    //! \param aktywowana / deaktywowana
    void activated(bool);
    //! grupa została kliknięta
    void clicked();

public:
    //! obsługuje zdarzenia kliknięcia myszki i wywołuje odpowiednie filtry
    //! \param e 
    virtual void mousePressEvent(QMouseEvent *e);
    //! deaktywuje filtry w grupie
    //! \param toSkip filtr, który powinien zostać pominięty
    void uncheckEntries(FilterEntryWidget* toSkip = nullptr);

public Q_SLOTS:
    //! resetuje ustawienia filtrów (zmienione np. przez konfiguratory)
    void resetFilters();

private Q_SLOTS:
    //! kliknięto grupę filtrów
    void onClick();

private:
    //! filtry zawarte w grupie
    std::vector<FilterEntryWidget*> entries;
    //! główne okno aplikacji, dostarcza drzewo danych
    HmmMainWindow* hmmWindow;
    //! czy grupa jest aktywna
    bool active;
    //! czy do grupy można jeszcze dodać następne filtry
    bool filtersClosed;
};
typedef boost::shared_ptr<DataFilterWidget> DataFilterWidgetPtr;
typedef boost::shared_ptr<const DataFilterWidget> DataFilterWidgetConstPtr;


#endif
