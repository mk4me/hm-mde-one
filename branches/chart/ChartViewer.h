/********************************************************************
	created:  2010/10/27
	created:  27:10:2010   17:12
	filename: ChartViewer.h
	author:	  Micha³ Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTVIEWER_H__
#define __HEADER_GUARD__CHARTVIEWER_H__

#include <QtGui/QWidget>
#include <osgViewer/Viewer>
#include <core/QOSGWidget.h>
#include "ChartDecoration.h"
#include "chart.h"
#include "ChartData.h"
class ChartWidget;

class ChartViewer : public QOSGViewer
{


private:
	ChartWidget* chartWidget;
	int index;
	
	std::string c3dFile;
		osg::Camera* cam;
		int chartCount;
		int cSysCount;
public:
	//void changeView(int index);
	ChartViewer(ChartWidget* chartWidget,int index,std::string c3dFile);
	~ChartViewer();
	Chart* chart;
	Chart* getChart();
	//ChartData* getData(int index);
	//ChartDecoration* getDecoration(int index);
	//void addCSys(int index);
	//int getChartCount();
	//void setChartCount(int chartCount);
	//int getCSysCount();
	//void setCSysCount(int cSysCount);

protected:
	virtual void resizeEvent(QResizeEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
};

#endif  // __HEADER_GUARD__CHARTWIDGET_H__