#include "NewChartPCH.h"
#include "StatsTable.h"
#include <QtGui/QItemDelegate>



class ItemDelegate : public QItemDelegate
{
public:
    ItemDelegate(int width, int height) : width(width), height(height) {}
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        return QSize(width, height);
    }

    int getWidth() const { return width; }
    void setWidth(int val) { width = val; }
    int getHeight() const { return height; }
    void setHeight(int val) { height = val; }

private:
    int width;
    int height;
};

StatsTable::StatsTable( QWidget* parent /*= nullptr*/, Qt::WindowFlags f /*= 0*/ ) :
    QWidget(parent, f),
    rowHeight(12)
{
    setupUi(this);
    table->setItemDelegate(new ItemDelegate(100, rowHeight));
}

 int getTreeItemRows(const QTreeWidgetItem* item) 
 {
     int sum = 0;
     
     if (item->isExpanded()) {
         int count = item->childCount();
         for (int i = 0; i < count; i++){
             sum += getTreeItemRows(item->child(i));
         }
     }

     sum++;
     return sum;
 }

 //void StatsTable::setExpand( bool expand )
 //{
 //    expanded = expand;
 //    table->resizeColumnToContents(0);
 //    table->setHidden(!expand);
 //    if (expand) {

 //        int height = 0;
 //        for (int i = 0; i < table->topLevelItemCount(); i++) {
 //            height += getTreeItemRows(table->topLevelItem(i));
 //        }
 //        table->setMaximumHeight(height * rowHeight + table->header()->height());
 //    }
 //}

  
 QTreeWidgetItem* StatsTable::addEntry(const QString& group, const QString& name, ScalarChannelStatsConstPtr stats, const QColor& backgroundColor )
 {
     if (group.isEmpty()) {
         throw std::runtime_error("StatsTable: Passed empty group");
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

     for (int i = 1; i < item->columnCount(); i++) {
        item->setBackgroundColor(i, backgroundColor);
     }
    
     for (int i = 0; i < table->topLevelItemCount(); i++) {
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
         throw std::runtime_error("Unable to find statistic item");
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
     table->clear();
 }

 void StatsTable::recalculateHeight()
 {
     table->resizeColumnToContents(0);
     int height = 0;
     for (int i = 0; i < table->topLevelItemCount(); i++) {
         height += getTreeItemRows(table->topLevelItem(i));
     }
     table->setMaximumHeight(height * rowHeight + table->header()->height());
 }
