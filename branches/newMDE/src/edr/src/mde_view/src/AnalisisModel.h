/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   18:38
	filename: 	AnalisisModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__
#define HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__

#include <coreui/HierarchyTreeModel.h>
#include <corelib/IDataManagerReader.h>
#include "ContextEventFilter.h"

class AnalisisModel : public core::IDataManagerReader::IObjectObserver
{
public:
    AnalisisModel(ContextEventFilterPtr contextEventFilter);
	virtual ~AnalisisModel() {}

public:
    virtual void observe(const core::IDataManagerReader::ChangeList & changes);
    coreui::HierarchyTreeModel* getTreeModel() { return &model; }
    ContextEventFilterPtr getContextEventFilter() const { return contextEventFilter; }


private:
    coreui::HierarchyTreeModel model;
    ContextEventFilterPtr contextEventFilter;
    core::IHierarchyItemPtr dataManagerTreeItem; 
};
DEFINE_SMART_POINTERS(AnalisisModel);

#endif
