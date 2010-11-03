/********************************************************************
	created:  2010/10/27
	created:  27:10:2010   17:12
	filename: ChartWidget.h
	author:	  Micha³ Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTVIEWER_H__
#define __HEADER_GUARD__CHARTVIEWER_H__

#include <QtGui/QWidget>
#include <osgViewer/Viewer>
#include <core/QOSGWidget.h>
#include "ChartDecoration.h"
#include "LineChart.h"
#include "ChartData.h"
class ChartWidget;

class ChartViewer : public QOSGViewer
{


private:
	ChartWidget* chartWidget;
	int index;
public:

	ChartViewer(ChartWidget* chartWidget,int index,std::string c3dFile);
	~ChartViewer();

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
};

#endif  // __HEADER_GUARD__CHARTWIDGET_H__