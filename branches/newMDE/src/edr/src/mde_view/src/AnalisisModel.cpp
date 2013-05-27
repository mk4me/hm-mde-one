#include "MdePCH.h"
#include "AnalisisModel.h"
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <plugins/newVdf/TreeBuilder.h>


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
    std::vector<core::IHierarchyItemConstPtr> itms;
    auto count = model.getNumChildren();
    for (int i = 0; i < count; ++i) {
        itms.push_back(model.getChild(i));
    }
    model.clear();
    for (int i = 0; i < count; ++i) {
        model.addRootItem(filter->getFilteredTree(itms[i]));
    }
}
