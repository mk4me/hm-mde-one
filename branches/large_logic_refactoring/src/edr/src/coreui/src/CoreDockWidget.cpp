#include "CoreUiPCH.h"
#include <coreui/CoreDockWidget.h>
#include <coreui/CoreAction.h>

using namespace coreUI;

CoreDockWidget::CoreDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags)
{

}

CoreDockWidget::CoreDockWidget(QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    
}

CoreDockWidget::~CoreDockWidget()
{
	
}

const bool CoreDockWidget::isDockWidgetPermanent(const QDockWidget * dockWidget)
{
	return dockWidget->testAttribute(Qt::WA_DeleteOnClose) == false;
}

void CoreDockWidget::setDockWidgetPermament(QDockWidget * dockWidget, bool permanent)
{
	dockWidget->setAttribute(Qt::WA_DeleteOnClose, !permanent);
}

bool CoreDockWidget::isPermanent() const
{
    return isDockWidgetPermanent(this);
}

void CoreDockWidget::setPermanent(bool permanent)
{
    setDockWidgetPermament(this, permanent);
}

void CoreDockWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}
