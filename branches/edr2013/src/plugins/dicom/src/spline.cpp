/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "spline.h"
#include "qwt_math.h"
#include "qwt_spline.h"
#include <qstack.h>
#include <qvector.h>


class SplineCurveFitter::PrivateData
{
public:
    PrivateData():
        splineSize( 250 )
    {
    }

    QwtSpline spline;
    int splineSize;
};

//! Constructor
SplineCurveFitter::SplineCurveFitter()
{
    d_data = new PrivateData;
}

//! Destructor
SplineCurveFitter::~SplineCurveFitter()
{
    delete d_data;
}



/*!
  Assign a spline

  \param spline Spline
  \sa spline()
*/
void SplineCurveFitter::setSpline( const QwtSpline &spline )
{
    d_data->spline = spline;
    d_data->spline.reset();
}

/*!
  \return Spline
  \sa setSpline()
*/
const QwtSpline &SplineCurveFitter::spline() const
{
    return d_data->spline;
}

/*!
  \return Spline
  \sa setSpline()
*/
QwtSpline &SplineCurveFitter::spline()
{
    return d_data->spline;
}

/*!
   Assign a spline size ( has to be at least 10 points )

   \param splineSize Spline size
   \sa splineSize()
*/
void SplineCurveFitter::setSplineSize( int splineSize )
{
    d_data->splineSize = qMax( splineSize, 10 );
}

/*!
  \return Spline size
  \sa setSplineSize()
*/
int SplineCurveFitter::splineSize() const
{
    return d_data->splineSize;
}

/*!
  Find a curve which has the best fit to a series of data points

  \param points Series of data points
  \return Curve points
*/
QPolygonF SplineCurveFitter::fitCurve( const QPolygonF &points ) const
{
    const int size = points.size();
    if ( size <= 2 )
        return points;

    if (points.isClosed()) {
        QPolygonF points2 = points;
        points2.push_front(points.data()[size - 2]);
        points2.push_back(points.data()[1]);
        return fitParametric( points2, true );
    }

    return fitParametric( points, false );
}


QPolygonF SplineCurveFitter::fitParametric( const QPolygonF &points , bool closed) const
{
    int i;

    const int size = points.size();
    const QPointF *p = points.data();

    

    QPolygonF fittedPoints( d_data->splineSize );
    QPolygonF splinePointsX( size );
    QPolygonF splinePointsY( size );

    QPointF *spX = splinePointsX.data();
    QPointF *spY = splinePointsY.data();

    double param = 0.0;
    for ( i = 0; i < size; i++ )
    {
        const double x = p[i].x();
        const double y = p[i].y();
        if ( i > 0 ) 
        {
            const double delta = qSqrt( qwtSqr( x - spX[i-1].y() )
                      + qwtSqr( y - spY[i-1].y() ) );
            param += qMax( delta, 1.0 );
        }
        spX[i].setX( param );
        spX[i].setY( x );
        spY[i].setX( param );
        spY[i].setY( y );
    }

    d_data->spline.setPoints( splinePointsX );
    if ( !d_data->spline.isValid() )
        return points;

    double deltaX, offsetX; 
    double deltaY, offsetY;
    if (closed) {
        offsetX = splinePointsX[1].x();
        double endX = splinePointsX[size - 2].x();
        deltaX = (endX - offsetX) / ( d_data->splineSize - 1 );

        offsetY = splinePointsY[1].x();
        double endY = splinePointsY[size - 2].x();
        deltaY = (endY - offsetY) / ( d_data->splineSize - 1 );
    } else {
        deltaX = splinePointsX[size - 1].x() / ( d_data->splineSize - 1 );
        deltaY = splinePointsY[size - 1].x() / ( d_data->splineSize - 1 );
        offsetX = offsetY = 0.0;
    }
    for ( i = 0; i < d_data->splineSize; i++ )
    {
        const double dtmp = offsetX + i * deltaX;
        fittedPoints[i].setX( d_data->spline.value( dtmp ) );
    }

    d_data->spline.setPoints( splinePointsY );
    if ( !d_data->spline.isValid() )
        return points;

    for ( i = 0; i < d_data->splineSize; i++ )
    {
        const double dtmp = offsetX + i * deltaY;
        fittedPoints[i].setY( d_data->spline.value( dtmp ) );
    }

    return fittedPoints;
}
