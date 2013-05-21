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
//#include "FilterCommand.h"
//#include "Vector3DFilterCommand.h"



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
        QModelIndexList list = tree->selectionModel()->selectedIndexes();
        if (!list.empty()) {
            QModelIndex idx = list.front();
            auto item = model.internalSmart(idx);
            auto dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
            if (dataItem && dataItem->getData()) {
                outPinA->value(dataItem->getData()->get());
            }
        }
        /*HmmTreeItem* treeItem = dynamic_cast<HmmTreeItem*>(tree->currentItem());
        if (treeItem) {
            NewVector3ItemHelperPtr vectorItem = utils::dynamic_pointer_cast<NewVector3ItemHelper>(treeItem->getHelper());
            auto wrp = vectorItem->getWrapper();
            channel = wrp->get();
            outPinA->value(channel);
            used = true;
        } else {
            throw std::runtime_error("Source is not set");
        }*/
    }

    virtual QWidget* getConfigurationWidget() { return tree; }
    virtual bool isNodeValid()
    {
        return true;
    }
    virtual QString getErrorMessage()
    {
        return QString("Source is not set");
    }

private:

    virtual void refresh( core::IHierarchyItemConstPtr root ) 
    {
        model.clear();
        model.addRootItem(root);
    }

private:
    UniversalOutputPin<PinT> * outPinA;
    QTreeView* tree;
    typename UniversalOutputPin<PinT>::ConstPtr channel;
    bool used;
    //BuilderFilterCommand::BranchFunction branchFunction;
    QIcon rootIcon;
    QIcon leafIcon;
    coreui::HierarchyTreeModel model;
};


#endif
