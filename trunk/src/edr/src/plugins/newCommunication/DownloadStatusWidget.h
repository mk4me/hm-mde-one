/********************************************************************
    created:  2012/03/18
    created:  18:3:2012   23:20
    filename: DownloadStatusWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DOWNLOADSTATUSWIDGET_H__
#define HEADER_GUARD___DOWNLOADSTATUSWIDGET_H__

#include "ui_DownloadStatusWidget.h"

//! Klasa widgeta obs³uguj¹cego progress pobierania
class DownloadStatusWidget : public QWidget, public Ui::DownloadStatusWidget
{
	Q_OBJECT

public:
	//! domyœlny konstruktor
	DownloadStatusWidget(QWidget * parent = nullptr);
	//! Wirtualny destruktor
	virtual ~DownloadStatusWidget();
};

#endif	//	HEADER_GUARD___DOWNLOADSTATUSWIDGET_H__
