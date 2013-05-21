/********************************************************************
	created:	2013/04/17
	created:	17:4:2013   10:19
	filename: 	AnalysisTreeDataItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALYSISTREEDATAITEM_H__
#define HEADER_GUARD_MDE_VIEW__ANALYSISTREEDATAITEM_H__

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <utils/ObjectWrapper.h>
#include "TreeItemHelper.h"
#include "AnalysisTreeItem.h"
#include "IAnalysisDataItem.h"

class AnalysisTreeDataItem : public AnalysisTreeItem, public IAnalysisDataItem
{
public:
    AnalysisTreeDataItem(utils::ObjectWrapperConstPtr wrapper,
        const QIcon& icon, const QString& name, TreeItemHelperPtr helper);

    AnalysisTreeDataItem(utils::ObjectWrapperConstPtr wrapper, 
        const QIcon& icon, const QString& name);

    AnalysisTreeDataItem(utils::ObjectWrapperConstPtr wrapper, 
        const QIcon& icon = QIcon());

    AnalysisTreeDataItem(const QIcon& icon, const QString& name, 
        TreeItemHelperPtr helper);

	virtual ~AnalysisTreeDataItem();

public:
    virtual TreeItemHelperPtr getHelper() const { return helper; }

private:
    TreeItemHelperPtr helper;
};

#endif
