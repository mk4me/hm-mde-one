#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>
#include "VisualizerManager.h"

using namespace core;

Visualizer::Visualizer( IVisualizer* impl ) :
impl(impl), widget(nullptr), source(impl)
{
    LOG_DEBUG("Visualizer " << impl->getName() << " created");
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
impl(visualizer.impl->create()),
source(visualizer.source), widget(nullptr)
{
}

Visualizer::~Visualizer()
{
    LOG_DEBUG("Visualizer " << impl->getName() << " destroyed.");
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

const Visualizer::SourcesTypes& Visualizer::getSourcesTypes() const
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