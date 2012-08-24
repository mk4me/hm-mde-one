/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   21:09
	filename: 	NewChartDot.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTDOT_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTDOT_H__

#include <plugins/c3d/C3DChannels.h>

//! reprezentuje punkt na wykresie
class NewChartDot : public QwtPlotItem
{
public:
    //! Konstruktor
    //! \param size wielkoœæ punktu 
    NewChartDot( int size = 2);

public:
    //! odrysowanie punktu
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    //! \return pozycja puntku w wspó³rzêdnych ekranowych
    virtual QPointF getPosition() const = 0;
    //! ustawia pozycje
    //! \param val nowa pozcja w wspó³rzêdnych ekranowych
    virtual void setPosition(const QPointF& val)  = 0;
    //! \return zwraca pêdzel u¿ywany do rysowania punktu
    const QBrush& getBrush() const { return brush; }
    //! ustawia pêdzel
    //! \param val pêdzel u¿ywany do rysowania punktu
    void setBrush(const QBrush& val) { brush = val; }
    //! \return pióro u¿ywane do rysowania punktu
    const QPen& getPen() const { return pen; }
    //! ustawia pióro
    //! \param val pióro u¿ywane do rysowania punktu
    void setPen(const QPen& val) { pen = val; }
    //! Emulacja rtti obs³ugiwana przez Qwt
    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }

private:
    //! rozmiar punktu
    int size;
    //! pióro punktu
    QPen pen;
    //! pêdzel punktu
    QBrush brush;
};
typedef NewChartDot* NewChartDotPtr;
typedef const NewChartDot* NewChartDotConstPtr;

#endif
