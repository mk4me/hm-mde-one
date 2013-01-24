#include "CorePCH.h"
#include "VisualizerManager.h"
#include "DataHierarchyManager.h"

using namespace core;

VisualizerManager::VisualizerManager()
{

}

VisualizerManager::~VisualizerManager()
{

}

void VisualizerManager::visualizerPrototypes(IVisualizerManager::VisualizerPrototypes & prototypes)
{
	for(auto it = visualizerPrototypes_.begin(); it != visualizerPrototypes_.end(); ++it){
		prototypes.push_back(it->second.visualizerPrototype.get());
	}
}

const plugin::IVisualizer * VisualizerManager::getVisualizerPrototype(UniqueID id)
{
	auto it = visualizerPrototypes_.find(id);
	if(it == visualizerPrototypes_.end()){
		return nullptr;
	}else{
		return it->second.visualizerPrototype.get();
	}
}

void VisualizerManager::getVisualizerPrototypesForType(core::TypeInfo & type, IVisualizerManager::VisualizerPrototypes & prototypes, bool exact)
{
	for(auto it = visualizerPrototypes_.begin(); it != visualizerPrototypes_.end(); ++it){
		if(it->second.basicSupportedTypes.find(type) != it->second.basicSupportedTypes.end()){
			prototypes.push_back(it->second.visualizerPrototype.get());
		}else if(exact == false && it->second.derrivedSupportedTypes.find(type) != it->second.derrivedSupportedTypes.end()){
			prototypes.push_back(it->second.visualizerPrototype.get());
		}		
	}
}

void VisualizerManager::registerForUpdate(plugin::IVisualizer * visualizer)
{
	ScopedLock lock(updateSync);
	updateVisualizers_.insert(visualizer);
}

void VisualizerManager::unregisterForUpdate(plugin::IVisualizer * visualizer)
{
	ScopedLock lock(updateSync);
	updateVisualizers_.erase(visualizer);
}

void VisualizerManager::registerObserver(IVisualizerManagerObserver * observer)
{
	ScopedLock lock(observerSync);
	observers_.insert(observer);
}

void VisualizerManager::unregisterObserver(IVisualizerManagerObserver * observer)
{
	ScopedLock lock(observerSync);
	observers_.erase(observer);
}

void VisualizerManager::notifyCreation(plugin::IVisualizer * visualizer)
{
	notify(visualizer, Creation);
}

void VisualizerManager::notifyDestruction(plugin::IVisualizer * visualizer)
{
	notify(visualizer, Destruction);
}

void VisualizerManager::notify(plugin::IVisualizer * visualizer, VisuzalizerOperation modyfication)
{
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

	if(modyfication == IVisualizerManager::Creation){
		visualizerInstances_.insert(visualizer);
	}else{
		visualizerInstances_.erase(visualizer);
	}
}

void VisualizerManager::registerVisualizerPrototype(plugin::IVisualizerPtr visualizerPrototype)
{
	if(visualizerPrototypes_.find(visualizerPrototype->getID()) != visualizerPrototypes_.end()){
		throw std::runtime_error("Visualizer with given ID already registered");
	}

	VisualizerPrototypeData protoData;
	protoData.visualizerPrototype = visualizerPrototype;

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
	for(auto it = updateVisualizers_.begin(); it != updateVisualizers_.end(); ++it){
		try{
			(*it)->update(deltaTime);
		}catch(std::exception & e){
			CORE_LOG_ERROR("Error while updating visualizer " << (*it)->getName() << " (ID: " << (*it)->getID() << "): " << e.what());
		}catch(...){
			CORE_LOG_ERROR("Unknown error while updating visualizer " << (*it)->getName() << " (ID: " << (*it)->getID() << ")");
		}
	}
}