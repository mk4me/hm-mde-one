/********************************************************************
	created:	2011/09/20
	created:	20:9:2011   8:37
	filename: 	EDRDockWidgetManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EDRDOCKWIDGETMANAGER_H__
#define HEADER_GUARD_HMM__EDRDOCKWIDGETMANAGER_H__

#include <boost/range.hpp>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>

class EDRDockWidget;
class EDRDockWidgetSet;

//! Klasa zarz�dza calymi grupami dock widgetow, laczac je w sety
class EDRDockWidgetManager : public QMainWindow
{
    Q_OBJECT;
public:
    typedef boost::iterator_range<std::list<EDRDockWidgetSet*>::iterator> range;
    typedef boost::iterator_range<std::list<EDRDockWidgetSet*>::const_iterator> const_range;

public:
	//! typowy konstruktor Qt 
	//! \param parent 
	//! \param flags 
	explicit EDRDockWidgetManager(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetManager() {}

public:
	//! Dodaj grupe dock widgetow
	//! \param set dodawana grupa
	void addDockWidgetSet(EDRDockWidgetSet* set);
    //! Grupa dock widgetow stanie si� aktywna (widoczna)
    //! \param set 
    void raiseSet( EDRDockWidgetSet* set );
    //! Dodaje widget do grupy, kt�ra ma na niego miejsce, je�li takiej nie ma to tworzona jest nowa
    //! \param widget dodawany dock widget
    void autoAddDockWidget(EDRDockWidget* widget);
    //! ustawia pozycje belki z tabami
    //! \param tabPosition pozycja N,S,W,E
    void setTabsPosition(QTabWidget::TabPosition tabPosition);
    //! \return const range z wszystkimi grupami dock widgetow
    const_range getDockSet() const { return boost::make_iterator_range(dockList.cbegin(), dockList.cend()); }
    //! \return range z wszystkimi grupami dock widgetow
    range getDockSet() { return boost::make_iterator_range(dockList.begin(), dockList.end()); }
    //! Na podstawie dock widgeta pobierana jest grupa, do kt�rej nale�y
    //! \param widget badany dock widget
    //! \return odnaleziona grupa lub nullptr
    EDRDockWidgetSet* tryGetDockSet(EDRDockWidget* widget);

private:
    //! iteruje po wszystkich grupach zmieniajac im nazwy
    void setTabNames();

signals:
    //! stan dock widgetow zmieni� si�
    void changed();

private slots:
    //! gdy grupa zostaje zamknieta
    //! \param object 
    void onSetClosed(QObject* object);
    //! gdy pojedynczy dock widget zostaje zamkniety
    void dockClosed();

private:
    //! lista wszystkich grup managera
	std::list<EDRDockWidgetSet*> dockList;
    //! lista grup, kt�ry zosta�y stworzone przez manager
	std::list<EDRDockWidgetSet*> generatedList;
};

#endif
