#include "CorePCH.h"
#include "VisualizerManager.h"
#include "DataManager.h"

using namespace core;

VisualizerManager* VisualizerManager::instance = nullptr;

void VisualizerManager::createInstance()
{
    UTILS_ASSERT(!instance);
    instance = new VisualizerManager();
}

void VisualizerManager::destroyInstance()
{
    delete instance;
    instance = nullptr;
}

IVisualizerConstPtr VisualizerManager::getPrototype( UniqueID id ) const
{
    IVisualizers::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(), 
        [=](const IVisualizerPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IVisualizerConstPtr(*found);
    } else {
        return IVisualizerConstPtr();
    }
}

VisualizerPtr VisualizerManager::createVisualizer( UniqueID id )
{
    IVisualizerConstPtr visualizer = getPrototype(id);
    if ( visualizer ) {
        return createVisualizer(visualizer);
    } else {
        throw std::runtime_error("Visualizer not registered.");
    }
}

VisualizerPtr VisualizerManager::createVisualizer( const IVisualizerConstPtr& prototype )
{
    VisualizerPtr result(new Visualizer(prototype->create()));
    visualizersTrace.push_back(result);
    return result;
}

void VisualizerManager::update()
{
    WeakVisualizers::iterator it = visualizersTrace.begin();
    WeakVisualizers::iterator last = visualizersTrace.end();
    while ( it != last ) {
        VisualizerPtr strong = it->lock();
        if ( strong ) {
            strong->update(0);
            ++it;
        } else {
            LOG_DEBUG("Removing weak visualizer reference");
            WeakVisualizers::iterator toErase = it++;
            visualizersTrace.erase(toErase);
        }
    }
}

void VisualizerManager::registerVisualizer( IVisualizerPtr visualizer )
{
    if (!getPrototype(visualizer->getID())) {
        prototypes.push_back(visualizer);
    } else {
        throw std::runtime_error("Visualizer with this ID already registered.");
    }
    
}

