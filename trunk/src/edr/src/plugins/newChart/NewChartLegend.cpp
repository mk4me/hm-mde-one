#include "NewChartPCH.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include "NewChartLegend.h"
#include "NewChartLegendItem.h"

QWidget * NewChartLegend::createWidget( const QwtLegendData & data ) const
{
    NewChartLegendItem* item = new NewChartLegendItem(data);
    connect( item, SIGNAL( buttonClicked( bool ) ), this,  SLOT( itemChecked( bool ) ) );
    connect( item, SIGNAL( checkClicked(bool)), this, SLOT(onCheck(bool)));
    return item;
}

void NewChartLegend::onCheck(bool checked)
{
    auto it = widget2PlotItem.find(qobject_cast<QWidget*>(sender()));
    if (it != widget2PlotItem.end()) {
        const QwtPlotItem* plotItem = it->second;
        emit checkboxChanged(plotItem, checked);
    } else {
        UTILS_ASSERT(false);
    }
}

void NewChartLegend::updateLegend( const QwtPlotItem *item, const QList<QwtLegendData> &list )
{
    QwtLegend::updateLegend(item, list);
    widget2PlotItem[this->legendWidget(item)] = item;
}

void NewChartLegend::updateWidget( QWidget* w, const QwtLegendData& data )
{
    NewChartLegendItem* item = qobject_cast<NewChartLegendItem*>(w);
    UTILS_ASSERT(item);
    if (item) {
        item->setData(data);
    }
}

