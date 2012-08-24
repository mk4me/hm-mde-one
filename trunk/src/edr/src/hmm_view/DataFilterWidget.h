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

//! widget przechowuj¹cy grupê filtrów
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
    //! \param bigLabelText du¿a etykieta dla tekstu
    //! \param dataFilter filtr danych trafiaj¹cych do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, PluginSubject::DataFilterPtr dataFilter, const QPixmap* icon = nullptr);
    //! Dodaje nowy filtr do grupy
    //! \param bigLabelText du¿a etykieta dla tekstu
    //! \param command akcja filtruj¹ca dane trafiaj¹ce do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, IFilterCommandPtr command, const QPixmap* icon = nullptr);
    //!  Dodaje nowy filtr do grupy
    //! \param entry gotowy, przygotowany wczeœniej filtr
    void addFilter(FilterEntryWidget* entry);
    //! zamyka mo¿liwoœæ dodawania kolejnych filtrów do grupy
    void closeFilters();
    //! \return czy dana grupa jest aktualnie aktywna?
    bool getActive() const { return active; }
    //! ustawia aktywnoœæ grupy
    //! \param val aktywuje / deaktywuje grupê
    void setActive(bool val);
    //! \return obrazek reprezentujacy grupê
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
    //! obs³uguje zdarzenia klikniêcia myszki i wywo³uje odpowiednie filtry
    //! \param object 
    //! \param event 
    bool eventFilter(QObject *object, QEvent *event);

signals:
    //! zmieni³a siê aktywnoœæ grupy
    //! \param aktywowana / deaktywowana
    void activated(bool);
    //! grupa zosta³a klikniêta
    void clicked();

public:
    //! obs³uguje zdarzenia klikniêcia myszki i wywo³uje odpowiednie filtry
    //! \param e 
    virtual void mousePressEvent(QMouseEvent *e);
    //! deaktywuje filtry w grupie
    //! \param toSkip filtr, który powinien zostaæ pominiêty
    void uncheckEntries(FilterEntryWidget* toSkip = nullptr);

public slots:
    //! resetuje ustawienia filtrów (zmienione np. przez konfiguratory)
    void resetFilters();

private slots:
    //! klikniêto grupê filtrów
    void onClick();

private:
    //! filtry zawarte w grupie
    std::vector<FilterEntryWidget*> entries;
    //! g³ówne okno aplikacji, dostarcza drzewo danych
    HmmMainWindow* hmmWindow;
    //! czy grupa jest aktywna
    bool active;
    //! czy do grupy mo¿na jeszcze dodaæ nastêpne filtry
    bool filtersClosed;
};
typedef boost::shared_ptr<DataFilterWidget> DataFilterWidgetPtr;
typedef boost::shared_ptr<const DataFilterWidget> DataFilterWidgetConstPtr;


#endif
