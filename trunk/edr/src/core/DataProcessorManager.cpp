#include "CorePCH.h"
#include "DataProcessorManager.h"
#include <boost/foreach.hpp>

using namespace core;

DataProcessorManager::DataProcessorManager()
{

}

DataProcessorManager::~DataProcessorManager()
{
    UTILS_ASSERT(dataProcessors.empty(), "Wszystkie wizualizatory powinny byæ zniszczone.");
    while (prototypes.size()) {
        prototypes.pop_back();
    }
}

IDataProcessorConstPtr DataProcessorManager::getPrototype( UniqueID id ) const
{
    IDataProcessors::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(), 
        [=](const IDataProcessorConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IDataProcessorConstPtr(*found);
    } else {
        return IDataProcessorConstPtr();
    }
}

DataProcessorPtr DataProcessorManager::createDataProcessor( UniqueID id )
{
    IDataProcessorConstPtr dataProcessor = getPrototype(id);
    if ( dataProcessor ) {
        return createDataProcessor(dataProcessor);
    } else {
        throw std::runtime_error("DataProcessor not registered.");
    }
}

int DataProcessorManager::getNumInstances( UniqueID id )
{
    return std::count_if( dataProcessors.begin(), dataProcessors.end(), [=](DataProcessor* ptr) {
        return ptr->getID() == id;
    });
}

DataProcessorPtr DataProcessorManager::createDataProcessor( const IDataProcessorConstPtr& prototype )
{
    DataProcessorPtr result(new DataProcessor(dynamic_cast<IDataProcessor*>(prototype->createClone())));
    //visualizersTrace.push_back(result);
    return result;
}

DataProcessorPtr DataProcessorManager::createDataProcessor( const DataProcessor& prototype )
{
    DataProcessorPtr result(new DataProcessor(prototype));
    //visualizersTrace.push_back(result);
    return result;
}

void DataProcessorManager::update()
{
    /*Visualizers::iterator it = visualizers.begin();
    Visualizers::iterator last = visualizers.end();
    while ( it != last ) {
        (*it)->update(0);
        ++it;
    }*/
    //     WeakVisualizers::iterator it = visualizersTrace.begin();
    //     WeakVisualizers::iterator last = visualizersTrace.end();
    //     while ( it != last ) {
    //         VisualizerPtr strong = it->lock();
    //         if ( strong ) {
    //             strong->update(0);
    //             ++it;
    //         } else {
    //             LOG_DEBUG("Removing weak visualizer reference");
    //             WeakVisualizers::iterator toErase = it++;
    //             visualizersTrace.erase(toErase);
    //         }
    //     }
}

void DataProcessorManager::registerDataProcessor(const IDataProcessorPtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        //IVisualizerPersistantData* data = new IVisualizerPersistantData();

        //// zapisujemy dane niezmienne
        //// ikona
        //QIcon* icon = visualizer->createIcon();
        //if ( !icon ) {
        //    icon = new QIcon();
        //} 
        //data->icon = *icon;

        //// lista wejœæ
        ////SourcesTypes::value_type info;
        //IInputDescription::InputInfo info;
        //for (int i = 0; i < IVisualizer::maxNumSources; ++i) {
        //    info.name.clear();
        //    info.types.clear();
        //    visualizer->getInputInfo(i, info);
        //    if ( info.types.empty() ) {
        //        break;
        //    } else {

        //        ObjectSlots::SlotInfo slotInfo;
        //        slotInfo.name = info.name;
        //        slotInfo.required = info.required;
        //        slotInfo.modify = info.modify;
        //        slotInfo.types = info.types;
        //        // zamiast push_backa mo¿na zrobiæ bardziej optymalnie i nie kopiowaæ wektora...
        //        data->sourcesTypes.push_back(slotInfo);
        //        //data->sourcesTypes.insert(data->sourcesTypes.end(), SourcesTypes::value_type())->swap(info);
        //    }
        //}
        //visualizersData.push_back(data);
        notify();

    } else {
        throw std::runtime_error("DataProcessor with this ID already registered.");
    }
}

//const QIcon& DataProcessorManager::getIcon( UniqueID id ) const
//{
//    int idx = getPrototypeIdx(id);
//    UTILS_ASSERT(idx >= 0);
//    return visualizersData[idx]->icon;
//}
//
//const VisualizerManager::SourcesTypes& DataProcessorManager::getSourcesTypes( UniqueID id ) const
//{
//    int idx = getPrototypeIdx(id);
//    UTILS_ASSERT(idx >= 0);
//    return visualizersData[idx]->sourcesTypes;
//}

int DataProcessorManager::getPrototypeIdx( UniqueID id ) const
{
    IDataProcessors::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(), 
        [=](const IDataProcessorPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

void DataProcessorManager::notifyCreated( DataProcessor* dataProcessor )
{
    LOG_DEBUG("DataProcessor " << dataProcessor->getName() << " created");
    dataProcessors.push_back(dataProcessor);
    /*if ( debugWidget ) {
        debugWidget->addVisualizer(visualizer);
    }*/
}

void DataProcessorManager::notifyDestroyed( DataProcessor* dataProcessor )
{
    LOG_DEBUG("DataProcessor " << dataProcessor->getName() << " destroyed.");
    dataProcessors.remove(dataProcessor);
    /*if ( debugWidget ) {
        debugWidget->removeVisualizer(visualizer);
    }*/
}

