#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>
#include "VisualizerManager.h"

using namespace core;

Visualizer::Visualizer( IVisualizer* impl ) :
impl(impl), widget(nullptr), source(VisualizerManager::getInstance()->getSourcesTypes(impl->getID()))
{
    //int numVisualizers = VisualizerManager::getInstance()->getNumInstances(impl->getID());
    //uiName = QString("%0 (%1)").arg(toString(getName())).arg(numVisualizers);
    VisualizerManager::getInstance()->notifyCreated(this);
    uiName = toString(getName());
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
impl(visualizer.impl->create()),
source(visualizer.source), widget(nullptr)
{
    //int numVisualizers = VisualizerManager::getInstance()->getNumInstances(impl->getID());
    //uiName = QString("%0 (%1)").arg(toString(getName())).arg(numVisualizers);
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
        LOG_DEBUG("Visualizer " << impl->getName() << " widget created");
        widget = impl->createWidget(genericActions);
        trySetUp();
        UTILS_ASSERT(widget, "Nie uda³o siê stworzyæ widgeta.");
    }
    return widget;
}

bool Visualizer::trySetUp()
{
    try {
        LOG_DEBUG("Visualizer " << impl->getName() << " setup");
        impl->setUp(&source);

        // aktualizacja widgetu
        UTILS_ASSERT(widget);
        widget->update();
        return true;
    } catch (const std::exception& ex) {
        LOG_ERROR("Error during setting up visualizer: " << ex.what());
        return false;
    }
}

const QIcon& Visualizer::getIcon() const
{
    return VisualizerManager::getInstance()->getIcon(getID());
}

const Visualizer::SlotsInfo& Visualizer::getSourcesTypes() const
{
    return VisualizerManager::getInstance()->getSourcesTypes(getID());
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