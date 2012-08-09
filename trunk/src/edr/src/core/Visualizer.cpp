#include "CorePCH.h"
#include <QtGui/QAction>
#include "Visualizer.h"
#include "VisualizerManager.h"
#include "VisualizerChannel.h"
#include "VisualizerWidget.h"

#include <core/ObjectWrapper.h>
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

        widget = getImplementation()->createWidget(&genericActions);
        //PrintWidgetAction* print = new PrintWidgetAction(widget, "Print visualizer", widget);
        QAction* print = new QAction("Print visualizer", widget);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/screenshot-b.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/resources/icons/screenshot-a.png"), QSize(), QIcon::Normal, QIcon::On);
        print->setIcon(icon);
        connect(print, SIGNAL(triggered()), this, SLOT(printActionPressed()));

        core::IActionsGroupManager::GroupID id = genericActions.createGroup(tr("Common"));
        genericActions.addGroupAction(id, print);

        tryRun();
        UTILS_ASSERT(widget, "Nie uda³o siê stworzyæ widgeta.");
    }
    return widget;
}

const QIcon& Visualizer::getIcon() const
{
    return VisualizerManager::getInstance()->getIcon(getID());
}

const ActionsGroupManager& Visualizer::getGenericActions() const
{
    return genericActions;
}

const ActionsGroupManager& Visualizer::getOrCreateGenericActions()
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

    DataSeries().swap(dataSeries);
}

const Visualizer::DataSeries & Visualizer::getDataSeries() const
{
    return dataSeries;
}

void Visualizer::reset()
{
    clearAllSeries();
}

void Visualizer::printActionPressed()
{
    QPixmap p = getImplementation()->print();
    emit this->printTriggered(p);
}
