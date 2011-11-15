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
    AnalisisWidget(QWidget* parent, int margin = 2, Qt::WindowFlags flags = 0) : 
      QWidget(parent, flags),
      margin(margin),
      filterWidth(-1), filterHeight(-1)
      {
          setupUi(this);
      }
	virtual ~AnalisisWidget() {}

public:
    QTreeWidget* getTreeWidget() { return this->treeWidget; }

    void setActivePixmapAndText(const QPixmap& pixmap, const QString& name) 
    {
        label->setText(name);
        picture->setPixmap(pixmap);
    }
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
            int w = 3 * margin + filterWidth * 2;
            int h = 2 * margin + (filterHeight + margin) * (y + 1);
            filterScroll->setMinimumSize(w, h);
            scrollArea->setMinimumWidth(w + 16);
            scrollArea->setMaximumWidth(w + 16);
            frame->setMaximumWidth     (w + 16);
            scrollArea->setMinimumHeight(3 * margin + filterHeight * 2 + 8);
        }

        filter->setParent(filterScroll);
        filter->setGeometry(margin + x * (filterWidth + margin),margin +  y * (margin + filterHeight), filterWidth, filterHeight);
    }

    QWidget* getArea() { return analisisArea; }

private:
    int filterWidth;
    int filterHeight;
    int margin;
};



#endif
