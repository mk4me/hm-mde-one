#include "CorePCH.h"
#include "VisualizerManager.h"
#include "DataManager.h"
#include <boost/foreach.hpp>

using namespace core;

VisualizerManager* VisualizerManager::instance = nullptr;

VisualizerManager::~VisualizerManager()
{
    BOOST_FOREACH( IVisualizerPersistantData* data, visualizersData ) {
        delete data;
    }
    visualizersData.clear();
}

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

VisualizerPtr VisualizerManager::createVisualizer( const Visualizer& prototype )
{
    VisualizerPtr result(new Visualizer(prototype));
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
        IVisualizerPersistantData* data = new IVisualizerPersistantData();

        // zapisujemy dane niezmienne
        // ikona
        QIcon* icon = visualizer->createIcon();
        if ( !icon ) {
            icon = new QIcon();
        } 
        data->icon = *icon;

        // lista wejœæ
        SourcesTypes::value_type info;
        for (int i = 0; i < IVisualizer::maxNumSources; ++i) {
            info.first.clear();
            info.second.clear();
            visualizer->getSlotInfo(i, info.first, info.second);
            if ( info.second.empty() ) {
                break;
            } else {
                // zamiast push_backa mo¿na zrobiæ bardziej optymalnie i nie kopiowaæ wektora...
                // data->sourcesTypes.push_back(info);
                data->sourcesTypes.insert(data->sourcesTypes.end(), SourcesTypes::value_type())->swap(info);
            }
        }
        visualizersData.push_back(data);

    } else {
        throw std::runtime_error("Visualizer with this ID already registered.");
    }
}

const QIcon& VisualizerManager::getIcon( UniqueID id ) const
{
    int idx = getPrototypeIdx(id);
    UTILS_ASSERT(idx >= 0);
    return visualizersData[idx]->icon;
}

const VisualizerManager::SourcesTypes& VisualizerManager::getSourcesTypes( UniqueID id ) const
{
    int idx = getPrototypeIdx(id);
    UTILS_ASSERT(idx >= 0);
    return visualizersData[idx]->sourcesTypes;
}

int VisualizerManager::getPrototypeIdx( UniqueID id ) const
{
    IVisualizers::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(), 
        [=](const IVisualizerPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

