#include "CorePCH.h"
#include "EDRDockWidget.h"

EDRDockWidget::EDRDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags), titleBar(new EDRTitleBar()), innerWidget(new EDRDockInnerWidget()),
    emptyTitleBar( new QWidget())
{
    init();
}

EDRDockWidget::EDRDockWidget(QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags), titleBar(new EDRTitleBar()), innerWidget(new EDRDockInnerWidget()),
    emptyTitleBar( new QWidget())
{
    init();
    setWindowTitle("Default EDRDockWidget");
}

EDRDockWidget::~EDRDockWidget()
{

}

void EDRDockWidget::init()
{
    showTitleBar = true;

    //mo¿na focusowaæ widget
    setFocusPolicy(Qt::StrongFocus);

    connect( this, SIGNAL(topLevelChanged(bool)), this, SLOT(onTopLevelChange(bool)) );
    connect( titleBar->actionClose, SIGNAL(triggered()), this, SLOT(close()) );
    connect( titleBar->actionFloat, SIGNAL(triggered(bool)), this, SLOT(setFloating(bool)) );

    setTitleBarWidget(titleBar);

    setWidget(innerWidget);
}

void EDRDockWidget::setTitleBarVisible(bool visible)
{
    if(showTitleBar != visible){
        showTitleBar = visible;
        
        if(showTitleBar == true){
            setTitleBarWidget(titleBar);
        }else{
            setTitleBarWidget(emptyTitleBar);
        }
    }
}

bool EDRDockWidget::isTitlebarVisible() const
{
    return showTitleBar;
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

bool EDRDockWidget::isPermanent() const
{
    return testAttribute(Qt::WA_DeleteOnClose) == false;
}

void EDRDockWidget::setPermanent(bool permanent)
{
    setAttribute(Qt::WA_DeleteOnClose, permanent);
}

void EDRDockWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}

void EDRDockWidget::setFloating( bool floating )
{
    QDockWidget::setFloating( floating );
}

void EDRDockWidget::onTopLevelChange(bool topLevel)
{
    if(topLevel == true){
        Qt::WindowFlags flags = this->windowFlags();
        if( (flags & Qt::FramelessWindowHint) == Qt::FramelessWindowHint){
            this->setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint);
            this->show();
        }
    }
}

void EDRDockWidget::focusInEvent(QFocusEvent * event)
{
    QDockWidget::focusInEvent(event);
    emit focuseGained();
}