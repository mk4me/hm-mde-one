/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   21:10
	filename: 	NewChartDotFloating.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTDOTFLOATING_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTDOTFLOATING_H__

#include <plugins/c3d/C3DChannels.h>
#include "NewChartDot.h"

class NewChartSerie;
//! Punkt, kt�rego pozycja zmienia si� wraz z wykresem
class NewChartDotFloating : public NewChartDot
{
public:
    //! Konstruktor
    //! \param position pozycja na wykresie
    //! \param relatedSerie seria powiazana z punktem, u�ywane s� jej transformacje 
    //! \param size wielko�� punktu
    NewChartDotFloating(const QPointF& position, const NewChartSerie* relatedSerie, int size = 2);

public:
    //! \return pozycja z uwzgl�dnieniem transformacji serii danych
    virtual QPointF getPosition() const;
    //! ustawia pozycje z uwzgl�dnieniem transformacji serii danych
    //! \param val wsp�rz�dne ekranowe
    virtual void setPosition(const QPointF& val);

private:
    //! pozycja obiektu
    QPointF position;
    //! przechowywana seria danych, z kt�rej pobiera si� transformacj�
    const NewChartSerie* relatedSerie;
};

#endif
