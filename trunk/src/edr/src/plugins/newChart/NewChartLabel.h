/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   21:10
	filename: 	NewChartLabel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLABEL_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLABEL_H__

#include <plugins/c3d/C3DChannels.h>
#include "NewChartDot.h"

//! Klasa reprezentuje etykiete na wykresie
class NewChartLabel : public QwtPlotItem
{
public:
    //! styl po³¹czeñ miêdzy wskazywanymi punktami a etykieta
    enum ConnectionStyle
    {
        Simple,       //!< proste strzalki
        Horizontal,   //!< strzalki lamane, najpierw poziome
        Vertical      //!< strzalki lamane, najpierw pionowe
    };
public:
    //! Konstruktor
    //! \param text tekst etykiety
    //! \param shift domyslne przesuniecie (miêdzy wsk. punktem i etykieta)
    //! \param size rozmiar etykiety
    NewChartLabel(const QString& text = QString(""), const QPoint& shift = QPoint(20, 20), const QPoint& size = QPoint(80, 35));

public:
    //! Odrysowanie etykiety
    //! \param painter obiekt paintera
    //! \param xMap transformacja X
    //! \param yMap transformacja Y
    //! \param canvasRect obszar do rysowania
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    //! Sprawdza, czy punkt znajduje siê wewn¹trz etykiety
    //! \param transformedPoint przetransformowany punkt
    //! \param curve krzywa, wykorzystuje siê jej transformacjê
    bool isInsideLabel(const QPoint& transformedPoint, const QwtPlotCurve* curve) const;
    //! \return tekst etykiety
    const QString& getText() const { return text; }
    //! ustawia tekst etykiety
    //! \param val nowy tekst
    void setText(const QString& val) { text = val; }
    //! \return przeusniecie miêdzy wskazywanym punktem, a etykieta
    const QPoint& getShift() const { return shift; }
    //! ustawia przeusniecie miêdzy wskazywanym punktem, a etykieta
    //! \param val nowe przesuniecie
    void setShift(const QPoint& val) { shift = val; }
    //! \return pierwszy punkt podpiêty pod etykiete
    NewChartDotConstPtr getPoint1() const { return point1; }
    //! \return przetransformowana ju¿ na podstawie krzywej pozycja pierwszego punktu
    QPoint getPoint1Transformed(const QwtPlotCurve* curve) const;
    //! ustawia pierwszy punkt podpiêty pod etykiete
    //! \param val ustawiany punkt
    void setPoint1(NewChartDotConstPtr val) { point1 = val; }
    //! \return drugi punkt podpiêty pod etykiete (mo¿e byæ nullptr)
    NewChartDotConstPtr getPoint2() const { return point2; }
    //! \return przetransformowana ju¿ na podstawie krzywej pozycja drugiego punktu
    QPoint getPoint2Transformed(const QwtPlotCurve* curve) const;
    //! ustawia drugi punkt podpiêty pod etykiete 
    //! \param val drugi punkt (mo¿e byæ nullptr)
    void setPoint2(NewChartDotConstPtr val) { point2 = val; }
    //! 
    const QBrush& getBrush() const { return brush; }
    //! 
    //! \param val 
    void setBrush(const QBrush& val) { brush = val; }
    //! 
    const QPen& getPen() const { return pen; }
    //! 
    //! \param val 
    void setPen(const QPen& val) { pen = val; }
    //! Emulacja rtti wykorzystywana przez Qwt
    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }
    //! laczy punkt z etykieta
    //! \param dot podlaczany punkt
    //! \param style styl po³¹czenia
    void connectDot(NewChartDotConstPtr dot, ConnectionStyle style = Simple);
    //! laczy dwa punkty z etykieta
    //! \param point1 pierwszy podlaczany punkt
    //! \param point2 drugi podlaczany punkt
    //! \param style styl po³¹czenia
    void connectDots(NewChartDotConstPtr point1, NewChartDotConstPtr point2, ConnectionStyle style );
    //! tworzy a nastepnie laczy dwa punkty z etykieta
    //! \param point1 pierwszy podlaczany punkt
    //! \param point2 drugi podlaczany punkt
    //! \param style styl po³¹czenia
    void connectDots(const QPointF& point1, const QPointF& point2, ConnectionStyle style);

private:
    //! Rysowanie po³¹czenia
    //! \param painter 
    //! \param box 
    //! \param transformedTo 
    //! \param style 
    //! \param arrowOutside 
    void drawConnection(QPainter* painter, const QRect& box, const QPoint& transformedTo, ConnectionStyle style, bool arrowOutside = false) const;
    //! Rysowanie strzalki
    //! \param painter 
    //! \param transformedFrom 
    //! \param transformedTo 
    //! \param outside 
    void drawArrow(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, bool outside = false) const;

private:
    //! tekst etykiety
    QString text;
    //! rozmiar etykiety
    QPoint size;
    //! przesuniecie miêdzy wskazywanym punktem a etykieta
    QPoint shift;
    //! pioro 
    QPen pen;
    //! pedzel 
    QBrush brush;
    //! pierwszy podpiêty punkt
    NewChartDotConstPtr point1;
    //! drugi podpiêty punkt (mo¿e byæ nullptr)
    NewChartDotConstPtr point2;
    //! styl laczenia etykiety z punktami
    ConnectionStyle connectionStyle;
};
typedef NewChartLabel* NewChartLabelPtr;
typedef const NewChartLabel* NewChartLabelConstPtr;


#endif
