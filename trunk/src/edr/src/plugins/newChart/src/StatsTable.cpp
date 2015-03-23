#include "NewChartPCH.h"
#include "StatsTable.h"

StatsTable::StatsTable( QWidget* parent /*= nullptr*/, Qt::WindowFlags f /*= 0*/ ) :
    QWidget(parent, f),
    rowHeight(13),
    treeItemDelegate(100, rowHeight)
{
    setupUi(this);
}

 int getTreeItemRows(const QTreeWidgetItem* item) 
 {
     int sum = 0;
     
     if (item->isExpanded()) {
         int count = item->childCount();
         for (int i = 0; i < count; ++i){
             sum += getTreeItemRows(item->child(i));
         }
     }

     ++sum;
     return sum;
 }
   
 QTreeWidgetItem* StatsTable::addEntry(const QString& group, const QString& name, c3dlib::ScalarChannelStatsConstPtr stats, const QColor& backgroundColor)
 {
     if (group.isEmpty()) {
         throw core::runtime_error("StatsTable: Passed empty group");
     }

     // zapelnienie elementu statystykami
     QTreeWidgetItem* item = new QTreeWidgetItem();
     QString timeUnit = stats->getTimeUnit().c_str();
     QString valueUnit = stats->getValueUnit().c_str();
     int i = -1;
     item->setText(++i, name);
     item->setText(++i, QString("%1 - %2 (%3)").arg(stats->getDefinedFrom()).arg(stats->getDefinedTo()).arg(timeUnit));
     item->setText(++i, QString("%1 (%2)").arg(stats->minValue()).arg(valueUnit));
     item->setText(++i, QString("%1 (%2)").arg(stats->minArgument()).arg(timeUnit));
     item->setText(++i, QString("%1 (%2)").arg(stats->maxValue()).arg(valueUnit));
     item->setText(++i, QString("%1 (%2)").arg(stats->maxArgument()).arg(timeUnit));
     item->setText(++i, QString("%1 (%2)").arg(stats->meanValue()).arg(valueUnit));
     item->setText(++i, QString("%1 (%2)").arg(stats->varianceValue()).arg(valueUnit));

     for (int i = 1; i < item->columnCount(); ++i) {
        item->setBackgroundColor(i, backgroundColor);
     }
    
     stats2TreeItems.insert(std::make_pair(stats, item));
     for (int i = 0; i < table->topLevelItemCount(); ++i) {
         QTreeWidgetItem* groupItem = table->topLevelItem(i);
         if (groupItem->text(0) == group) {
             groupItem->addChild(item);
             return item;  // <<------------------------ return !!
         }
     }

     // nie znaleziono odpowiedniej grupy, tworzenie nowej
     QTreeWidgetItem* groupItem = new QTreeWidgetItem();
     groupItem->setText(0, group);
     groupItem->addChild(item);
     table->addTopLevelItem(groupItem);
     groupItem->setExpanded(true);
     return item;
 }

 void StatsTable::setActive( const QString& group, const QString& name )
 {
     QTreeWidgetItem* item = tryGetEntry(group, name);
     if (item) {
         setActive(item);
     } else {
         throw core::runtime_error("Unable to find statistic item");
     }
 }

 void StatsTable::setActive( QTreeWidgetItem* treeItem )
 {
     QTreeWidgetItem* groupItem = treeItem->parent();
     if (groupItem) {
         int icount = groupItem->childCount(); 
         for (int i = 0; i < icount; ++i) {
             QTreeWidgetItem* child = groupItem->child(i);
             child->setHidden(child == treeItem);
         }
     }
 }

 QTreeWidgetItem* StatsTable::tryGetEntry( const QString& group, const QString& name )
 {
     int icount = table->topLevelItemCount();
     for (int i = 0; i < icount; ++i) {
         QTreeWidgetItem* groupItem = table->topLevelItem(i);
         if (groupItem->text(0) == group) {
             int jcount = groupItem->childCount();
             for (int j = 0; j < jcount; ++j) {
                 QTreeWidgetItem* item = groupItem->child(j);
                 if (item->text(0) == name) {
                    return item;  // <<------------------------ return !!
                 }
             }
         }
     }

     return nullptr;
 }

 void StatsTable::clear()
 {
     stats2TreeItems.clear();
     table->clear();
 }

 void StatsTable::recalculateHeight()
 {
     table->resizeColumnToContents(0);
     int height = 0;
     for (int i = 0; i < table->topLevelItemCount(); ++i) {
         height += getTreeItemRows(table->topLevelItem(i));
     }
     table->setMaximumHeight(height * rowHeight + table->header()->height() + 8);
     label->height();
 }

QStringList StatsTable::getGroups() const
 {
     QStringList list;
     for(int i = 0; i < table->topLevelItemCount(); ++i) {
        list.push_back(table->topLevelItem(i)->text(0));
     }
     return list;
 }

StatsTable::range StatsTable::getEntries(c3dlib::ScalarChannelStatsConstPtr stats)
{
    auto r = stats2TreeItems.equal_range(stats);
    return boost::make_iterator_range(r.first, r.second);
}

std::list<QTreeWidgetItem*> StatsTable::getEntriesByChannel( channelConstPtr channel )
{
    std::list<QTreeWidgetItem*> list;
    for (auto it = stats2TreeItems.begin(); it != stats2TreeItems.end(); ++it) {
        if (it->first->getChannel() == channel) {
            list.push_back(it->second);
        }
    }

    return list;
}
