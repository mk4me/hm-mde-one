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

#include <core/QOSGWidget.h>

	
#include "ui_ChartWidget.h"

class ChartViewer;
class ChartWidget : public QWidget, public Ui::ChartWidget
{
    Q_OBJECT

private:
    //! Us³uga tworz¹ca widget.
	IService* service;

	ChartViewer* mainViewer;
	std::string c3dFile;
	void createEMGChanels();
	void createGRFChanels();
	void createChart(QOSGViewer* viewer,int chartIndex);
	QWidget* actualWidget;

public:
    //! 
	ChartWidget(IService* service);
    //! 
    virtual ~ChartWidget();
	void createLargeChart(int chartIndex);
	ChartViewer* getViewer();
public slots:
    //! \param 
   void comboBoxChanged(int index);
   
};

#endif  // __HEADER_GUARD__CHARTWIDGET_H__