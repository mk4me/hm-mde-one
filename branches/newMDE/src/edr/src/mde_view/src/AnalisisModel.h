/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   18:38
	filename: 	AnalisisModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__
#define HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__

#include <QtCore/QObject>
#include <coreui/HierarchyTreeModel.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IFilterProvider.h>
#include <corelib/IFilterBundle.h>
#include "ContextEventFilter.h"

class AnalisisModel : public QObject, public core::IDataManagerReader::IObjectObserver
{
    Q_OBJECT
public:
    AnalisisModel(ContextEventFilterPtr contextEventFilter);
	virtual ~AnalisisModel() {}

public:
    virtual void observe(const core::IDataManagerReader::ChangeList & changes);
    coreui::HierarchyTreeModel* getTreeModel() { return &model; }
    ContextEventFilterPtr getContextEventFilter() const { return contextEventFilter; }
    void addFilterBundles( const core::IFilterProvider::FilterBundles& bundles );
    void applyFilter( core::IFilterCommandPtr filter );

Q_SIGNALS:
    void filterBundleAdded(core::IFilterBundlePtr);
    void reportCreated(const QString& html);
private:
    coreui::HierarchyTreeModel model;
    ContextEventFilterPtr contextEventFilter;
    core::IHierarchyItemPtr dataManagerTreeItem; 
    core::IFilterProvider::FilterBundles filters;
};
DEFINE_SMART_POINTERS(AnalisisModel);

#endif
