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

//! Klasa reprezentuje etykiet� na wykresie
class NewChartLabel : public QwtPlotItem
{
public:
    //! styl po��cze� mi�dzy wskazywanymi punktami a etykiet�
    enum ConnectionStyle
    {
        Simple,       //!< proste strza�ki
        Horizontal,   //!< strza�ki �amane, najpierw poziome
        Vertical      //!< strza�ki �amane, najpierw pionowe
    };
public:
    //! Konstruktor
    //! \param text tekst etykiety
    //! \param shift domy�lne przesuni�cie (mi�dzy wsk. punktem i etykiet�)
    //! \param size rozmiar etykiety
    NewChartLabel(const QString& text = QString(""), const QPoint& shift = QPoint(20, 20), const QPoint& size = QPoint(80, 35));

public:
    //! Odrysowanie etykiety
    //! \param painter obiekt paintera
    //! \param xMap transformacja X
    //! \param yMap transformacja Y
    //! \param canvasRect obszar do rysowania
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;
    //! Sprawdza, czy punkt znajduje si� wewn�trz etykiety
    //! \param transformedPoint przetransformowany punkt
    //! \param curve krzywa, wykorzystuje si� jej transformacj�
    bool isInsideLabel(const QPoint& transformedPoint, const QwtPlotCurve* curve) const;
    //! \return tekst etykiety
    const QString& getText() const { return text; }
    //! ustawia tekst etykiety
    //! \param val nowy tekst
    void setText(const QString& val) { text = val; }
    //! \return przeusniecie mi�dzy wskazywanym punktem, a etykiet�
    const QPoint& getShift() const { return shift; }
    //! ustawia przeusniecie mi�dzy wskazywanym punktem, a etykiet�
    //! \param val nowe przesuni�cie
    void setShift(const QPoint& val) { shift = val; }
    //! \return pierwszy punkt podpi�ty pod etykiet�
    NewChartDotConstPtr getPoint1() const { return point1; }
    //! \return przetransformowana ju� na podstawie krzywej pozycja pierwszego punktu
    QPoint getPoint1Transformed(const QwtPlotCurve* curve) const;
    //! ustawia pierwszy punkt podpi�ty pod etykiet�
    //! \param val ustawiany punkt
    void setPoint1(NewChartDotConstPtr val) { point1 = val; }
    //! \return drugi punkt podpi�ty pod etykiet� (mo�e by� nullptr)
    NewChartDotConstPtr getPoint2() const { return point2; }
    //! \return przetransformowana ju� na podstawie krzywej pozycja drugiego punktu
    QPoint getPoint2Transformed(const QwtPlotCurve* curve) const;
    //! ustawia drugi punkt podpi�ty pod etykiet� 
    //! \param val drugi punkt (mo�e by� nullptr)
    void setPoint2(NewChartDotConstPtr val) { point2 = val; }
    //! \return p�dzel, kt�ry jest u�ywany do rysowania etykiety
    const QBrush& getBrush() const { return brush; }
    //! ustawia p�dzel
    //! \param val p�dzel, kt�ry b�dzie u�ywany do rysowania etykiety
    void setBrush(const QBrush& val) { brush = val; }
    //! \return pi�ro, kt�ry jest u�ywane do rysowania etykiety
    const QPen& getPen() const { return pen; }
    //! ustawia pi�ro
    //! \param val pi�ro, kt�ry b�dzie u�ywane do rysowania etykiety
    void setPen(const QPen& val) { pen = val; }
    //! Emulacja rtti wykorzystywana przez Qwt
    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }
    //! ��czy punkt z etykiet�
    //! \param dot pod��czany punkt
    //! \param style styl po��czenia
    void connectDot(NewChartDotConstPtr dot, ConnectionStyle style = Simple);
    //! ��czy dwa punkty z etykiet�
    //! \param point1 pierwszy pod��czany punkt
    //! \param point2 drugi pod��czany punkt
    //! \param style styl po��czenia
    void connectDots(NewChartDotConstPtr point1, NewChartDotConstPtr point2, ConnectionStyle style );
    //! tworzy a nastepnie ��czy dwa punkty z etykiet�
    //! \param point1 pierwszy pod��czany punkt
    //! \param point2 drugi pod��czany punkt
    //! \param style styl po��czenia
    void connectDots(const QPointF& point1, const QPointF& point2, ConnectionStyle style);

private:
    //! Rysowanie po��czenia
    //! \param painter obiekt paintera
    //! \param box obszar zajmowany przez etykiet�
    //! \param transformedTo ju� przetransformowany punkt, kt�ry ma by� przy��czony
    //! \param style styl ��czenia (prosty, horyzontalny, ...)
    //! \param arrowOutside czy strza�ki maj� by� do wewn�trz, czy na zewn�trz
    void drawConnection(QPainter* painter, const QRect& box, const QPoint& transformedTo, ConnectionStyle style, bool arrowOutside = false) const;
    //! Rysowanie strza�ki
    //! \param painter obiekt paintera
    //! \param transformedFrom ju� przetransformowany punkt, z kt�rego wychodzi strza�ka
    //! \param transformedTo ju� przetransformowany punkt, do kt�rego prowadzi strza�ka
    //! \param outside czy strza�ki maj� by� do wewn�trz, czy na zewn�trz
    void drawArrow(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, bool outside = false) const;

private:
    //! tekst etykiety
    QString text;
    //! rozmiar etykiety
    QPoint size;
    //! przesuni�cie mi�dzy wskazywanym punktem a etykiet�
    QPoint shift;
    //! pi�ro 
    QPen pen;
    //! p�dzel 
    QBrush brush;
    //! pierwszy podpi�ty punkt
    NewChartDotConstPtr point1;
    //! drugi podpi�ty punkt (mo�e by� nullptr)
    NewChartDotConstPtr point2;
    //! styl ��czenia etykiety z punktami
    ConnectionStyle connectionStyle;
};
typedef NewChartLabel* NewChartLabelPtr;
typedef const NewChartLabel* NewChartLabelConstPtr;


#endif
