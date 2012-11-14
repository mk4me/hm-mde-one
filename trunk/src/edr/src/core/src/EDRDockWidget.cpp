#include "CorePCH.h"
#include "EDRDockWidget.h"

EDRDockWidget::EDRDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags)
{

}

EDRDockWidget::EDRDockWidget(QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    //setWindowTitle("Default EDRDockWidget");
}

EDRDockWidget::~EDRDockWidget()
{

}

bool EDRDockWidget::isPermanent() const
{
    return testAttribute(Qt::WA_DeleteOnClose) == false;
}

void EDRDockWidget::setPermanent(bool permanent)
{
    setAttribute(Qt::WA_DeleteOnClose, !permanent);
}

void EDRDockWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}

void EDRDockWidget::setWindowTitle(const QString & title)
{
    QWidget::setWindowTitle(title);
    emit windowTitleChanged(title);
}
