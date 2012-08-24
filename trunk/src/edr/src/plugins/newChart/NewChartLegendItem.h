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
    //! Konstruktor, wywo³ywany przez Qwt przy tworzeniu legendy
    //! \param data dane elementu (np. kolor, nazwa)
    //! \param parent parent elementu
    NewChartLegendItem(const QwtLegendData & data, QWidget* parent = nullptr);

public:
    //! Czy seria danych jest widoczna (stan check boxa)
    bool isItemVisible();
    //! Ustawia widocznoœæ serii (i check boxa)
    //! \param active krzywa widoczna / ukryta
    void setItemVisible(bool active);
    //! Ustawia check box zmieniaj¹cy widocznoœæ serii
    //! \param enabled edytowalny / nieedytowalny
    void setItemVisibleEnabled(bool enabled);
    //! \return czy seria danych jest aktywna
    bool isItemActive() const;
    //! ustawia aktywnoœæ serii danych (i stan przycisku)
    //! \param checked seria aktywna / nieaktywna
    void setItemActive(bool checked);
    //! Ustawia przysik zmieniaj¹cy aktywnoœæ serii
    //! \param enabled edytowalny / nieedytowalny
    void setItemActiveEnabled(bool enabled);
    //! Zmienia dane legendy (kolor, tytu³ itp.) wywo³ywane przez qwt
    //! \param data dane elementu
    void setData( const QwtLegendData& data );
    //! Ustawia opis elementu
    //! \param val opis
    void setTooltip( const QString& val );

signals:
    //! klikniêto przycisk, po wcisnieciu którego seria staje siê aktywna
    void buttonClicked(bool);
    //! zmieniono check box zarz¹dza widocznoœci¹ serii danych
    //! \param widoczny / ukryty 
    void checkClicked(bool);

private slots:
    //! Przechwytuje sygna³ i przesy³a go dalej
    void onClick(bool);

private:
    //! check box zarz¹dza widocznoœci¹ serii danych
    QCheckBox* check;
    //! przycisk, po wcisnieciu którego seria staje siê aktywna, tylko jeden mo¿e byæ wcisniety
    QPushButton* button;
};


#endif
