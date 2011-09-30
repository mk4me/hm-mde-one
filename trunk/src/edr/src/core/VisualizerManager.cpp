#include "CorePCH.h"
#include "VisualizerManager.h"
#include "DataManager.h"
#include "SceneGraphWidget.h"
#include <boost/foreach.hpp>

using namespace core;



VisualizerManager::VisualizerManager() :
debugWidget(nullptr)
{

}

VisualizerManager::~VisualizerManager()
{
    UTILS_ASSERT(visualizers.empty(), "Wszystkie wizualizatory powinny byæ zniszczone.");
    while (prototypes.size()) {
        prototypes.pop_back();
    }
    BOOST_FOREACH( IVisualizerPersistantData* data, visualizersData ) {
        delete data;
    }

    visualizersData.swap(std::vector< IVisualizerPersistantData* >());

    // zwalniamy wszystkie pozosta³e kana³y poniewa¿ nie mamy ju¿ ¿adnego wizualizatora
    BOOST_FOREACH( IVisualizerChannel* channel, visualizerChannels ) {
        channel->releaseChannel();
    }

    visualizerChannels.swap(VisualizerChannels());
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

int VisualizerManager::getNumInstances( UniqueID id )
{
    return std::count_if( visualizers.begin(), visualizers.end(), [=](Visualizer* ptr) {
        return ptr->getID() == id;
    });
}

VisualizerPtr VisualizerManager::createVisualizer( const IVisualizerConstPtr& prototype )
{
    VisualizerPtr result(new Visualizer(dynamic_cast<IVisualizer*>(prototype->createClone())));
    return result;
}

VisualizerPtr VisualizerManager::createVisualizer( const Visualizer& prototype )
{
    VisualizerPtr result(new Visualizer(prototype));
    return result;
}

VisualizerPtr VisualizerManager::createVisualizer( const core::TypeInfo& typeInfo )
{
	DataManager* dataManager = DataManager::getInstance();
	for (auto it = mapType2ID.begin(); it != mapType2ID.end(); it++) {
		if (dataManager->isTypeCompatible(typeInfo, it->first)) {
			return createVisualizer(it->second);
		}
	}
	throw std::runtime_error("Visualizer not registered.");
}

void VisualizerManager::update()
{
    Visualizers::iterator it = visualizers.begin();
    Visualizers::iterator last = visualizers.end();
    while ( it != last ) {
        (*it)->update(0);
        ++it;
    }
}

void VisualizerManager::registerVisualizer( IVisualizerPtr visualizer )
{
    if (!getPrototype(visualizer->getID())) {

        std::vector<IInputDescription::InputInfo> visualizerInputInfo;

        visualizer->getInputInfo(visualizerInputInfo);

        if(visualizerInputInfo.empty() == true){
            LOG_WARNING("Visualizer " << visualizer->getName() << " ID = " << visualizer->getID() << " does not support minimum one data type! NOT registered in application");
        }else{
			// wypelniamy mape, dzieki ktorej mozemy latwo stworzyc wizualizator na postawie typu
			for (auto it = visualizerInputInfo.begin(); it != visualizerInputInfo.end(); it++) {
				// TODO : co w przypadku, gdy mamy wiele wizualizatorow obslugujacych ten sam typ
				mapType2ID[it->type] = visualizer->getID();
			}

            prototypes.push_back(visualizer);
            IVisualizerPersistantData* data = new IVisualizerPersistantData();

            // zapisujemy dane niezmienne
            // ikona
            QIcon* icon = visualizer->createIcon();
            if ( !icon ) {
                icon = new QIcon();
            } 
            data->icon = *icon;

            data->sourcesTypes = Visualizer::getSlotsInfoFromInputInfo(visualizerInputInfo);

            visualizersData.push_back(data);
            notify();
        }

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

void VisualizerManager::notifyCreated( Visualizer* visualizer )
{
    LOG_DEBUG("Visualizer " << visualizer->getName() << " created");
    visualizers.push_back(visualizer);
    if ( debugWidget ) {
        debugWidget->addVisualizer(visualizer);
    }
}

void VisualizerManager::notifyDestroyed( Visualizer* visualizer )
{
    LOG_DEBUG("Visualizer " << visualizer->getName() << " destroyed.");
    visualizers.remove(visualizer);
    if ( debugWidget ) {
        debugWidget->removeVisualizer(visualizer);
    }
}

void VisualizerManager::notifyCreated(IVisualizerChannel* channel)
{
    visualizerChannels.push_back(channel);
}

void VisualizerManager::notifyDestroyed(IVisualizerChannel* channel)
{
    visualizerChannels.remove(channel);
}

