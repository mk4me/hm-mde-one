/********************************************************************
	created:	2013/04/17
	created:	17:4:2013   10:13
	filename: 	AnalysisTreeModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALYSISTREEMODEL_H__
#define HEADER_GUARD_MDE_VIEW__ANALYSISTREEMODEL_H__

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
//#include "TreeItemHelper.h"
#include <corelib/HierarchyHelper.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/IHierarchyDataItem.h>




class AnalysisTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    AnalysisTreeModel(QObject* parent = nullptr);
    //AnalysisTreeModel(core::IHierarchyItemConstPtr root, QObject* parent = nullptr);
    virtual ~AnalysisTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void addRootItem(core::IHierarchyItemPtr root);
    void removeRootItem(core::IHierarchyItemPtr root);
    void updateItem(core::IHierarchyItemConstPtr item);


    core::IHierarchyItemConstPtr internalSmart(const QModelIndex& idx) const;

private:
    QModelIndex createSmartIndex(int row, int col, core::IHierarchyItemConstPtr ptr) const;
    bool hasChild(core::IHierarchyItemConstPtr parent, core::IHierarchyItemConstPtr child) const;

private:
    mutable std::map<const void*, core::IHierarchyItemConstPtr> raw2Smart;
    core::IHierarchyItemPtr rootItem;

};




#endif
