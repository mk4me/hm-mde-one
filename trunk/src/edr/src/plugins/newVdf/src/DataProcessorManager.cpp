#include "NewVdfPCH.h"
#include <utils/Debug.h>
#include "DataProcessorManager.h"

using namespace vdf;
//using namespace core;

DataProcessorManager::DataProcessorManager()
{

}

DataProcessorManager::~DataProcessorManager()
{
    UTILS_ASSERT(dataProcessors.empty(), "Wszystkie elementy przetwarzające powinny być zniszczone.");
    while (prototypes.size()) {
        prototypes.pop_back();
    }
}

IDataProcessorConstPtr DataProcessorManager::getPrototype( core::UniqueID id ) const
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

//DataProcessorPtr DataProcessorManager::createDataProcessor( UniqueID id )
//{
//    IDataProcessorConstPtr dataProcessor = getPrototype(id);
//    if ( dataProcessor ) {
//        return createDataProcessor(dataProcessor);
//    } else {
//        throw std::runtime_error("DataProcessor not registered.");
//    }
//}

//int DataProcessorManager::getNumInstances( UniqueID id )
//{
//    return std::count_if( dataProcessors.begin(), dataProcessors.end(), [=](DataProcessor* ptr) {
//        return ptr->getID() == id;
//    });
//}

//DataProcessorPtr DataProcessorManager::createDataProcessor( const IDataProcessorConstPtr& prototype )
//{
//    DataProcessorPtr result(new DataProcessor(dynamic_cast<IDataProcessor*>(prototype->createClone())));
//    return result;
//}
//
//DataProcessorPtr DataProcessorManager::createDataProcessor( const DataProcessor& prototype )
//{
//    DataProcessorPtr result(new DataProcessor(prototype));
//    return result;
//}

void DataProcessorManager::registerDataProcessor(const IDataProcessorPtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        notify();

    } else {
        throw std::runtime_error("DataProcessor with this ID already registered.");
    }
}

int DataProcessorManager::getPrototypeIdx( core::UniqueID id ) const
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

//void DataProcessorManager::notifyCreated( DataProcessor* dataProcessor )
//{
//    CORE_LOG_DEBUG("DataProcessor " << dataProcessor->getName() << " created");
//    dataProcessors.push_back(dataProcessor);
//}
//
//void DataProcessorManager::notifyDestroyed( DataProcessor* dataProcessor )
//{
//    CORE_LOG_DEBUG("DataProcessor " << dataProcessor->getName() << " destroyed.");
//    dataProcessors.remove(dataProcessor);
//}
