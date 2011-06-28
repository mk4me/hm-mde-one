/********************************************************************
    created:  2011/05/04
    created:  4:5:2011   8:38
    filename: EDRConsoleInnerWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRCONSOLEINNERWIDGET_H__
#define HEADER_GUARD_CORE__EDRCONSOLEINNERWIDGET_H__

#include "ui_EDRConsoleInnerWidget.h"

class EDRConsoleInnerWidget : public QWidget, public Ui::EDRConsoleInnerWidget
{
    Q_OBJECT

public:

    EDRConsoleInnerWidget(QWidget * parent = nullptr);
    ~EDRConsoleInnerWidget();
};

#endif  //  HEADER_GUARD_CORE__EDRCONSOLEINNERWIDGET_H__

