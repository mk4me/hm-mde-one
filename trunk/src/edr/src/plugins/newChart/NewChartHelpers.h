/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   13:14
	filename: 	NewChartHelpers.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTHELPERS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTHELPERS_H__

#include <boost/tuple/tuple.hpp>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>

class LabeledSpinbox
{
public:
    static boost::tuple<QWidget*, QLabel*, QDoubleSpinBox*> create(QWidget* parent, const QString& label, double singleStep, double minimum, double maximum);
};


#endif
