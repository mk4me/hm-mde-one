#include "MdePCH.h"
#include "AnalisisModel.h"
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <plugins/newVdf/TreeBuilder.h>
#include <QtGui/QDockWidget>

AnalisisModel::AnalisisModel(ContextEventFilterPtr contextEventFilter) :
    contextEventFilter(contextEventFilter)
{

}

void AnalisisModel::observe( const core::IDataManagerReader::ChangeList & changes )
{
    core::ConstObjectsList sessions;
    plugin::getDataManagerReader()->getObjects(sessions, typeid(PluginSubject::ISession), false);

    if (dataManagerTreeItem) {
        model.removeRootItem(dataManagerTreeItem);
    }
    dataManagerTreeItem = TreeBuilder::createTree("Sessions", sessions);
    model.addRootItem(dataManagerTreeItem);
    /*break;
    auto dhm = plugin::getDataHierachyManagerReader();
    for(auto it = changes.begin(); it != changes.end(); ++it){
        if((*it).type == typeid(PluginSubject::ISession) || dhm->isTypeCompatible(typeid(PluginSubject::ISession), (*it).type)){
            core::ConstObjectsList sessions;
            plugin::getDataManagerReader()->getObjects(sessions, typeid(PluginSubject::ISession), false);
            model.setRootItem(TreeBuilder::createTree("Sessions", sessions));
            break;
        }
    }*/
}

void AnalisisModel::addFilterBundles( const core::IFilterProvider::FilterBundles& bundles )
{
    for (auto it = bundles.begin(); it != bundles.end(); ++it) {
        filters.push_back(*it);
        emit filterBundleAdded(*it);
    }
}

void AnalisisModel::applyFilter( core::IFilterCommandPtr filter )
{
    if (filter) {
        if (itms.empty()) {
            saveSourceItems();
        }
        model.clear();
        int count = static_cast<int>(itms.size());
        for (int i = 0; i < count; ++i) {
            model.addRootItem(filter->getFilteredTree(itms[i]));
        }
    } else {
        loadSourceItems();
    }
}

void AnalisisModel::saveSourceItems()
{
    itms.clear();
    auto count = model.getNumChildren();
    for (int i = 0; i < count; ++i) {
        itms.push_back(model.getChild(i));
    }
}

void AnalisisModel::loadSourceItems()
{
    model.clear();
    for (auto it = itms.begin(); it != itms.end(); ++it) {
        model.addRootItem(*it);
    }
    itms.clear();
}

void AnalisisModel::addVisualizerDataDescription( core::HierarchyHelperPtr helper, AnalisisModel::DataItemDescriptionPtr desc )
{
    items2Descriptions.insert(std::make_pair(core::HierarchyHelperWeakPtr(helper), desc));
}

std::list<AnalisisModel::DataItemDescriptionConstPtr> AnalisisModel::getVisualizerDataDescriptions( const core::HierarchyHelperPtr& helper )
{
    std::list<DataItemDescriptionConstPtr> descs;
    auto range = items2Descriptions.equal_range(core::HierarchyHelperWeakPtr(helper));
    for (auto it = range.first; it != range.second; it++) {
        descs.push_back(it->second);
    }

    return descs;
}

AnalisisModel::DataItemDescriptionConstPtr AnalisisModel::getVisualizerDataDescription( const core::VisualizerPtr& visualizer )
{
    for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); ++it) {
        DataItemDescriptionPtr d = it->second;
        if (d->visualizerWidget->getVisualizer() == visualizer) {
            return d;
        }
    }
    return DataItemDescriptionConstPtr();
}

core::HierarchyHelperPtr AnalisisModel::getHelper( const DataItemDescriptionConstPtr& desc ) 
{
    for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); ++it) {
        if (it->second == desc) {
            return core::HierarchyHelperPtr(it->first);
        }
    }


    return core::HierarchyHelperPtr();
}


AnalisisModel::DataItemDescription::DataItemDescription( coreUI::CoreVisualizerWidget* widget, QDockWidget * dockWidget) :
    visualizerWidget(widget),
    visualizerDockWidget(dockWidget)
{
    //// konwersja na weak ptr.
    //for (auto it = series.begin(); it != series.end(); ++it) {
    //    this->series.push_back(*it);
    //}
}