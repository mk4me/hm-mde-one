/********************************************************************
	created:	2013/09/30
	created:	30:9:2013   16:48
	filename: 	MergedWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_VDF__MERGEDWIDGET_H__
#define HEADER_GUARD_NEW_VDF__MERGEDWIDGET_H__

#include <QtGui/QWidget>

class QListWidget;

namespace vdf {

//! Widget wy�wietla list� zmergowanych element�w sceny.
class MergedWidget : public QWidget
{
    Q_OBJECT
public:
    MergedWidget();
	virtual ~MergedWidget() {}
private:
    QListWidget* listWidget;
};
}

#endif
