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
      QWidget(parent, flags),
      filterWidth(-1), filterHeight(-1)
      {
          setupUi(this);
      }
	virtual ~AnalisisWidget() {}

public:
    QTreeWidget* getTreeWidget() { return this->treeWidget; }
    void addDataFilterWidget(DataFilterWidget* filter)
    {
        if (filterWidth < 0 && filterHeight < 0) {
            filterWidth = filter->width();
            filterHeight = filter->height();
        }

        UTILS_ASSERT(filterHeight == filter->height() && filterWidth == filter->width());
        

        int count = filterScroll->children().size();
        int x = count % 2;
        int y = count / 2;

        if (x == 0) {
            filterScroll->setMinimumSize(filterWidth * 2, filterHeight * (y + 1));
        }

        filter->setParent(filterScroll);
        filter->setGeometry(x * filterWidth, y * filterHeight, filterWidth, filterHeight);
    }
    //QWidget * getFilterTabWidget() { return filterScroll; }
    QWidget* getArea() { return analisisArea; }

private:
    int filterWidth;
    int filterHeight;
};



#endif
