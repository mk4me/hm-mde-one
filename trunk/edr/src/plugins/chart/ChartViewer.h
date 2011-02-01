/********************************************************************
	created:  2010/10/27
	created:  27:10:2010   17:12
	filename: ChartViewer.h
	author:	  Micha³ Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTVIEWER_H__
#define __HEADER_GUARD__CHARTVIEWER_H__


#include <osgViewer/Viewer>
#include <core/QOsgWidgets.h>

class Chart;
class ChartWidget;

class ChartViewer : public core::QOsgViewer
{


private:
	ChartWidget* chartWidget;
	int index;
	
	std::string c3dFile;
		osg::ref_ptr<osg::Camera> cam;
		int chartCount;
		int cSysCount;
public:

	ChartViewer(ChartWidget* chartWidget,int index);
	~ChartViewer();
	Chart* chart;
	Chart* getChart();


protected:

};

#endif  // __HEADER_GUARD__CHARTWIDGET_H__