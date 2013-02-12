#include "CorePCH.h"
#include "VisualizerManager.h"
#include "DataHierarchyManager.h"
#include <utils/Push.h>

using namespace core;

VisualizerManager::VisualizerManager() : skipNotify(false)
{

}

VisualizerManager::~VisualizerManager()
{
	if(visualizerInstances_.empty() == false){
		CORE_LOG_DEBUG("Some visualizer instances still alive");
		for(auto it = visualizerInstances_.begin(); it != visualizerInstances_.end(); ++it){
			(*it).first->destroyAllSeries();
		}
	}

	skipNotify = true;
}

void VisualizerManager::visualizerPrototypes(IVisualizerManager::VisualizerPrototypes & prototypes)
{
	for(auto it = visualizerPrototypes_.begin(); it != visualizerPrototypes_.end(); ++it){
		prototypes.push_back(it->second.visualizerPrototype);
	}
}

VisualizerConstPtr VisualizerManager::getVisualizerPrototype(UniqueID id)
{
	auto it = visualizerPrototypes_.find(id);
	if(it == visualizerPrototypes_.end()){
		return VisualizerConstPtr();
	}else{
		return it->second.visualizerPrototype;
	}
}

void VisualizerManager::getVisualizerPrototypes(const core::TypeInfo & type, IVisualizerManager::VisualizerPrototypes & prototypes, bool exact)
{
	for(auto it = visualizerPrototypes_.begin(); it != visualizerPrototypes_.end(); ++it){
		if(it->second.basicSupportedTypes.find(type) != it->second.basicSupportedTypes.end()){
			prototypes.push_back(it->second.visualizerPrototype);
		}else if(exact == false && it->second.derrivedSupportedTypes.find(type) != it->second.derrivedSupportedTypes.end()){
			prototypes.push_back(it->second.visualizerPrototype);
		}		
	}
}

void VisualizerManager::registerVisualizer(Visualizer* visualizerImpl, plugin::IVisualizer * visualizer)
{
	if(skipNotify == true){
		return;
	}

	ScopedLock lockUpdate(updateSync);
	ScopedLock lockObserver(observerSync);
	visualizerInstances_[visualizerImpl] = visualizer;
	notify(visualizerImpl, IVisualizerManager::Creation);
}

void VisualizerManager::unregisterVisualizer(Visualizer* visualizer)
{
	if(skipNotify == true){
		return;
	}

	ScopedLock lockUpdate(updateSync);
	ScopedLock lockObserver(observerSync);
	visualizerInstances_.erase(visualizer);
	notify(visualizer, IVisualizerManager::Destruction);
}

void VisualizerManager::registerObserver(IVisualizerManagerObserver * observer)
{
	ScopedLock lock(observerSync);
	observers_.insert(observer);
}

void VisualizerManager::unregisterObserver(IVisualizerManagerObserver * observer)
{
	if(skipNotify == true){
		return;
	}

	ScopedLock lock(observerSync);
	observers_.erase(observer);
}

void VisualizerManager::notify(Visualizer * visualizer, VisuzalizerOperation modyfication)
{
	if(skipNotify == true){
		return;
	}

	ScopedLock lock(observerSync);

	for(auto it = observers_.begin(); it != observers_.end(); ++it){
		try{			
			(*it)->update(modyfication, visualizer);
		}catch(std::exception & e){
			CORE_LOG_ERROR("Error while updating visualizer observer: " << e.what());
		}catch(...){
			CORE_LOG_ERROR("Unknown error while updating visualizer observer");
		}
	}
}

void VisualizerManager::registerVisualizerPrototype(plugin::IVisualizerPtr visualizerPrototype)
{
	if(visualizerPrototypes_.find(visualizerPrototype->getID()) != visualizerPrototypes_.end()){
		throw std::runtime_error("Visualizer with given ID already registered");
	}

	VisualizerPrototypeData protoData;
	{
		utils::Push<bool> localSkipNotify(skipNotify, true);
		protoData.visualizerPrototype.reset(new Visualizer(visualizerPrototype, getMemoryDataManager(), this));
	}

	auto dhm = getDataHierarchyManager();

	TypeInfoList visTypes;
	visualizerPrototype->getSupportedTypes(visTypes);
	protoData.basicSupportedTypes.insert(visTypes.begin(), visTypes.end());
	for(auto it = visTypes.begin(); it != visTypes.end(); ++it){
		dhm->getTypeDerrivedTypes(*it, protoData.derrivedSupportedTypes);
	}

	visualizerPrototypes_.insert(VisualizerPrototypes::value_type(visualizerPrototype->getID(), protoData));
}

void VisualizerManager::update(double deltaTime)
{
	ScopedLock lock(updateSync);
	for(auto it = visualizerInstances_.begin(); it != visualizerInstances_.end(); ++it){
		try{
			it->second->update(deltaTime);
		}catch(std::exception & e){
			CORE_LOG_ERROR("Error while updating visualizer " << it->second->getName() << " (ID: " << it->second->getID() << "): " << e.what());
		}catch(...){
			CORE_LOG_ERROR("Unknown error while updating visualizer " << it->second->getName() << " (ID: " << it->second->getID() << ")");
		}
	}
}