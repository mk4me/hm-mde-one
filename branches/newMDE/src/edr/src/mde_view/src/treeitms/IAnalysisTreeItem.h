/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   8:53
	filename: 	IAnalysisTreeItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__IANALYSISTREEITEM_H__
#define HEADER_GUARD_MDE_VIEW__IANALYSISTREEITEM_H__

#include <utils/SmartPtr.h>

class IAnalysisTreeItem;
DEFINE_SMART_POINTERS(IAnalysisTreeItem);

class IAnalysisTreeItem
{
public:
	virtual ~IAnalysisTreeItem() {}

public:
    virtual void appendChild(IAnalysisTreeItemPtr child) = 0;
    virtual void removeChild(IAnalysisTreeItemPtr child) = 0;
    virtual IAnalysisTreeItemConstPtr getChild(int index) const = 0;
    virtual int getNumChildren() const = 0;
    virtual int getIndex() const = 0;
    virtual int getChildIndex(IAnalysisTreeItemConstPtr child) const = 0;
    virtual IAnalysisTreeItemConstPtr getParent() const = 0;
    virtual QString getName() const = 0;
    virtual QIcon getIcon() const = 0;
    virtual void setParent(IAnalysisTreeItemPtr parent) = 0;
};


#endif
