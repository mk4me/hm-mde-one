#include "CorePCH.h"
#include <utils/Debug.h>
#include "DataProcessorManager.h"

using namespace core;

template<>
DataProcessorManager * ManagerHelper<DataProcessorManager>::manager = nullptr;

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

plugin::IDataProcessorConstPtr DataProcessorManager::getPrototype( UniqueID id ) const
{
    IDataProcessors::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const plugin::IDataProcessorConstPtr& ptr) {
			//TODO
			//return ptr->getID() == id;
			return false;
		}
    );
    if ( found != prototypes.end() ) {
        return plugin::IDataProcessorConstPtr(*found);
    } else {
        return plugin::IDataProcessorConstPtr();
    }
}

DataProcessorPtr DataProcessorManager::createDataProcessor( UniqueID id )
{
    plugin::IDataProcessorConstPtr dataProcessor = getPrototype(id);
    if ( dataProcessor ) {
        return createDataProcessor(dataProcessor);
    } else {
        throw std::runtime_error("DataProcessor not registered.");
    }
}

int DataProcessorManager::getNumInstances( UniqueID id )
{
    return std::count_if( dataProcessors.begin(), dataProcessors.end(), [=](DataProcessor* ptr) {
        //TODO
		//return ptr->getID() == id;
		return false;
    });
}

DataProcessorPtr DataProcessorManager::createDataProcessor( const plugin::IDataProcessorConstPtr& prototype )
{
	//TODO
    //DataProcessorPtr result(new DataProcessor(dynamic_cast<plugin::IDataProcessor*>(prototype->createClone())));
	DataProcessorPtr result;
    return result;
}

DataProcessorPtr DataProcessorManager::createDataProcessor( const DataProcessor& prototype )
{
    DataProcessorPtr result(new DataProcessor(prototype));
    return result;
}

void DataProcessorManager::registerDataProcessor(const plugin::IDataProcessorPtr & dataProcesor )
{
	//TODO
	/*if (!getPrototype(dataProcesor->getID())) {
	prototypes.push_back(dataProcesor);
	notify();

	} else {
	throw std::runtime_error("DataProcessor with this ID already registered.");
	}*/
}

int DataProcessorManager::getPrototypeIdx( UniqueID id ) const
{
    IDataProcessors::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const plugin::IDataProcessorPtr& ptr) {
			//TODO
			//return ptr->getID() == id;
			return false;
		}
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

void DataProcessorManager::notifyCreated( DataProcessor* dataProcessor )
{
	//TODO
    //CORE_LOG_DEBUG("DataProcessor " << dataProcessor->getName() << " created");
	CORE_LOG_DEBUG("DataProcessor " << " created");
    dataProcessors.push_back(dataProcessor);
}

void DataProcessorManager::notifyDestroyed( DataProcessor* dataProcessor )
{
	//TODO
    //CORE_LOG_DEBUG("DataProcessor " << dataProcessor->getName() << " destroyed.");
	CORE_LOG_DEBUG("DataProcessor " << " destroyed.");
    dataProcessors.remove(dataProcessor);
}

