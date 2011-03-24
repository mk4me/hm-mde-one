#include "CorePCH.h"
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerWidget.h"
#include "DataManager.h"
#include "ui_VisualizerWidget.h"

using namespace core;


//! Wewnêtrzny widget.
class VisualizerWidget::InnerWidget : public QWidget, private Ui::VisualizerWidget
{
    friend class VisualizerWidget;
public:
    InnerWidget()
    {
        setupUi(this);
    }
};

VisualizerWidget::VisualizerWidget(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/) :
QDockWidget(parent, flags)
{
    init();
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        titleBar->addVisualizer( toQString(vis->getName()), vis->getID() );
    }
}

VisualizerWidget::VisualizerWidget( UniqueID visualizerID, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ )
{
    init();
    // blokujemy sygna³y 
    titleBar->comboType->blockSignals(true);
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        titleBar->addVisualizer( toQString(vis->getName()), vis->getID() );
    }
    // ustawiamy wizualizator
    titleBar->comboType->blockSignals(false);
    titleBar->setCurrentVisualizer(visualizerID);
}

VisualizerWidget::~VisualizerWidget()
{        
    // usuniêcie widgeta
    setVisualizer(VisualizerPtr());
}

void VisualizerWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}

void VisualizerWidget::setFloating( bool floating )
{
    QDockWidget::setFloating( floating );
}

void VisualizerWidget::setReplaceTitleBar( bool replace )
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(widget()->layout());
    UTILS_ASSERT(layout);
    if ( replace ) {
        UTILS_ASSERT(titleBarWidget());
        // usuniêcie titlebara - na Windows bez tego trzeba rêcznie implementowaæ resize itp.
        setTitleBarWidget( nullptr );
        layout->insertWidget(0, titleBar);
        // bo jest hidden po usuniêciu
        titleBar->show();
    } else {
        // przywrócenie toolbara
        UTILS_ASSERT(!titleBarWidget());
        layout->removeWidget(titleBar);
        setTitleBarWidget( titleBar );
    }
}

void VisualizerWidget::queueReplaceTitleBar( bool replace )
{
    if ( replace ) {
        QMetaObject::invokeMethod(this, "setReplaceTitleBar", Qt::QueuedConnection, Q_ARG(bool, replace));
        //setReplaceTitleBar( true );
    } else {
        setReplaceTitleBar( false );
    }
}

void VisualizerWidget::setVisualizer( const VisualizerPtr& visualizer )
{
    if ( visualizer ) {
        QWidget* widget = visualizer->getOrCreateWidget();
        innerWidget->layoutContent->addWidget(widget);
    } else {
        // usuniêcie widgeta z layoutu, nie w ogóle!
        // dodanie widgata do layoutu powoduje, ¿e ramka staje siê jego rodzicem;
        // dlatego przed usuniêciem itema trzeba zmieniæ rodzica widgetowi
        UTILS_ASSERT(innerWidget->layoutContent->count() <= 1);
        QLayoutItem* item = innerWidget->layoutContent->takeAt(0);
        if ( item ) {
            QWidget* widget = item->widget();
            widget->setParent(nullptr);
            delete item;
        }
    }

//     widgetCustomInner = visualizer->createWidget();
// 
//     //UTILS_ASSERT(widgetCustomInner && targetLayout);
//     if ( targetLayout ) {
//         targetLayout->addWidget(widgetCustomInner);
//     }
}

void VisualizerWidget::init()
{
    innerWidget = new InnerWidget();
    innerWidget->setParent(this);
    setWidget(innerWidget);

    titleBar = new VisualizerTitleBar();
    QObject::connect( titleBar->actionClose, SIGNAL(triggered()), this, SLOT(close()) );
    QObject::connect( titleBar->actionFloat, SIGNAL(triggered(bool)), this, SLOT(setFloating(bool)) );
    QObject::connect( this, SIGNAL(topLevelChanged(bool)), titleBar->actionFloat, SLOT(setChecked(bool)) );
    QObject::connect( titleBar, SIGNAL(currentVisualizerChanged(VisualizerPtr)), this, SLOT(setVisualizer(VisualizerPtr)));
    setTitleBarWidget(titleBar);
}
// void VisualizerWidget::clearLayout( QLayout* layout )
// {
//     QLayoutItem* item;
//     while ((item = layout->takeAt(0)) != nullptr) {
//         QWidget* widget = item->widget();
//         delete item;
//         delete widget;
//     }
// }