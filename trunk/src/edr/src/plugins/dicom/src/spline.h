/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/
#ifndef HEADER_GUARD_DICOM__SPLINE_H__
#define HEADER_GUARD_DICOM__SPLINE_H__



#include "qwt_global.h"
#include <qpolygon.h>
#include <qrect.h>
#include <qwt_spline.h>

//! Jest to nieco przerobiona klasa QwtSplineCurveFitter. Dodano do niej wsparcie dla zamkniêtych polygonów.
class SplineCurveFitter
{
public:

    SplineCurveFitter();
    virtual ~SplineCurveFitter();

    void setSpline( const QwtSpline& );
    const QwtSpline &spline() const;
    QwtSpline &spline();

    void setSplineSize( int size );
    int splineSize() const;

    virtual QPolygonF fitCurve( const QPolygonF & ) const;

private:
    QPolygonF fitParametric( const QPolygonF & , bool) const;

    class PrivateData;
    PrivateData *d_data;
};

#endif
