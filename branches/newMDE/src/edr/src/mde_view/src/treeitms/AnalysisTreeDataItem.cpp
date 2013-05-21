#include "MdePCH.h"
#include "AnalysisTreeDataItem.h"
#include "TreeItemHelper.h"


AnalysisTreeDataItem::AnalysisTreeDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, TreeItemHelperPtr helper) :
    AnalysisTreeItem(name, icon),
    helper(helper)
{
}

AnalysisTreeDataItem::AnalysisTreeDataItem(utils::ObjectWrapperConstPtr wrapper,
        const QIcon& icon, const QString& name) :
    AnalysisTreeItem(name, icon),
    helper(new TreeWrappedItemHelper(wrapper))
{
}

AnalysisTreeDataItem::AnalysisTreeDataItem(utils::ObjectWrapperConstPtr wrapper, const QIcon& icon) :
    AnalysisTreeItem( QString::fromStdString(wrapper->getClassName()), icon),
    helper(new TreeWrappedItemHelper(wrapper))
{
}

AnalysisTreeDataItem::AnalysisTreeDataItem(const QIcon& icon, const QString& name, TreeItemHelperPtr helper) :
    AnalysisTreeItem( name, icon),
    helper(helper)
{

}

AnalysisTreeDataItem::~AnalysisTreeDataItem()
{

}
