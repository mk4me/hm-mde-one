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

//! Klasa reprezentujaca pojedynczy kanal w czasie - rysuje jego zakres na osi ze wzgledu na parametry begin i end,
//! które stanowia znormalizowane wartosci. Root zajmuje zawsze caly dostepny kanal, jego dlugosc jest punktem 
//! odniesienia dla pozostalych kanalow. Zakres poczatku i konca to [0;1] gdzie poczatek musi byc przed lub rowno z koncem
class ChannelWidget : //public QWidget 
    public QFrame 
{

    Q_OBJECT;

private:

    //! Znormalizowany poczatek kanalu w czasie
    double begin;

    //! Znormalizowany koniec kanalu w czasie
    double end;

    //! Znormalizowany aktualny czas kanalu
    double currentTime;

    //! Znormalizowana polowa szerokosci dla aktualnego czasu
    double currentTimeHalfWidth;

    //! Kolor wype³nienia kana³u
    QColor fillColor;

    //! Kolor t³a
    QColor backgroundColor;

    //! Kolor markera czasu
    QColor markerColor;

public:

    ChannelWidget(QWidget* parent = 0, Qt::WindowFlags f = 0, double begin = 0, double end = 0, double currentTimeHalfWidth = 0.03, double currentTime = 0);

    //! Zwraca znormalizowany poczatek kanalu 
    double getBegin() const;

    //! \return Znormalizowany koniec kanalu
    double getEnd() const;

    //! \return Aktualny znormalizowany czas w kanale
    double getCurrentTime() const;

    //! \return Kolor wype³nienia kana³u
    const QColor & getFillColor() const;

    //! \return Kolor t³a
    const QColor & getBackgroundColor() const;

    //! \return Kolor znacznika czasu
    const QColor & getMarkerColor() const;

public slots:
    //! \param begin Znormalizowany poczatek kanalu - musi byc mniejszy lub rowny koncowi
    void setBegin(double begin);

    //! \param end Znormalizowany koniec kanalu - musi byc wiekszy lub rowny poczatkowi
    void setEnd(double end);

    //! \param current Znormalizowany aktualny czas w kanale
    void setCurrentTime(double current);

    //! \param begin begin Znormalizowany poczatek kanalu - musi byc mniejszy lub rowny koncowi
    //! \param end Znormalizowany koniec kanalu - musi byc wiekszy lub rowny poczatkowi
    //! \brief Obie wartoœci z przedzia³u [0;1], druga wiêksza lub równa pierwszej
    void setRange(double begin, double end);

    //! \param fillColor Kolor wype³niaj¹cy kana³
    void setFillColor(const QColor & fillColor);

    //! \param backgorundColor Kolor t³a
    void setBackgorundColor(const QColor & backgorundColor);

    //! \param backgorundColor Kolor t³a
    void setMarkerColor(const QColor & markerColor);

protected:
    //! Reimplementacja metody rysuj¹cej z QWidget
    virtual void paintEvent(QPaintEvent * pEvent);
};

#endif  //  HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__