/********************************************************************
	created:	2012/06/04
	created:	4:6:2012   20:00
	filename: 	ContextEventFilter.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONTEXTEVENTFILTER_H__
#define HEADER_GUARD_HMM__CONTEXTEVENTFILTER_H__

#include <set>

class HmmMainWindow;
#include<QtGui/QWidget>
#include<QtCore/QObject>

//! filtruje zmiany focusów widgetów, obs³uguje zdarzenia dla kontekstów, wywo³uje przydzielanie kontekstu dla odpowiedniego widgetu
class ContextEventFilter : public QObject
{
    Q_OBJECT
public:
    //! konstruktor, przyjmuje na w³asnoœæ g³ówne okno
    //! \param mainWindow potrzebne, aby uzyskaæ dostêp do kontekstów
    ContextEventFilter(HmmMainWindow * mainWindow);
    //! destruktor
    virtual ~ContextEventFilter() {}

public:
    //! rejestruje widget, któremu przekazywany bêdzie focus
    //! \param widget  widget, który nie zostanie zniszczony
    void registerPermamentContextWidget(QWidget * widget);
    //! rejestruje widget, któremu przekazywany bêdzie focus
    //! \param widget widget, który trzeba wyrejstrowaæ z systemu w przypadku jego zamkniêcia 
    void registerClosableContextWidget(QWidget * widget);
    //! usuwa widget z kolekcji
    //! \param widget usuwany widget
    void unregisterPermamentContextWidget(QWidget * widget);
    //! usuwa widget z kolekcji
    //! \param widget usuwany widget
    void unregisterClosableContextWidget(QWidget * widget);

protected:
    //! filtruje zdarzenia dla zarejstrowanych widgetów, 
    //! dziêki tej metodzie wysy³ane s¹ informacje, dla jakich widgetów powinien zostaæ aktywowany kontekst
    //! \param obj 
    //! \param event 
    bool eventFilter(QObject *obj, QEvent *event);

private:
    //! 
    HmmMainWindow * mainWindow;
    //! 
    std::set<QWidget*> permamentWidgets;
    //! 
    std::set<QWidget*> closableWidgets;
};


#endif
