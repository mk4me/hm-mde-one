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
#include <core/IService.h>

#include "C3DChartData.h"
#include <core/QOSGWidget.h>
#include <core/Chart.h>
#include <core/MultiView.h>	
#include "ui_ChartWidget.h"

class ChartViewer;
class ChartWidget : public QWidget, public Ui::ChartWidget
{
    Q_OBJECT

private:
    //! Us³uga tworz¹ca widget.
	IService* service;
    //! Widget z widokiem OSG.
    QOSGViewer* viewer;

	std::string c3dFile;
	QWidget* actualWidget;
    //! Multi view.
    osg::ref_ptr<core::MultiView> multiView;
	std::vector<Chart*> previewItem;
	std::vector<Chart*> item;
	std::vector<Chart*>::iterator itItem;
	std::vector<Chart*>::iterator itPItem;
public:


    //! 
	ChartWidget(IService* service);
    //! 
    virtual ~ChartWidget();


	void addChart(C3DChartData* c3dData);
void update(double targetTime);
   double getLenght();
};

#endif  // __HEADER_GUARD__CHARTWIDGET_H__