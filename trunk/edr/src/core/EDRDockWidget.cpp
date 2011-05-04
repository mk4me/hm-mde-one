#include "CorePCH.h"
#include "EDRDockWidget.h"

EDRDockWidget::EDRDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags), titleBar(new EDRTitleBar()), innerWidget(new EDRDockInnerWidget())
{
    init();
}

EDRDockWidget::EDRDockWidget(QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags), titleBar(new EDRTitleBar()), innerWidget(new EDRDockInnerWidget())
{
    init();
    setWindowTitle("Default EDRDockWidget");
}

EDRDockWidget::~EDRDockWidget()
{

}

void EDRDockWidget::init()
{
    QObject::connect( titleBar->actionClose, SIGNAL(triggered()), this, SLOT(close()) );
    QObject::connect( titleBar->actionFloat, SIGNAL(triggered(bool)), this, SLOT(setFloating(bool)) );

    setTitleBarWidget(titleBar);

    setWidget(innerWidget);
}

EDRDockInnerWidget * EDRDockWidget::getInnerWidget()
{
    return innerWidget;
}

const EDRDockInnerWidget * EDRDockWidget::getInnerWidget() const
{
    return innerWidget;
}

EDRTitleBar * EDRDockWidget::getTitleBar()
{
    return titleBar;
}

const EDRTitleBar * EDRDockWidget::getTitleBar() const
{
    return titleBar;
}

void EDRDockWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}

void EDRDockWidget::setFloating( bool floating )
{
    QDockWidget::setFloating( floating );
}