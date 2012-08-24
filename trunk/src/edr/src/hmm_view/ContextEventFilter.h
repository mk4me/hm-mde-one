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

//! filtruje zmiany focus�w widget�w, obs�uguje zdarzenia dla kontekst�w, wywo�uje przydzielanie kontekstu dla odpowiedniego widgetu
class ContextEventFilter : public QObject
{
    Q_OBJECT
public:
    //! konstruktor, przyjmuje na w�asno�� g��wne okno
    //! \param mainWindow potrzebne, aby uzyska� dost�p do kontekst�w
    ContextEventFilter(HmmMainWindow * mainWindow);
    //! destruktor
    virtual ~ContextEventFilter() {}

public:
    //! rejestruje widget, kt�remu przekazywany b�dzie focus
    //! \param widget  widget, kt�ry nie zostanie zniszczony
    void registerPermamentContextWidget(QWidget * widget);
    //! rejestruje widget, kt�remu przekazywany b�dzie focus
    //! \param widget widget, kt�ry trzeba wyrejstrowa� z systemu w przypadku jego zamkni�cia 
    void registerClosableContextWidget(QWidget * widget);
    //! usuwa widget z kolekcji
    //! \param widget usuwany widget
    void unregisterPermamentContextWidget(QWidget * widget);
    //! usuwa widget z kolekcji
    //! \param widget usuwany widget
    void unregisterClosableContextWidget(QWidget * widget);

protected:
    //! filtruje zdarzenia dla zarejstrowanych widget�w, 
    //! dzi�ki tej metodzie wysy�ane s� informacje, dla jakich widget�w powinien zosta� aktywowany kontekst
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
