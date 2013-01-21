#include "CorePCH.h"
#include "CoreDockWidget.h"

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

bool CoreDockWidget::isPermanent() const
{
    return testAttribute(Qt::WA_DeleteOnClose) == false;
}

void CoreDockWidget::setPermanent(bool permanent)
{
    setAttribute(Qt::WA_DeleteOnClose, !permanent);
}

void CoreDockWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}
