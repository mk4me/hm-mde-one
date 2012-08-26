/********************************************************************
    created:  2011/12/15
    created:  15:12:2011   10:08
    filename: FlexiWidgetTabContentWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_QT_EXT__FLEXIWIDGETTABCONTENTWIDGET_H__
#define HEADER_GUARD_QT_EXT__FLEXIWIDGETTABCONTENTWIDGET_H__

#include "ui_FlexiWidgetTabContentWidget.h"
#include <QtGui/QWidget>

class FlexiWidgetTabContentWidget : public QWidget, public Ui::FlexiWidgetTabContentWidget
{
    Q_OBJECT;

public:
    FlexiWidgetTabContentWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
};

#endif HEADER_GUARD_QT_EXT__FLEXIWIDGETTABCONTENTWIDGET_H__
