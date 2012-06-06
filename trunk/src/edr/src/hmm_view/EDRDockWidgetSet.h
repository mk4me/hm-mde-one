/********************************************************************
	created:	2011/09/19
	created:	19:9:2011   14:55
	filename: 	EDRDockWidgetSet.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EDRDOCKWIDGETSET_H__
#define HEADER_GUARD_HMM__EDRDOCKWIDGETSET_H__

#include <boost/range.hpp>
#include <QtCore/QString>
#include <QtGui/QMainWindow>
#include "EDRDockWidget.h"

//! Klasa obsluguje grupe dock widgetow, sama bedac dock widgetem
class EDRDockWidgetSet : public EDRDockWidget
{
	Q_OBJECT;
public:
    typedef boost::iterator_range<std::list<EDRDockWidget*>::const_iterator> const_range;

public:
	//! Konstruktor
	//! \param title wyswietlana w tabie nazwa grupy
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetSet(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	//! Typowy konstruktor Qt
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetSet(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetSet() {}

public:
    //! Dodaje dock widget do grupy, jesli nie ma na niego miejsca rzucany jest wyjatek
    //! \param widget dodawany dock widget
    //! \param area obszar okna (lewy, prawy, gora dol)
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
    void addDockWidget(EDRDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jesli nie ma na niego miejsca rzucany jest wyjatek
    //! \param widget dodawany dock widget
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
	void addDockWidget(EDRDockWidget* widget, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jesli nie ma na niego miejsca rzucany jest wyjatek
    //! \param widget dodawany dock widget
    void addDockWidget(EDRDockWidget* widget);
	//! \return czy mozna jeszcze dodac dock widget do grpy
	bool isAdditionPossible(EDRDockWidget* widget = nullptr) const;
	//! blokuje lub odblokowuje mozliwosc dodawania widgetow
	//! \param additionPossible 
	void blockAddition(bool additionPossible);
	//! \return liczba dock widgetow w grupie
	int getNumWidgets() const;
	//! \return maksymalna liczba dock widgetow w grupie
	int getMaxWidgetsNumber() const { return maxWidgetsNumber; }
	//! ustawia maksymalna liczbe dock widgetow w grupie
	//! \param val 
	void setMaxWidgetsNumber(int val) { maxWidgetsNumber = val; }
    //! \return const range wszystkich dock widgetow w grupie
    const_range getDockWidgets() const { return boost::make_iterator_range(widgetsList.cbegin(), widgetsList.cend()); }

signals:
    //! sygnal oznaczajacy, ze dock widget podpiety pod grupe zostal zamkniety
    void dockClosed();

private slots:
    //! dock widget podpiety pod grupe zostal zamkniety
    //! \param object 
    void onDockWidgetClosed(QObject* object);
    //! dock widget podpiety pod grupe zmienil pozycje
    //! \param area 
    void onDockWidgetLocationChanged(Qt::DockWidgetArea area);

private:
    //! kontener dodawanych dock widgetow
	QMainWindow* mainWindow;
    //! dodane do grupy dock widgety
	std::list<EDRDockWidget*> widgetsList;
    //! maksymalna liczba dock widgetow w grupie
	int maxWidgetsNumber;
    //! czy mozliwe jest dalsze dodawanie widgetow (niezalezne od maksymalnej liczby dock widgetow)
	bool additionPossible;
};

#endif
