#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>
#include "VisualizerManager.h"

#include <plugins/newTimeline/ITimelineService.h>

using namespace core;

Visualizer::Visualizer( IVisualizer* impl ) : 
    InputItem<IVisualizer>(impl, VisualizerManager::getInstance()->getSourcesTypes(impl->getID())),
    widget(nullptr)
{
    VisualizerManager::getInstance()->notifyCreated(this);
    uiName = toString(getName());
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
 InputItem<core::IVisualizer>(
     dynamic_cast<IVisualizer*> (visualizer.getImplementation()->createClone()),
     VisualizerManager::getInstance()->getSourcesTypes(getImplementation()->getID())),
 widget(nullptr)
{
    VisualizerManager::getInstance()->notifyCreated(this);
    uiName = toString(getName());
}

Visualizer::~Visualizer()
{
    VisualizerManager::getInstance()->notifyDestroyed(this);
}

QWidget* Visualizer::getOrCreateWidget()
{
    if (!widget) {
        LOG_DEBUG("Visualizer " << getImplementation()->getName() << " widget created");
        widget = getImplementation()->createWidget(genericActions);
        tryRun();
        UTILS_ASSERT(widget, "Nie uda³o siê stworzyæ widgeta.");
    }
    return widget;
}

const QIcon& Visualizer::getIcon() const
{
    return VisualizerManager::getInstance()->getIcon(getID());
}

const std::vector<QObject*>& Visualizer::getGenericActions() const
{
    return genericActions;
}

const std::vector<QObject*>& Visualizer::getOrCreateGenericActions()
{
    if ( !widget ) {
        getOrCreateWidget();
    }
    return genericActions;
}

void Visualizer::setUIName( const QString& uiName )
{
    this->uiName = uiName;
}

const QString& Visualizer::getUIName() const
{
    return uiName;
}

QWidget* Visualizer::getWidget()
{
    return widget;
}

void Visualizer::update(double deltaTime)
{
    getImplementation()->update(deltaTime);
}

int Visualizer::getMaxSeries() const
{
    return getImplementation()->getMaxDataSeries();
}

const core::VisualizerSeriePtr & Visualizer::createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
{
    core::VisualizerSeriePtr serie(getImplementation()->createSerie(data, name));
    
    if(serie->getName().empty() == true){
        serie->setName(name);
    }

    auto it = dataSeries.insert(serie).first;

    TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
    if(timeline != nullptr && dynamic_cast<timeline::IChannel*>(serie.get()) != nullptr) {
		timeline::IChannelPtr channel = core::dynamic_pointer_cast<timeline::IChannel>(serie);
        try{
            timeline->addChannel(serie->getName(), channel);
        }catch(std::runtime_error e){
            LOG_WARNING("Could not add channel to timeline because: " << e.what());
        }catch(...){
            LOG_WARNING("Could not add channel to timeline. Unknown reason.");
        }
        timelineDataSeries.insert(serie);
    }

    return *it;
}

void Visualizer::removeSerie(const core::VisualizerSeriePtr & serie)
{
    //sprawdz czy to nie seria w timeline
    auto it = timelineDataSeries.find(serie);
    if(it != timelineDataSeries.end()){
        //usun z timeline
        TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
        try{
            timeline->removeChannel(serie->getName());
        }catch(std::runtime_error e){
            LOG_WARNING("Could not remove channel from timeline because: " << e.what());
        }catch(...){
            LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
        }
        timelineDataSeries.erase(it);
    }

    getImplementation()->removeSerie(serie.get());
    dataSeries.erase(serie);
}

void Visualizer::clearAllSeries()
{
    TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());

    while(timelineDataSeries.empty() == false){
        //usun z timeline
        try{
            timeline->removeChannel((*(timelineDataSeries.begin()))->getName());
        }catch(std::runtime_error e){
            LOG_WARNING("Could not remove channel from timeline because: " << e.what());
        }catch(...){
            LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
        }
        getImplementation()->removeSerie((*timelineDataSeries.begin()).get());
        dataSeries.erase(*(timelineDataSeries.begin()));
        timelineDataSeries.erase(timelineDataSeries.begin());
    }

    while(dataSeries.empty() == false){
        getImplementation()->removeSerie((*dataSeries.begin()).get());
        dataSeries.erase(dataSeries.begin());
    }

    dataSeries.swap(DataSeries());
    timelineDataSeries.swap(DataSeries());
}

const Visualizer::DataSeries & Visualizer::getDataSeries() const
{
    return dataSeries;
}

const Visualizer::DataSeries & Visualizer::getTimelineDataSeries() const
{
    return timelineDataSeries;
}

void Visualizer::reset()
{
    clearAllSeries();
}