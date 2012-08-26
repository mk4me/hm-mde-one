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

//! Klasa obsługuje grupę dock widgetów, sama będąc dock widgetem
class EDRDockWidgetSet : public EDRDockWidget
{
	Q_OBJECT;
public:
    //! niemodyfikowalny zakres wszystkich podległych widgetów zakładki
    typedef boost::iterator_range<std::list<EDRDockWidget*>::const_iterator> const_range;

public:
	//! Konstruktor
	//! \param title wyświetlana w tabie nazwa grupy
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetSet(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	//! Typowy konstruktor Qt
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetSet(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetSet() {}

public:
    //! Dodaje dock widget do grupy, jeśli nie ma na niego miejsca rzucany jest wyjątek
    //! \param widget dodawany dock widget
    //! \param area obszar okna (lewy, prawy, góra, dol)
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
    void addDockWidget(EDRDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jeśli nie ma na niego miejsca rzucany jest wyjątek
    //! \param widget dodawany dock widget
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
	void addDockWidget(EDRDockWidget* widget, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jeśli nie ma na niego miejsca rzucany jest wyjątek
    //! \param widget dodawany dock widget
    void addDockWidget(EDRDockWidget* widget);
	//! \return czy można jeszcze dodać dock widget do grpy
	bool isAdditionPossible(EDRDockWidget* widget = nullptr) const;
	//! blokuje lub odblokowuje możliwość dodawania widgetów
	//! \param additionPossible można / nie można dodać nowego widgeta
	void blockAddition(bool additionPossible);
	//! \return liczba dock widgetów w grupie
	int getNumWidgets() const;
	//! \return maksymalna liczba dock widgetów w grupie
	int getMaxWidgetsNumber() const { return maxWidgetsNumber; }
	//! ustawia maksymalna liczbe dock widgetów w grupie
	//! \param val maksymalna liczbe dock widgetów
	void setMaxWidgetsNumber(int val) { maxWidgetsNumber = val; }
    //! \return const range wszystkich dock widgetów w grupie
    const_range getDockWidgets() const { return boost::make_iterator_range(widgetsList.cbegin(), widgetsList.cend()); }

signals:
    //! sygnał oznaczający, ze dock widget podpięty pod grupę został zamknięty
    void dockClosed();

private slots:
    //! dock widget podpięty pod grupę został zamknięty
    //! \param object 
    void onDockWidgetClosed(QObject* object);
    //! dock widget podpięty pod grupę zmienił pozycję
    //! \param area 
    void onDockWidgetLocationChanged(Qt::DockWidgetArea area);

private:
    //! kontener dodawanych dock widgetów
	QMainWindow* mainWindow;
    //! dodane do grupy dock widgety
	std::list<EDRDockWidget*> widgetsList;
    //! maksymalna liczba dock widgetów w grupie
	int maxWidgetsNumber;
    //! czy możliwe jest dalsze dodawanie widgetów (niezalezne od maksymalnej liczby dock widgetów)
	bool additionPossible;
};

#endif
