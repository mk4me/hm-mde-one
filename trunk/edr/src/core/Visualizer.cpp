#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/Visualizer.h>

using namespace core;

Visualizer::Visualizer( IVisualizer* impl ) :
impl(impl), widget(nullptr), source(impl)
{
    LOG_DEBUG("Visualizer " << impl->getName() << " created");
}

Visualizer::~Visualizer()
{
    LOG_DEBUG("Visualizer " << impl->getName() << " destroyed.");
}

QWidget* Visualizer::getOrCreateWidget()
{
    if (!widget) {
        LOG_DEBUG("Visualizer " << impl->getName() << " widget created");
        widget = impl->createWidget();
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