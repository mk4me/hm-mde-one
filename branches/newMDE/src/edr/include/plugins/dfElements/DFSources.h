/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   11:57
	filename: 	DFSources.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS__DFSOURCES_H__
#define HEADER_GUARD_DFELEMENTS__DFSOURCES_H__

#include <plugins/dfElements/DFPins.h>
#include <QtGui/QDoubleSpinBox>
#include <plugins/dfElements/DFPins.h>
#include <utils/ObjectWrapper.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QIcon>
#include <coreui/HierarchyTreeModel.h>
#include <plugins/subject/SubjectDataFilters.h>
#include <plugins/newVdf/TreeBuilder.h>


template <class PinT>
class UniversalSource : public df::SourceNode, public df::IDFSource, public vdf::INodeConfiguration, public vdf::INodeValidation, public vdf::INodeHierarchyObserver
{
public:
    UniversalSource ()
      {
          tree = new QTreeView();
          tree->setModel(&model);
          tree->setHeaderHidden(true);
          outPinA = new UniversalOutputPin<PinT>(this);
          addOutputPin(outPinA);
          used = false;
      }

    virtual ~UniversalSource() { delete tree; }
public:
    virtual void reset()
    {
        used = false;
    }
    virtual const bool empty() const
    {
        return used || !channel;
    }
    virtual void produce()
    {
        auto item = tryGetSelectedItem();
        auto dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
        if (dataItem && dataItem->getData()) {
            outPinA->value(dataItem->getData()->get());
        }
    }

    
    virtual QWidget* getConfigurationWidget() { return tree; }
    virtual bool isNodeValid()
    {
        return tryGetSelectedItem();
    }
    virtual QString getErrorMessage()
    {
        return isNodeValid() ? QString() : QString("Source is not set");
    }
    
    virtual void refresh( core::IDataManagerReader* dm, const core::IDataManagerReader::ChangeList & changes )
    {
        core::ConstObjectsList sessions, inputSessions;
        dm->getObjects(inputSessions, typeid(PluginSubject::ISession), false);
        SubjectHierarchyFilterPtr typeFilter(new SubjectHierarchyTypeFilter(typeid(VectorChannelCollection)));
        typeFilter->filterSessions(inputSessions, sessions);
        //
        auto item = TreeBuilder::createTree("Sessions", sessions, typeFilter);
        refreshTree(item);
    }

private:
    void refreshTree( core::IHierarchyItemConstPtr root ) 
    {
        model.clear();
        model.addRootItem(root);
    }

    core::IHierarchyItemConstPtr tryGetSelectedItem()
    {
        QModelIndexList list = tree->selectionModel()->selectedIndexes();
        if (!list.empty()) {
            QModelIndex idx = list.front();
            return model.internalSmart(idx);
        }
        return core::IHierarchyItemConstPtr();
    }


private:
    UniversalOutputPin<PinT> * outPinA;
    QTreeView* tree;
    typename UniversalOutputPin<PinT>::ConstPtr channel;
    bool used;
    QIcon rootIcon;
    QIcon leafIcon;
    coreui::HierarchyTreeModel model;
};


#endif
