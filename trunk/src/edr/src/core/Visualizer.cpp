#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>
#include "VisualizerManager.h"

#include <plugins/newTimeline/TimelineService.h>

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
    auto it = dataSeries.insert(serie).first;

    TimelinePtr timeline = core::queryServices<TimelineService>(core::getServiceManager());
    if(timeline != nullptr && dynamic_cast<timeline::IChannel*>(serie.get()) != nullptr) {
        timeline->addChannel(data->getName(), core::dynamic_pointer_cast<timeline::IChannel>(serie));
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
        TimelinePtr timeline = core::queryServices<TimelineService>(core::getServiceManager());
        //timeline->removeChannel(serie->getName());
        timelineDataSeries.erase(it);
    }

    getImplementation()->removeSerie(serie.get());
    dataSeries.erase(serie);
}

void Visualizer::clearAllSeries()
{
    while(timelineDataSeries.empty() == false){
        //usun z timeline

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