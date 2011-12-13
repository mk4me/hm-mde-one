#include "NewChartPCH.h"
#include "StatsTable.h"

StatsTable::StatsTable( QWidget* parent /*= nullptr*/, Qt::WindowFlags f /*= 0*/ ) :
 QWidget(parent, f)
{
    setupUi(this);
    setExpand(false);
    connect(hideButton, SIGNAL(clicked()), this, SLOT(onButton()));
}

 void StatsTable::setExpand( bool expand )
 {
     expanded = expand;
     table->setHidden(!expand);
 }

 void StatsTable::onButton()
 {
     setExpand(!expanded);
 }

 void StatsTable::addEntry(const QString& name, ScalarChannelStatsConstPtr stats )
 {
     QTreeWidgetItem* item = new QTreeWidgetItem();
     item->setText(0, name);
     item->setText(1, QString("%1 ( %2s )").arg(stats->minValue()).arg(stats->minArgument()));
     item->setText(2, QString("%1 ( %2s )").arg(stats->maxValue()).arg(stats->maxArgument()));
     item->setText(3, QString("%1").arg(stats->meanValue()));
     item->setText(4, QString("%1").arg(stats->varianceValue()));
     table->addTopLevelItem(item);
 }

