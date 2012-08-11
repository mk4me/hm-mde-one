/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   13:23
	filename: 	NewChartLegendItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLEGENDITEM_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLEGENDITEM_H__

#include <QtGui/QWidget>
#include <qwt_legend.h>
#include <qwt_legend_data.h>

class QCheckBox;
class QPushButton;
class QwtPlotItem;

//! Element legendy wykresu
class NewChartLegendItem : public QWidget
{
    Q_OBJECT;
public:
    //! Konstruktor, wywolywany przez Qwt przy tworzeniu legendy
    //! \param data dane elementu (np. kolor, nazwa)
    //! \param parent parent elementu
    NewChartLegendItem(const QwtLegendData & data, QWidget* parent = nullptr);

public:
    //! Czy seria danych jest widoczna (stan check boxa)
    bool isItemVisible();
    //! Ustawia widocznosc serii (i check boxa)
    //! \param active krzywa widoczna / ukryta
    void setItemVisible(bool active);
    //! Ustawia check box zmieniajacy widocznosc serii
    //! \param enabled edytowalny / nieedytowalny
    void setItemVisibleEnabled(bool enabled);
    //! \return czy seria danych jest aktywna
    bool isItemActive() const;
    //! ustawia aktywnosc serii danych (i stan przycisku)
    //! \param checked seria aktywna / nieaktywna
    void setItemActive(bool checked);
    //! Ustawia przysik zmieniajacy aktywnosc serii
    //! \param enabled edytowalny / nieedytowalny
    void setItemActiveEnabled(bool enabled);
    //! Zmienia dane legendy (kolor, tytul itp.) wywolywane przez qwt
    //! \param data dane elementu
    void setData( const QwtLegendData& data );
    //! Ustawia opis elementu
    //! \param val opis
    void setTooltip( const QString& val );

signals:
    //! kliknieto przycisk, po wcisnieciu ktorego seria staje sie aktywna
    void buttonClicked(bool);
    //! zmieniono check box zarzadza widocznoscia serii danych
    //! \param widoczny / ukryty 
    void checkClicked(bool);

private slots:
    //! Przechwytuje sygnal i przesyla go dalej
    void onClick(bool);

private:
    //! check box zarzadza widocznoscia serii danych
    QCheckBox* check;
    //! przycisk, po wcisnieciu ktorego seria staje sie aktywna, tylko jeden moze byc wcisniety
    QPushButton* button;
};


#endif
