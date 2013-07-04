#include "MdePCH.h"
#include "AnalisisModel.h"
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <plugins/newVdf/TreeBuilder.h>
#include <QtGui/QDockWidget>
#include <plugins/newTimeline/ITimelineService.h>
#include <corelib/IServiceManager.h>

AnalisisModel::AnalisisModel()
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



void AnalisisModel::update(core::Visualizer::VisualizerSerie * serie, core::Visualizer::SerieModyfication modyfication )
{
    if(modyfication == core::Visualizer::REMOVE_SERIE){
        auto it = seriesToChannels.find(serie);
        if(it != seriesToChannels.end()){
            std::string path = it->second;
            auto timeline = core::queryServices<ITimelineService>(plugin::getServiceManager());
            if(timeline != nullptr){
                timeline->removeChannel(it->second);
            }

            seriesToChannels.erase(it);

            //TODO
            //usun¹æ wpisy dla pozosta³ych serii bo kanal usuwamy tylko raz

            auto tIT = seriesToChannels.begin();
            while(tIT != seriesToChannels.end()){
                if(tIT->second == path){
                    auto toErase = tIT;
                    ++tIT;
                    seriesToChannels.erase(toErase);
                }else{
                    ++tIT;
                }
            }

            //teraz usuwam wpisy dla menu
            for(auto it = items2Descriptions.begin(); it != items2Descriptions.end(); ++it)
            {
                if(it->second->path == path){
                    items2Descriptions.erase(it);
                    break;
                }
            }
        }
    }
}



void AnalisisModel::addSeriesToVisualizer( core::VisualizerPtr visualizer, core::HierarchyHelperPtr helper, QString &path, QDockWidget * visualizerDockWidget )
{
    std::vector<core::Visualizer::VisualizerSerie*> series;
    helper->getSeries(visualizer, path, series);
    if (!series.empty()) {

        DataItemDescriptionPtr desc = utils::make_shared<AnalisisModel::DataItemDescription>(qobject_cast<coreUI::CoreVisualizerWidget*>(visualizerDockWidget->widget()), visualizerDockWidget);	 
        desc->channel = utils::shared_ptr<VisualizerSerieTimelineMultiChannel>(new VisualizerSerieTimelineMultiChannel(VisualizerSerieTimelineMultiChannel::VisualizersSeries(series.begin(), series.end())));
        desc->path = path.toStdString();
        addVisualizerDataDescription(helper, desc);

        auto timeline = core::queryServices<ITimelineService>(plugin::getServiceManager());
        //timeline->addChannel(desc.path, desc.channel);
        auto channels = utils::shared_ptr<VisualizerSerieTimelineMultiChannel>(new VisualizerSerieTimelineMultiChannel(VisualizerSerieTimelineMultiChannel::VisualizersSeries(series.begin(), series.end())));
        timeline->addChannel(path.toStdString(), channels);

        for(auto it = series.begin(); it != series.end(); ++it){
            seriesToChannels[*it] = desc->path;
        }
    } else {
        PLUGIN_LOG_WARNING("Problem with adding series to visualizer");
    }
}