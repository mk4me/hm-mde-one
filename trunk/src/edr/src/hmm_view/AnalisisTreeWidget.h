/********************************************************************
	created:	2012/06/05
	created:	5:6:2012   9:48
	filename: 	AnalisisTreeWidget.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__ANALISISTREEWIDGET_H__
#define HEADER_GUARD_HMM__ANALISISTREEWIDGET_H__


#include <QtCore/QObject>
#include <QtGui/QTreeWidget>

class QMouseEvent;
class QContextMenuEvent;
class QWidget;

//! Klasa do której trafiaja elementy drzewa danych w analizach
class AnalisisTreeWidget : public QTreeWidget
{
    Q_OBJECT;
public:
    explicit AnalisisTreeWidget(QWidget *parent = 0);

protected:
    //! Obsługa zdarzeń myszki
    //! \param event 
    virtual void mousePressEvent(QMouseEvent *event);
    //! Obsługa zdarzeń menu kontekstowego
    //! \param event 
    virtual void contextMenuEvent ( QContextMenuEvent * event );
};


#endif
