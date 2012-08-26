/********************************************************************
    created:  2011/04/27
    created:  27:4:2011   9:59
    filename: VdfCustomQOsgWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___VDFCUSTOMQOSGWIDGET_H__
#define HEADER_GUARD___VDFCUSTOMQOSGWIDGET_H__

#include <osgui/QOsgWidgets.h>

class VdfCustomQOsgWidget : public osgui::QOsgDefaultWidget
{
    Q_OBJECT

public:
    VdfCustomQOsgWidget(QWidget * parent = 0, const osg::GraphicsContext::Traits* traits = 0, Qt::WindowFlags f = 0);

protected:

    virtual void mousePressEvent(QMouseEvent * event);
};

#endif  //  HEADER_GUARD___VDFCUSTOMQOSGWIDGET_H__
