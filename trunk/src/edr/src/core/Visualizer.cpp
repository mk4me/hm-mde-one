#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>
#include "VisualizerManager.h"
#include "VisualizerWidget.h"
#include "PrintWidgetAction.h"
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
        //PrintWidgetAction* print = new PrintWidgetAction(widget, "Print visualizer", widget);
        QAction* print = new QAction("Print visualizer", widget);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/screenshot-b.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon.addFile(QString::fromUtf8(":/resources/icons/screenshot-a.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        print->setIcon(icon);
        genericActions.push_back(print);
        connect(print, SIGNAL(triggered()), this, SLOT(printActionPressed()));
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

    return *it;
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


IVisualizerChannel::IVisualizerChannel(const std::string & path, Visualizer * visualizer) : path(path), visualizer(visualizer), managed(false)
{
    VisualizerManager::getInstance()->notifyCreated(this);
}

IVisualizerChannel::~IVisualizerChannel()
{
    //if(VisualizerManager::getInstance() != nullptr){
    if(managed == false){
        VisualizerManager::getInstance()->notifyDestroyed(this);
    }
    //}
}

const std::string & IVisualizerChannel::getChannelPath() const
{
    return path;
}

const Visualizer * IVisualizerChannel::getVisualizer() const
{
    return visualizer;
}

Visualizer * IVisualizerChannel::getVisualizer()
{
    return visualizer;
}

VisualizerChannel::VisualizerChannel(const std::string & path, Visualizer * visualizer, const core::VisualizerTimeSeriePtr & serie)
    : IVisualizerChannel(path, visualizer), serie(serie), constSerie(serie)
{

}

//void VisualizerChannel::releaseChannel()
//{
//    serie = core::VisualizerTimeSeriePtr();
//    visualizer = nullptr;
//}

VisualizerChannel::~VisualizerChannel()
{
    /*if(visualizer!= nullptr && visualizer->getCurrentVisualizer()){
        visualizer->getCurrentVisualizer()->removeSerie(serie);
    }*/
}

void VisualizerChannel::setTime(double time)
{
    serie->setTime(time);
}

double VisualizerChannel::getLength() const
{
    return serie->getLength();
}

VisualizerChannel * VisualizerChannel::clone() const
{
    //TODO
    //mo¿na tutaj wprowadzic zarz¹dzanie klonowaniem serii

    return nullptr;
}

const core::VisualizerTimeSeriePtr & VisualizerChannel::getSerie()
{
    return serie;
}

const core::VisualizerTimeSerieConstPtr & VisualizerChannel::getSerie() const
{
    return constSerie;
}

void Visualizer::printActionPressed()
{
    QPixmap p = getImplementation()->print();
    emit this->printTriggered(p);
}

//VisualizerWidget * VisualizerChannel::getVisualizer()
//{
//    return visualizer;
//}
//
//const VisualizerWidget * VisualizerChannel::getVisualizer() const
//{
//    return visualizer;
//}

VisualizerMultiChannel::VisualizerMultiChannel(const std::string & path, Visualizer * visualizer, const SeriesWidgets seriesWidgets)
    : IVisualizerChannel(path, visualizer), seriesWidgets(seriesWidgets)
{
    UTILS_ASSERT((seriesWidgets.empty() == false), "Nie podano ¿adnych serii dla kanalu");

    auto it = seriesWidgets.begin();

    length = (*it)->getLength();

    it++;

    for( ; it != seriesWidgets.end(); it++){
        if(length < (*it)->getLength()){
            length = (*it)->getLength();
        }
    }
}

//void VisualizerMultiChannel::releaseChannel()
//{
//    //seriesWidgets.swap(SeriesWidgets());
//}

VisualizerMultiChannel::~VisualizerMultiChannel()
{
    /*for(auto it = seriesWidgets.begin(); it != seriesWidgets.end(); it++){
        if(it->second->getCurrentVisualizer() != nullptr){
            it->second->getCurrentVisualizer()->removeSerie(it->first);
        }
    }*/
}

void VisualizerMultiChannel::setTime(double time)
{
    for(auto it = seriesWidgets.begin(); it != seriesWidgets.end(); it++){
        (*it)->setTime(time);
    }
}

double VisualizerMultiChannel::getLength() const
{
    return length;
}

VisualizerMultiChannel * VisualizerMultiChannel::clone() const
{
    //TODO
    //mo¿na tutaj wprowadzic zarz¹dzanie klonowaniem serii

    return nullptr;
}

const VisualizerMultiChannel::SeriesWidgets & VisualizerMultiChannel::getSeriesWidgets() const
{
    return seriesWidgets;
}