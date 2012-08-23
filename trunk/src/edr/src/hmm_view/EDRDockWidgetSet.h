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

//! Klasa obs³uguje grupe dock widgetów, sama bêd¹c dock widgetem
class EDRDockWidgetSet : public EDRDockWidget
{
	Q_OBJECT;
public:
    typedef boost::iterator_range<std::list<EDRDockWidget*>::const_iterator> const_range;

public:
	//! Konstruktor
	//! \param title wyœwietlana w tabie nazwa grupy
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetSet(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	//! Typowy konstruktor Qt
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetSet(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetSet() {}

public:
    //! Dodaje dock widget do grupy, jeœli nie ma na niego miejsca rzucany jest wyj¹tek
    //! \param widget dodawany dock widget
    //! \param area obszar okna (lewy, prawy, gora dol)
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
    void addDockWidget(EDRDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jeœli nie ma na niego miejsca rzucany jest wyj¹tek
    //! \param widget dodawany dock widget
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
	void addDockWidget(EDRDockWidget* widget, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jeœli nie ma na niego miejsca rzucany jest wyj¹tek
    //! \param widget dodawany dock widget
    void addDockWidget(EDRDockWidget* widget);
	//! \return czy mo¿na jeszcze dodaæ dock widget do grpy
	bool isAdditionPossible(EDRDockWidget* widget = nullptr) const;
	//! blokuje lub odblokowuje mo¿liwoœæ dodawania widgetów
	//! \param additionPossible 
	void blockAddition(bool additionPossible);
	//! \return liczba dock widgetów w grupie
	int getNumWidgets() const;
	//! \return maksymalna liczba dock widgetów w grupie
	int getMaxWidgetsNumber() const { return maxWidgetsNumber; }
	//! ustawia maksymalna liczbe dock widgetów w grupie
	//! \param val 
	void setMaxWidgetsNumber(int val) { maxWidgetsNumber = val; }
    //! \return const range wszystkich dock widgetów w grupie
    const_range getDockWidgets() const { return boost::make_iterator_range(widgetsList.cbegin(), widgetsList.cend()); }

signals:
    //! sygnal oznaczaj¹cy, ze dock widget podpiêty pod grupe zosta³ zamkniety
    void dockClosed();

private slots:
    //! dock widget podpiêty pod grupe zosta³ zamkniety
    //! \param object 
    void onDockWidgetClosed(QObject* object);
    //! dock widget podpiêty pod grupe zmieni³ pozycje
    //! \param area 
    void onDockWidgetLocationChanged(Qt::DockWidgetArea area);

private:
    //! kontener dodawanych dock widgetów
	QMainWindow* mainWindow;
    //! dodane do grupy dock widgety
	std::list<EDRDockWidget*> widgetsList;
    //! maksymalna liczba dock widgetów w grupie
	int maxWidgetsNumber;
    //! czy mo¿liwe jest dalsze dodawanie widgetow (niezalezne od maksymalnej liczby dock widgetów)
	bool additionPossible;
};

#endif
