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

class ContextEventFilter : public QObject
{
    Q_OBJECT
public:
    ContextEventFilter(HmmMainWindow * mainWindow);
    virtual ~ContextEventFilter() {}

public:
    void registerPermamentContextWidget(QWidget * widget);
    void registerClosableContextWidget(QWidget * widget);
    void unregisterPermamentContextWidget(QWidget * widget);
    void unregisterClosableContextWidget(QWidget * widget);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    HmmMainWindow * mainWindow;
    std::set<QWidget*> permamentWidgets;
    std::set<QWidget*> closableWidgets;
};


#endif
