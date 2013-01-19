#include "CorePCH.h"
#include <QtGui/QAction>
#include "Visualizer.h"
#include "VisualizerManager.h"
#include "VisualizerChannel.h"
#include "VisualizerWidget.h"

#include <core/ObjectWrapper.h>
#include <plugins/newTimeline/ITimelineService.h>

using namespace core;

Visualizer::Visualizer( plugin::IVisualizer* impl ) :
    InputItem<plugin::IVisualizer>(impl, VisualizerManager::getInstance()->getSourcesTypes(impl->getID())),
    widget(nullptr)
{
    VisualizerManager::getInstance()->notifyCreated(this);
	//TODO
    //uiName = QString::fromStdString(getName());
}

Visualizer::Visualizer( const Visualizer& visualizer )
	//TODO
	//: InputItem<plugin::IVisualizer>(dynamic_cast<plugin::IVisualizer*> (visualizer.getImplementation()->createClone()),VisualizerManager::getInstance()->getSourcesTypes(getImplementation()->getID())),
  : widget(nullptr)
{
    VisualizerManager::getInstance()->notifyCreated(this);
	//TODO
    //uiName = QString::fromStdString(getName());
}

Visualizer::~Visualizer()
{
    VisualizerManager::getInstance()->notifyDestroyed(this);
}

QWidget* Visualizer::getOrCreateWidget()
{
    if (!widget) {
		//TODO
        //CORE_LOG_DEBUG("Visualizer " << getImplementation()->getName() << " widget created");
		CORE_LOG_DEBUG("Visualizer " <<  " widget created");

		//TODO
        //widget = getImplementation()->createWidget(&genericActions);
        QAction* print = new QAction("Print visualizer", widget);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/screenshot-b.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/resources/icons/screenshot-a.png"), QSize(), QIcon::Normal, QIcon::On);
        print->setIcon(icon);
        connect(print, SIGNAL(triggered()), this, SLOT(printActionPressed()));

        IActionsGroupManager::GroupID id = genericActions.createGroup(tr("Common"));
        genericActions.addGroupAction(id, print);

        //tryRun();
        UTILS_ASSERT(widget, "Nie udało się stworzyć widgeta.");
    }
    return widget;
}

const QIcon& Visualizer::getIcon() const
{
	//TODO
	static QIcon icon = QIcon();
	return icon;
    //return VisualizerManager::getInstance()->getIcon(getID());
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
	//TODO
    //getImplementation()->update(deltaTime);
}

int Visualizer::getMaxSeries() const
{
	//TODO
    //return getImplementation()->getMaxDataSeries();
	return 0;
}

const plugin::VisualizerSeriePtr & Visualizer::createSerie(const ObjectWrapperConstPtr & data, const std::string & name)
{
	//TODO
	plugin::VisualizerSeriePtr serie;
    //plugin::VisualizerSeriePtr serie(getImplementation()->createSerie(data, name));

    if(serie->getName().empty() == true){
        serie->setName(name);
    }

    auto it = dataSeries.insert(serie).first;

    return *it;
}

void Visualizer::removeSerie(const plugin::VisualizerSeriePtr & serie)
{
	//TODO
    //getImplementation()->removeSerie(serie.get());
    dataSeries.erase(serie);
}

void Visualizer::clearAllSeries()
{
    while(dataSeries.empty() == false){
		//TODO
        //getImplementation()->removeSerie((*dataSeries.begin()).get());
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
	//TODO
    //QPixmap p = getImplementation()->print();
	QPixmap p;
    emit this->printTriggered(p);
}
