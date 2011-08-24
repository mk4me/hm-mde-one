/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   14:35
    filename: EDRTitleBar.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRTITLEBAR_H__
#define HEADER_GUARD_CORE__EDRTITLEBAR_H__

#include "ui_EDRTitleBar.h"
#include <QtGui/QWidget>
#include <core/IEDRTitleBar.h>
#include <map>

class EDRTitleBar : public QWidget, public Ui::EDRTitleBar, public IEDRTitleBar
{
    Q_OBJECT

private:
    typedef std::pair<SideType, QWidget*> OBJECT_DESCRIPTOR;
    typedef std::map<QObject*, OBJECT_DESCRIPTOR> OBJECTS_DATA;
    typedef std::map<QWidget*, QObject*> WIDGET_OBJECTS;

public:
    //! \param parent
    EDRTitleBar(QWidget* parent = nullptr);
    virtual ~EDRTitleBar();

public:

    virtual void addObject(QObject * object, SideType side);
    virtual void removeObject(QObject * object);
    
    virtual void clearSide(SideType side);
    virtual void clear();

    bool isCloseButtonVisible() const;
    void setCloseButtonVisible(bool visible);

    bool isFloatButtonVisible() const;
    void setFloatButtonVisible(bool visible);

protected:
    virtual void paintEvent(QPaintEvent *paintEvent);

private:
    QFrame * getSideFrame(SideType side) const;

private:

    OBJECTS_DATA objectData;
    WIDGET_OBJECTS widgetObjects;
};

#endif  //  HEADER_GUARD_CORE__EDRTITLEBAR_H__