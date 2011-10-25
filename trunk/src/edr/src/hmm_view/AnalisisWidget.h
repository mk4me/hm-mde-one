/********************************************************************
	created:	2011/10/21
	created:	21:10:2011   11:09
	filename: 	AnalisisWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__ANALISISWIDGET_H__
#define HEADER_GUARD_HMM__ANALISISWIDGET_H__

#include "ui_AnalisisWidget.h"

class AnalisisWidget : public QWidget, public Ui::AnalisisWidget
{
public:
    AnalisisWidget(QWidget* parent, Qt::WindowFlags flags = 0) : 
      QWidget(parent, flags)
      {
          setupUi(this);
      }
	virtual ~AnalisisWidget() {}

public:
    QTreeWidget* getTreeWidget() { return this->treeWidget; }
    QTabWidget * getFilterTabWidget() { return filterTabWidget; }
    QWidget* getArea() { return analisisArea; }
};


#endif
