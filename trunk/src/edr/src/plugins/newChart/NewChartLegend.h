/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:30
	filename: 	NewChartLegend.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLEGEND_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLEGEND_H__

#include <QtGui/QWidget>
#include <qwt_legend.h>
#include <qwt_legend_data.h>

class QCheckBox;
class QPushButton;

//! Klasa zarządzająca legendą New Charta
class NewChartLegend : public QwtLegend
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param parent  
    explicit NewChartLegend( QWidget *parent = nullptr ) : QwtLegend(parent) {}
    //! destruktor
    virtual ~NewChartLegend() {}

protected:
    //! Tworzy widget legendy, wywoływane przez qwt
    //! \param & standardowe qwt dane nt. legendy
    virtual QWidget *createWidget( const QwtLegendData & ) const;

signals:
    //! zmienił się checkbox z widocznością serii
    //! \param item element legendy zawierający checkbox
    //! \param active seria powinna stac się widoczna/ukryta
    void checkboxChanged(const QwtPlotItem* item, bool active);

public slots:
    //! Aktualizacja legendy
    virtual void updateLegend( const QwtPlotItem *, const QList<QwtLegendData> & );
    //! Aktualizacja pojedynczego elementu legendy
    //! \param w element klasy NewChartLegendItem
    //! \param data zmienione dane elementu
    virtual void updateWidget( QWidget* w, const QwtLegendData& data );

private slots:
    //! zmienił się checkbox z widocznością serii
    //! \param checked seria ma stac się widoczna/ukryta
    void onCheck(bool checked);

private:
    //! mapa z powiązaniem element wykresu (krzywa) - > widget legendy (NewChartLegendItem)
    std::map<const QWidget*, const QwtPlotItem *> widget2PlotItem;
};


#endif
