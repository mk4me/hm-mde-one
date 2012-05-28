#include "CorePCH.h"
#include "EDRTitleBar.h"
#include "EDRDockWidget.h"

EDRTitleBar::EDRTitleBar(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    buttonClose->setDefaultAction(actionClose);
    buttonFloat->setDefaultAction(actionFloat);
}

EDRTitleBar::~EDRTitleBar()
{
    clear();
}

void EDRTitleBar::addObject(QObject * object, SideType side)
{
    if(objectData.find(object) != objectData.end()){
        throw std::runtime_error("Object already present in titlebar!");
    }

    QFrame * frame = getSideFrame(side);
    QWidget * widget = nullptr;
    QAction * action = qobject_cast<QAction*>(object);
    if(action != nullptr){
        QToolButton* button = new QToolButton(this);
        button->setDefaultAction(action);
        widget = button;
    }else{
        QMenu * menu = qobject_cast<QMenu*>(object);
        if(menu != nullptr){
            QToolButton* button = new QToolButton(this);
            button->setText(menu->title());
            button->setPopupMode(QToolButton::InstantPopup);
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            
            if(side == Right){
                button->setLayoutDirection(Qt::LeftToRight);
            }

            button->setMenu(menu);
            widget = button;
        }
    }
    
    if(widget == nullptr){
        widget = qobject_cast<QWidget*>(object);
    }

    if(widget == nullptr){
        throw std::runtime_error("Object type not supported in titlebar!");
    }
        
    frame->layout()->addWidget(widget);
    objectData[object] = ObjectDescriptor(side, widget);
    widgetObjects[widget] = object;
}

void EDRTitleBar::removeObject(QObject * object)
{
    auto it = objectData.find(object);
    if(it == objectData.end()){
        throw std::runtime_error("Object is not present in titlebar!");
    }
    QLayout * layout = getSideFrame(it->second.first)->layout();
    
    layout->removeWidget(it->second.second);
    
    //jezeli nie widget customowy to znaczy ze akcja lub menu dla ktorych przygotowalem button,
    // reszta klient sam zarzadza
    if(it->first != it->second.second){
        delete it->second.second;
    }

    widgetObjects.erase(it->second.second);
    objectData.erase(it);
}

QFrame * EDRTitleBar::getSideFrame(SideType side) const
{
    QFrame * frame = nullptr;
    switch(side)
    {
    case Left:
        frame = leftFrame;
        break;

    case Right:
        frame = rightFrame;
        break;
    }

    return frame;
}

void EDRTitleBar::clearSide(SideType side)
{
    QLayout * layout = getSideFrame(side)->layout();
    for(int i = layout->count() - 1; i > -1; --i){
        auto it = widgetObjects.find(layout->itemAt(i)->widget());

        layout->removeWidget(it->first);
        delete it->first;

        objectData.erase(it->second);
        
        widgetObjects.erase(it);
    }
}

void EDRTitleBar::clear()
{
    clearSide(Left);
    clearSide(Right);

    objectData.swap(ObjectData());
    widgetObjects.swap(WidgetObjects());
}

bool EDRTitleBar::isCloseButtonVisible() const
{
    return buttonClose->isVisible();
}

void EDRTitleBar::setCloseButtonVisible(bool visible)
{
    buttonClose->setVisible(visible);
}

bool EDRTitleBar::isFloatButtonVisible() const
{
    return buttonFloat->isVisible();
}

void EDRTitleBar::setFloatButtonVisible(bool visible)
{
    buttonFloat->setVisible(visible);
}

void EDRTitleBar::paintEvent( QPaintEvent *paintEvent )
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // PE_Widget
}

bool EDRTitleBar::isTitleVisible() const
{
    return titleLabel->isVisible();
}

QString EDRTitleBar::getTitle() const
{
    return titleLabel->text();
}

void EDRTitleBar::setTitleVisible(bool visible)
{
    titleLabel->setVisible(visible);
}

void EDRTitleBar::setTitle(const QString & title)
{
    titleLabel->setText(title);
}

void EDRTitleBar::refreshFeatures(QDockWidget::DockWidgetFeatures features)
{
    if(features & QDockWidget::DockWidgetClosable){
        buttonClose->setVisible(true);
    }else{
        buttonClose->setVisible(false);
    }

    if(features & QDockWidget::DockWidgetFloatable){
        buttonFloat->setVisible(true);
    }else{
        buttonFloat->setVisible(false);
    }

    if(features & QDockWidget::DockWidgetMovable){
        setEnabled(true);
    }else{
        setEnabled(false);
    }
}

EDRTitleBar * supplyWithEDRTitleBar(EDRDockWidget * dockWidget, bool refresh)
{
    if(dockWidget == nullptr){
        throw std::runtime_error("EDRTitleBar supply request for uninitialized EDRDockWidget");
    }

    if(dockWidget->titleBarWidget() != nullptr){
        LOG_WARNING("DockWidget already has a titlebar!!");
        return nullptr;
    }

    auto titleBar = new EDRTitleBar();

    dockWidget->setTitleBarWidget(titleBar);

    QObject::connect(dockWidget, SIGNAL(windowTitleChanged(const QString &)), titleBar, SLOT(setTitle(const QString &)));
    QObject::connect(dockWidget, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), titleBar, SLOT(refreshFeatures(QDockWidget::DockWidgetFeatures)));
    QObject::connect(titleBar->actionFloat, SIGNAL(triggered()), dockWidget, SLOT(toggleFloating()));
    QObject::connect(titleBar->actionClose, SIGNAL(triggered()), dockWidget, SLOT(close()));
    
    //odswiezam titlebar!!
    if(refresh == true){
        titleBar->setTitle(dockWidget->windowTitle());
        titleBar->refreshFeatures(dockWidget->features());
    }

    return titleBar;
}