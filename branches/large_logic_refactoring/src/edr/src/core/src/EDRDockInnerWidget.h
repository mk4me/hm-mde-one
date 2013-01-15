/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   16:31
    filename: EDRDockInnerWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRINNERWIDGET_H__
#define HEADER_GUARD_CORE__EDRINNERWIDGET_H__

#include "ui_EDRDockInnerWidget.h"
#include <QtGui/QWidget>

class EDRDockInnerWidget : public QWidget, public Ui::EDRDockInnerWidget
{
    Q_OBJECT

public:
    EDRDockInnerWidget(QWidget * parent = nullptr);
    ~EDRDockInnerWidget();

};

#endif  //  HEADER_GUARD_CORE__EDRINNERWIDGET_H__

