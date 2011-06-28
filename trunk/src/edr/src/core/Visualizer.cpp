#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>
#include "VisualizerManager.h"

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
    dataSeries.insert(serie);
    return *(dataSeries.find(serie));
}

void Visualizer::removeSerie(const core::VisualizerSeriePtr & serie)
{
    getImplementation()->removeSerie(serie.get());
    dataSeries.erase(serie);
}

void Visualizer::clearAllSeries()
{
    while(dataSeries.empty() == false){
        getImplementation()->removeSerie((*dataSeries.begin()).get());
        dataSeries.erase(dataSeries.begin());
    }

    dataSeries.swap(DataSeries());
}

const Visualizer::DataSeries & Visualizer::getDataSeries() const
{
    return dataSeries;
}

void Visualizer::reset()
{
    clearAllSeries();
}