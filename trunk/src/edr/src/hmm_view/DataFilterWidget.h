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

//! widget przechowuj�cy grup� filtr�w
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
    //! \param bigLabelText du�a etykieta dla tekstu
    //! \param dataFilter filtr danych trafiaj�cych do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, PluginSubject::DataFilterPtr dataFilter, const QPixmap* icon = nullptr);
    //! Dodaje nowy filtr do grupy
    //! \param bigLabelText du�a etykieta dla tekstu
    //! \param command akcja filtruj�ca dane trafiaj�ce do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, IFilterCommandPtr command, const QPixmap* icon = nullptr);
    //!  Dodaje nowy filtr do grupy
    //! \param entry gotowy, przygotowany wcze�niej filtr
    void addFilter(FilterEntryWidget* entry);
    //! zamyka mo�liwo�� dodawania kolejnych filtr�w do grupy
    void closeFilters();
    //! \return czy dana grupa jest aktualnie aktywna?
    bool getActive() const { return active; }
    //! ustawia aktywno�� grupy
    //! \param val aktywuje / deaktywuje grup�
    void setActive(bool val);
    //! \return obrazek reprezentujacy grup�
    const QPixmap& getPixmap() const { return *this->pictureLabel->pixmap(); }
    //! \return nazwa grupy filtr�w
    QString getName() const { return this->label->text(); }
    //! usawia kolor grupy filtr�w
    //! \param color nowy kolor
    void setColor(const QColor& color);
    //! \return liczba filtr�w w grupie
    int getNumEntries() const { return entries.size(); }
    //! zwraca filtr 
    //! \param index ineks filtru w grupie
    //! \return filtr przydzielony do grupy
    const FilterEntryWidget* getEntry(int index) const;

protected:
    //! obs�uguje zdarzenia klikni�cia myszki i wywo�uje odpowiednie filtry
    //! \param object 
    //! \param event 
    bool eventFilter(QObject *object, QEvent *event);

signals:
    //! zmieni�a si� aktywno�� grupy
    //! \param aktywowana / deaktywowana
    void activated(bool);
    //! grupa zosta�a klikni�ta
    void clicked();

public:
    //! obs�uguje zdarzenia klikni�cia myszki i wywo�uje odpowiednie filtry
    //! \param e 
    virtual void mousePressEvent(QMouseEvent *e);
    //! deaktywuje filtry w grupie
    //! \param toSkip filtr, kt�ry powinien zosta� pomini�ty
    void uncheckEntries(FilterEntryWidget* toSkip = nullptr);

public slots:
    //! resetuje ustawienia filtr�w (zmienione np. przez konfiguratory)
    void resetFilters();

private slots:
    //! klikni�to grup� filtr�w
    void onClick();

private:
    //! filtry zawarte w grupie
    std::vector<FilterEntryWidget*> entries;
    //! g��wne okno aplikacji, dostarcza drzewo danych
    HmmMainWindow* hmmWindow;
    //! czy grupa jest aktywna
    bool active;
    //! czy do grupy mo�na jeszcze doda� nast�pne filtry
    bool filtersClosed;
};
typedef boost::shared_ptr<DataFilterWidget> DataFilterWidgetPtr;
typedef boost::shared_ptr<const DataFilterWidget> DataFilterWidgetConstPtr;


#endif
