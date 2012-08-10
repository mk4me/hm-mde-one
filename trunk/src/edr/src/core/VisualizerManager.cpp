#include "CorePCH.h"
#include "VisualizerManager.h"
#include <plugins/newTimeline/ITimelineService.h>
#include "ServiceManager.h"
#include "DataManager.h"
//#include "SceneGraphWidget.h"
#include <boost/foreach.hpp>
#include <OpenThreads/ScopedLock>

using namespace core;


template<>
VisualizerManager * ManagerHelper<VisualizerManager>::manager = nullptr;

VisualizerManager::VisualizerManager() //:
//debugWidget(nullptr)
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

    std::vector< IVisualizerPersistantData* >().swap(visualizersData);

    UTILS_ASSERT(visualizerChannels.empty(), "Wszystkie kana³y czasowe wizualizatorów powinny byæ zniszczone.");

    //// zwalniamy wszystkie pozosta³e kana³y poniewa¿ nie mamy ju¿ ¿adnego wizualizatora
    //BOOST_FOREACH( IVisualizerChannel* channel, visualizerChannels ) {
    //    channel->releaseChannel();
    //}

    //visualizerChannels.swap(VisualizerChannels());
}

IVisualizerConstPtr VisualizerManager::getPrototype( UniqueID id ) const
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
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
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
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
	for (auto it = mapType2ID.begin(); it != mapType2ID.end(); ++it) {
		if (dataManager->isTypeCompatible(typeInfo, it->first)) {
			return createVisualizer(it->second);
		}
	}
	throw std::runtime_error("Visualizer not registered.");
}

void VisualizerManager::update()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
    Visualizers::iterator it = visualizers.begin();
    Visualizers::iterator last = visualizers.end();
    while ( it != last ) {
        (*it)->update(0);
        ++it;
    }
}

void VisualizerManager::registerVisualizer( IVisualizerPtr visualizer )
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
    if (!getPrototype(visualizer->getID())) {

        std::vector<IInputDescription::InputInfo> visualizerInputInfo;

        visualizer->getInputInfo(visualizerInputInfo);

        if(visualizerInputInfo.empty() == true){
            LOG_WARNING("Visualizer " << visualizer->getName() << " ID = " << visualizer->getID() << " does not support minimum one data type! NOT registered in application");
        }else{
			// wypelniamy mape, dzieki ktorej mozemy latwo stworzyc wizualizator na postawie typu
			for (auto it = visualizerInputInfo.begin(); it != visualizerInputInfo.end(); ++it) {
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

            delete icon;

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
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
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
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
    LOG_DEBUG("Visualizer " << visualizer->getName() << " created");
    visualizers.push_back(visualizer);
}

void VisualizerManager::notifyDestroyed( Visualizer* visualizer )
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(visualizersMutex);
    LOG_DEBUG("Visualizer " << visualizer->getName() << " destroyed.");
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
			}catch(std::exception & e){

			}catch(...){

			}
		}
	}

	//teraz probuje usuwac serie danych z wizualizatora
	visualizerChannels[channelIT->second.visualzier].erase(channelIT->first);

	if(visualizerChannels[channelIT->second.visualzier].empty() == true){
		visualizerChannels.erase(channelIT->second.visualzier);
	}

	channels.erase(channel);
}

const void * VisualizerManager::createChannel(const core::VisualizerTimeSeriePtr & serie, Visualizer * visualizer, const std::string & path, bool synchRemove)
{
	TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
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
			LOG_WARNING("Could not add channel to timeline because: " << e.what());
		}catch(...){
			LOG_WARNING("Could not add channel to timeline. Unknown reason.");
		}
	}

	return nullptr;
}

const void * VisualizerManager::createChannel(const std::vector<core::VisualizerTimeSeriePtr> & series, Visualizer * visualizer, const std::string & path, bool synchRemove)
{
	TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
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
			LOG_WARNING("Could not add channel to timeline because: " << e.what());
		}catch(...){
			LOG_WARNING("Could not add channel to timeline. Unknown reason.");
		}
	}

	return nullptr;
}

void VisualizerManager::removeChannel(const void * channel)
{
	//sprawdzam czy jest taki kanal
	auto channelIT = channels.find((IVisualizerChannel*)channel);
	if(channelIT == channels.end()){
		//jesli nie ma to wychodzimy
		return;
	}

	//wlasciwa proba usuniecia kanalu z wizualizatora i timeline
	TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
	bool removed = false;
	if(timeline != nullptr) {

		try{
			//oczekuje ¿e destuktor kana³u zrobi resztê wywo³uj¹c notifyDestroyed
			timeline->removeChannel(channelIT->second.path);
			removed = true;
		}catch(std::runtime_error & e){
			LOG_WARNING("Could not remove channel from timeline because: " << e.what());
		}catch(...){
			LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
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
	//wlasciwa proba usuniecia kanalu z wizualizatora i timeline
	TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
	if(timeline != nullptr) {
		for(auto channelIT = channels.begin(); channelIT != channels.end(); ++channelIT){
			try{
				//oczekuje ¿e destuktor kana³u zrobi resztê wywo³uj¹c notifyDestroyed
				timeline->removeChannel(channelIT->second.path);
			}catch(std::runtime_error & e){
				LOG_WARNING("Could not remove channel from timeline because: " << e.what());
			}catch(...){
				LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
			}
		}
	}

	std::map<IVisualizerChannel *, ChannelData>().swap(channels);
	std::map<Visualizer *, std::set<IVisualizerChannel *>>().swap(visualizerChannels);
}

void VisualizerManager::markAsRemovedFromVisualizer(const void * channel)
{
	//sprawdzam czy jest taki kanal
	auto channelIT = channels.find((IVisualizerChannel*)channel);
	if(channelIT == channels.end()){
		//jesli nie ma to wychodzimy
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

