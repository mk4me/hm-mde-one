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

//! Klasa reprezentuje etykietê na wykresie
class NewChartLabel : public QwtPlotItem
{
public:
    //! styl po³¹czeñ miêdzy wskazywanymi punktami a etykiet¹
    enum ConnectionStyle
    {
        Simple,       //!< proste strza³ki
        Horizontal,   //!< strza³ki ³amane, najpierw poziome
        Vertical      //!< strza³ki ³amane, najpierw pionowe
    };
public:
    //! Konstruktor
    //! \param text tekst etykiety
    //! \param shift domyœlne przesuniêcie (miêdzy wsk. punktem i etykiet¹)
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
    //! \return przeusniecie miêdzy wskazywanym punktem, a etykiet¹
    const QPoint& getShift() const { return shift; }
    //! ustawia przeusniecie miêdzy wskazywanym punktem, a etykiet¹
    //! \param val nowe przesuniêcie
    void setShift(const QPoint& val) { shift = val; }
    //! \return pierwszy punkt podpiêty pod etykietê
    NewChartDotConstPtr getPoint1() const { return point1; }
    //! \return przetransformowana ju¿ na podstawie krzywej pozycja pierwszego punktu
    QPoint getPoint1Transformed(const QwtPlotCurve* curve) const;
    //! ustawia pierwszy punkt podpiêty pod etykietê
    //! \param val ustawiany punkt
    void setPoint1(NewChartDotConstPtr val) { point1 = val; }
    //! \return drugi punkt podpiêty pod etykietê (mo¿e byæ nullptr)
    NewChartDotConstPtr getPoint2() const { return point2; }
    //! \return przetransformowana ju¿ na podstawie krzywej pozycja drugiego punktu
    QPoint getPoint2Transformed(const QwtPlotCurve* curve) const;
    //! ustawia drugi punkt podpiêty pod etykietê 
    //! \param val drugi punkt (mo¿e byæ nullptr)
    void setPoint2(NewChartDotConstPtr val) { point2 = val; }
    //! \return pêdzel, który jest u¿ywany do rysowania etykiety
    const QBrush& getBrush() const { return brush; }
    //! ustawia pêdzel
    //! \param val pêdzel, który bêdzie u¿ywany do rysowania etykiety
    void setBrush(const QBrush& val) { brush = val; }
    //! \return pióro, który jest u¿ywane do rysowania etykiety
    const QPen& getPen() const { return pen; }
    //! ustawia pióro
    //! \param val pióro, który bêdzie u¿ywane do rysowania etykiety
    void setPen(const QPen& val) { pen = val; }
    //! Emulacja rtti wykorzystywana przez Qwt
    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }
    //! ³¹czy punkt z etykiet¹
    //! \param dot pod³¹czany punkt
    //! \param style styl po³¹czenia
    void connectDot(NewChartDotConstPtr dot, ConnectionStyle style = Simple);
    //! ³¹czy dwa punkty z etykiet¹
    //! \param point1 pierwszy pod³¹czany punkt
    //! \param point2 drugi pod³¹czany punkt
    //! \param style styl po³¹czenia
    void connectDots(NewChartDotConstPtr point1, NewChartDotConstPtr point2, ConnectionStyle style );
    //! tworzy a nastepnie ³¹czy dwa punkty z etykiet¹
    //! \param point1 pierwszy pod³¹czany punkt
    //! \param point2 drugi pod³¹czany punkt
    //! \param style styl po³¹czenia
    void connectDots(const QPointF& point1, const QPointF& point2, ConnectionStyle style);

private:
    //! Rysowanie po³¹czenia
    //! \param painter obiekt paintera
    //! \param box obszar zajmowany przez etykietê
    //! \param transformedTo ju¿ przetransformowany punkt, który ma byæ przy³¹czony
    //! \param style styl ³¹czenia (prosty, horyzontalny, ...)
    //! \param arrowOutside czy strza³ki maj¹ byæ do wewn¹trz, czy na zewn¹trz
    void drawConnection(QPainter* painter, const QRect& box, const QPoint& transformedTo, ConnectionStyle style, bool arrowOutside = false) const;
    //! Rysowanie strza³ki
    //! \param painter obiekt paintera
    //! \param transformedFrom ju¿ przetransformowany punkt, z którego wychodzi strza³ka
    //! \param transformedTo ju¿ przetransformowany punkt, do którego prowadzi strza³ka
    //! \param outside czy strza³ki maj¹ byæ do wewn¹trz, czy na zewn¹trz
    void drawArrow(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, bool outside = false) const;

private:
    //! tekst etykiety
    QString text;
    //! rozmiar etykiety
    QPoint size;
    //! przesuniêcie miêdzy wskazywanym punktem a etykiet¹
    QPoint shift;
    //! pióro 
    QPen pen;
    //! pêdzel 
    QBrush brush;
    //! pierwszy podpiêty punkt
    NewChartDotConstPtr point1;
    //! drugi podpiêty punkt (mo¿e byæ nullptr)
    NewChartDotConstPtr point2;
    //! styl ³¹czenia etykiety z punktami
    ConnectionStyle connectionStyle;
};
typedef NewChartLabel* NewChartLabelPtr;
typedef const NewChartLabel* NewChartLabelConstPtr;


#endif
