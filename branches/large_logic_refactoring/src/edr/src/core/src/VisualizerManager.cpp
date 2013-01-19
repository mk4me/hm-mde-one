#include "CorePCH.h"
#include "VisualizerManager.h"
#include <plugins/newTimeline/ITimelineService.h>
#include "ServiceManager.h"
//#include "SceneGraphWidget.h"
#include <boost/foreach.hpp>
#include <OpenThreads/ScopedLock>
#include "DataHierarchyManager.h"

using namespace core;

VisualizerManager::VisualizerManager() //:
//debugWidget(nullptr)
{

}

VisualizerManager::~VisualizerManager()
{
    UTILS_ASSERT(visualizers.empty(), "Wszystkie wizualizatory powinny być zniszczone.");
    while (prototypes.size()) {
        prototypes.pop_back();
    }
    BOOST_FOREACH( IVisualizerPersistantData* data, visualizersData ) {
        delete data;
    }

    std::vector< IVisualizerPersistantData* >().swap(visualizersData);

    UTILS_ASSERT(visualizerChannels.empty(), "Wszystkie kanały czasowe wizualizatorów powinny być zniszczone.");

    //// zwalniamy wszystkie pozostałe kanały ponieważ nie mamy już żadnego wizualizatora
    //BOOST_FOREACH( IVisualizerChannel* channel, visualizerChannels ) {
    //    channel->releaseChannel();
    //}

    //visualizerChannels.swap(VisualizerChannels());
}

plugin::IVisualizerConstPtr VisualizerManager::getPrototype( UniqueID id ) const
{
    IVisualizers::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const plugin::IVisualizerPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return plugin::IVisualizerConstPtr(*found);
    } else {
        return plugin::IVisualizerConstPtr();
    }
}

VisualizerPtr VisualizerManager::createVisualizer( UniqueID id )
{
    plugin::IVisualizerConstPtr visualizer = getPrototype(id);
    if ( visualizer ) {
        return createVisualizer(visualizer);
    } else {
        throw std::runtime_error("Visualizer not registered.");
    }
}

int VisualizerManager::getNumInstances( UniqueID id )
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
    return std::count_if( visualizers.begin(), visualizers.end(), [=](Visualizer* ptr) {
		//TODO
        //return ptr->getID() == id;
		return false;
    });
}

VisualizerPtr VisualizerManager::createVisualizer( const plugin::IVisualizerConstPtr& prototype )
{
    VisualizerPtr result(new Visualizer(dynamic_cast<plugin::IVisualizer*>(prototype->create())));
    return result;
}

VisualizerPtr VisualizerManager::createVisualizer( const Visualizer& prototype )
{
    VisualizerPtr result(new Visualizer(prototype));
    return result;
}

VisualizerPtr VisualizerManager::createVisualizer( const TypeInfo& typeInfo )
{
	auto dataHierarchyManager = getDataHierarchyManager();
	for (auto it = mapType2ID.begin(); it != mapType2ID.end(); ++it) {
		if (dataHierarchyManager->isTypeCompatible(typeInfo, it->first)) {
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

void VisualizerManager::registerVisualizer( plugin::IVisualizerPtr visualizer )
{
    if (!getPrototype(visualizer->getID())) {

        std::vector<plugin::IInputDescription::InputInfo> visualizerInputInfo;

        visualizer->getInputInfo(visualizerInputInfo);

        if(visualizerInputInfo.empty() == true){
            CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " ID = " << visualizer->getID() << " does not support minimum one data type! NOT registered in application");
        }else{
			// wypełniamy mape, dzięki której mozemy łatwo stworzyć wizualizator na postawie typu
			for (auto it = visualizerInputInfo.begin(); it != visualizerInputInfo.end(); ++it) {
				// TODO : co w przypadku, gdy mamy wiele wizualizatorów obsługujących ten sam typ
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

            delete icon;
			//TODO
            //data->sourcesTypes = Visualizer::getSlotsInfoFromInputInfo(visualizerInputInfo);

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
        [=](const plugin::IVisualizerPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

void VisualizerManager::notifyCreated( Visualizer* visualizer )
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
	//TODO
    //CORE_LOG_DEBUG("Visualizer " << visualizer->getName() << " created");
	CORE_LOG_DEBUG("Visualizer " <<  " created");
    visualizers.push_back(visualizer);
}

void VisualizerManager::notifyDestroyed( Visualizer* visualizer )
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
	//TODO
	//CORE_LOG_DEBUG("Visualizer " << visualizer->getName() << " destroyed.");
    CORE_LOG_DEBUG("Visualizer " << " destroyed.");
    visualizers.remove(visualizer);
}

void VisualizerManager::notifyDestroyed(IVisualizerChannel* channel)
{
	auto channelIT = channels.find(channel);

	if(channelIT == channels.end()){
		return;
	}

	if(channelIT->second.synchRemove == true && channelIT->second.managed == false){

		auto seriesITEnd = channelIT->second.series.end();
		for(auto it = channelIT->second.series.begin(); it != seriesITEnd; ++it){
			try{
				channelIT->second.visualzier->removeSerie(*it);
			}catch(std::exception &){

			}catch(...){

			}
		}
	}

	//teraz próbuje usuwać serie danych z wizualizatora
	visualizerChannels[channelIT->second.visualzier].erase(channelIT->first);

	if(visualizerChannels[channelIT->second.visualzier].empty() == true){
		visualizerChannels.erase(channelIT->second.visualzier);
	}

	channels.erase(channel);
}

const void * VisualizerManager::createChannel(const plugin::VisualizerTimeSeriePtr & serie, Visualizer * visualizer, const std::string & path, bool synchRemove)
{
	TimelinePtr timeline = queryServices<ITimelineService>(ServiceManager::getInstance());
	if(timeline != nullptr) {

		VisualizerChannelPtr channel(new VisualizerChannel(serie));

		try{

			std::string p = (path.empty() == false) ? path : serie->getName();

			timeline->addChannel(p, channel);

			ChannelData data;
			data.visualzier = visualizer;
			data.series.insert(serie);
			data.path = p;
			data.managed = false;
			data.synchRemove = synchRemove;

			channels[channel.get()] = data;
			visualizerChannels[visualizer].insert(channel.get());
			return channel.get();
		}catch(std::runtime_error & e){
			CORE_LOG_WARNING("Could not add channel to timeline because: " << e.what());
		}catch(...){
			CORE_LOG_WARNING("Could not add channel to timeline. Unknown reason.");
		}
	}

	return nullptr;
}

const void * VisualizerManager::createChannel(const std::vector<plugin::VisualizerTimeSeriePtr> & series, Visualizer * visualizer, const std::string & path, bool synchRemove)
{
	TimelinePtr timeline = queryServices<ITimelineService>(ServiceManager::getInstance());
	if(timeline != nullptr) {

		VisualizerChannelPtr channel(new VisualizerMultiChannel(series));

		try{
			timeline->addChannel(path, channel);

			ChannelData data;
			data.visualzier = visualizer;
			data.series.insert(series.begin(), series.end());
			data.path = path;
			data.managed = false;
			data.synchRemove = synchRemove;

			channels[channel.get()] = data;
			visualizerChannels[visualizer].insert(channel.get());
			return channel.get();
		}catch(std::runtime_error & e){
			CORE_LOG_WARNING("Could not add channel to timeline because: " << e.what());
		}catch(...){
			CORE_LOG_WARNING("Could not add channel to timeline. Unknown reason.");
		}
	}

	return nullptr;
}

void VisualizerManager::removeChannel(const void * channel)
{
	//sprawdzam czy jest taki kanał
	auto channelIT = channels.find((IVisualizerChannel*)channel);
	if(channelIT == channels.end()){
		//jeśli nie ma to wychodzimy
		return;
	}

	//właściwa próba usunięcia kanału z wizualizatora i timeline
	TimelinePtr timeline = queryServices<ITimelineService>(ServiceManager::getInstance());
	bool removed = false;
	if(timeline != nullptr) {

		try{
			//oczekuje że destuktor kanału zrobi resztę wywołując notifyDestroyed
			timeline->removeChannel(channelIT->second.path);
			removed = true;
		}catch(std::runtime_error & e){
			CORE_LOG_WARNING("Could not remove channel from timeline because: " << e.what());
		}catch(...){
			CORE_LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
		}
	}

	if(removed == false){
		visualizerChannels[channelIT->second.visualzier].erase(channelIT->first);

		if(visualizerChannels[channelIT->second.visualzier].empty() == true){
			visualizerChannels.erase(channelIT->second.visualzier);
		}

		channels.erase(channelIT);
	}
}

void VisualizerManager::removeAllChannels()
{
	//właściwa próba usunięcia kanału z wizualizatora i timeline
	TimelinePtr timeline = queryServices<ITimelineService>(ServiceManager::getInstance());
	if(timeline != nullptr) {
		for(auto channelIT = channels.begin(); channelIT != channels.end(); ++channelIT){
			try{
				//oczekuje że destuktor kanału zrobi resztę wywołując notifyDestroyed
				timeline->removeChannel(channelIT->second.path);
			}catch(std::runtime_error & e){
				CORE_LOG_WARNING("Could not remove channel from timeline because: " << e.what());
			}catch(...){
				CORE_LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
			}
		}
	}

	std::map<IVisualizerChannel *, ChannelData>().swap(channels);
	std::map<Visualizer *, std::set<IVisualizerChannel *>>().swap(visualizerChannels);
}

void VisualizerManager::markAsRemovedFromVisualizer(const void * channel)
{
	//sprawdzam czy jest taki kanał
	auto channelIT = channels.find((IVisualizerChannel*)channel);
	if(channelIT == channels.end()){
		//jeśli nie ma to wychodzimy
		return;
	}

	channelIT->second.managed = true;
}

void VisualizerManager::markAllChannelsAsRemovedFromVisualizer(Visualizer * visualizer)
{
	auto visualizerIT = visualizerChannels.find(visualizer);
	if(visualizerIT == visualizerChannels.end()){
		return;
	}

	for(auto channelIT = visualizerIT->second.begin(); channelIT != visualizerIT->second.end(); ++channelIT){
		channels[*channelIT].managed = true;
	}
}

void VisualizerManager::removeAllChannelsFromVisualizer(Visualizer * visualizer)
{
	auto visualizerIT = visualizerChannels.find(visualizer);
	if(visualizerIT == visualizerChannels.end()){
		return;
	}

	for(auto channelIT = visualizerIT->second.begin(); channelIT != visualizerIT->second.end(); ++channelIT){
		removeChannel(*channelIT);
	}
}

void VisualizerManager::markAllChannelsAsRemoved()
{
	for(auto channelIT = channels.begin(); channelIT != channels.end(); ++channelIT){
		channelIT->second.managed = true;
	}
}