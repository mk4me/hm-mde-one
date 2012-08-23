/********************************************************************
    created:  2011/07/18
    created:  18:7:2011   14:05
    filename: ChannelWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__
#define HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__

#include <utils/Debug.h>
#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <timelinelib/Model.h>

//! Klasa reprezentuj¹ca pojedynczy kana³ w czasie - rysuje jego zakres na osi ze wzgledu na parametry begin i end,
//! które stanowia znormalizowane wartosci. Root zajmuje zawsze caly dostêpny kana³, jego d³ugoœæ jest punktem 
//! odniesienia dla pozosta³ych kana³ów. Zakres pocz¹tku i konca to [0;1] gdzie pocz¹tek musi byæ przed lub rowno z koncem
class ChannelWidget : //public QWidget 
    public QFrame 
{

    Q_OBJECT;

private:

    //! Znormalizowana po³owa szerokoœci dla aktualnego czasu
    double currentTimeHalfWidth;

    //! Kolor wype³nienia kana³u
    QColor fillColor;

    //! Kolor t³a
    QColor backgroundColor;

    //! Kolor markera czasu
    QColor markerColor;

    //! Left margin
    double leftMargin;

    //! Right margin
    double rightMargin;

    timeline::ControllerConstPtr controller;
    timeline::Model::TChannelConstPtr channel;

public:

    ChannelWidget(const timeline::ControllerConstPtr & controller, const timeline::Model::TChannelConstPtr & channel,
        QWidget* parent = 0, Qt::WindowFlags f = 0, double leftMargin = 0, double rightMargin = 0,
        double currentTimeHalfWidth = 0.03);


    //! \return Kolor wype³nienia kana³u
    const QColor & getFillColor() const;

    //! \return Kolor t³a
    const QColor & getBackgroundColor() const;

    //! \return Kolor znacznika czasu
    const QColor & getMarkerColor() const;

    //! \return Margines z lewej
    double getLeftMargin() const;

    //! \return Margines z prawej
    double getRightMargin() const;

public slots:

    //! \param leftMargin Lewy margines
    //! \param rightMargin Prawy margines
    void setMargins(double leftMargin, double rightMargin);

    //! \param margin Margines z lewej i prawej strony
    void setMargins(double margin);

    //! \param fillColor Kolor wype³niaj¹cy kana³
    void setFillColor(const QColor & fillColor);

    //! \param backgorundColor Kolor t³a
    void setBackgorundColor(const QColor & backgorundColor);

    //! \param backgorundColor Kolor t³a
    void setMarkerColor(const QColor & markerColor);

protected:
    //! Reimplementacja metody rysuj¹cej z QWidget
    virtual void paintEvent(QPaintEvent * pEvent);

    virtual void drawSingleTag(QPainter & painter, const QRectF & rect, double normBegin, double normLength);

};

#endif  //  HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__