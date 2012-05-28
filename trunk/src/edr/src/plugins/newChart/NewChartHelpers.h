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

//! Klasa tworzy widget z dwoma potomkami : QLabel i QDoubleSpinBox
class LabeledSpinbox
{
public:
    //! Tworzy etykietowany spinbox
    //! \param parent rodzic widgeta
    //! \param label tekst etykiety
    //! \param singleStep krok dla spinboxa
    //! \param minimum minimalna wartosc spinboxa
    //! \param maximum maksymalna wartosc spinboxa
    //! \return krotka z utworzonymi widgetami (kontener, etykieta, spinbox)
    static boost::tuple<QWidget*, QLabel*, QDoubleSpinBox*> 
        create(QWidget* parent, const QString& label, double singleStep, double minimum, double maximum);
};


#endif
