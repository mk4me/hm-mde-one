/********************************************************************
	created:  2010/10/27
	created:  27:10:2010   17:12
	filename: ChartWidget.h
	author:	  Micha³ Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTWIDGET_H__
#define __HEADER_GUARD__CHARTWIDGET_H__

#include <QtGui/QWidget>
#include <plugins/chart/ChartService.h>
#include "ui_ChartWidget.h"

class ChartWidget : public QWidget, public Ui::ChartWidget
{
    Q_OBJECT

private:
    //! Us³uga tworz¹ca widget.
    ChartService* service;

public:
    //! 
    ChartWidget(ChartService* service);
    //! 
    virtual ~ChartWidget();

public slots:
    //! \param 
   

};

#endif  // __HEADER_GUARD__CHARTWIDGET_H__