/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   16:31
    filename: EDRInnerWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRINNERWIDGET_H__
#define HEADER_GUARD_CORE__EDRINNERWIDGET_H__

#include "ui_EDRInnerWidget.h"
#include <QtGui/QWidget>

class EDRInnerWidget : public QWidget, public Ui::EDRInnerWidget
{
    Q_OBJECT

public:
    EDRInnerWidget(QWidget * parent = nullptr);
    ~EDRInnerWidget();

};

#endif  //  HEADER_GUARD_CORE__EDRINNERWIDGET_H__

