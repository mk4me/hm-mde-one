#include "CorePCH.h"
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerWidget.h"
#include "DataManager.h"
#include "ui_VisualizerWidget.h"
#include "ToolboxMain.h"

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
        addVisualizerToTitlebar(vis);
    }
}

VisualizerWidget::VisualizerWidget( UniqueID visualizerID, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ )
{
    init();
    // blokujemy sygna³y 
    titleBar->comboType->blockSignals(true);
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        addVisualizerToTitlebar(vis);
    }
    // ustawiamy wizualizator
    titleBar->comboType->blockSignals(false);
    titleBar->setCurrentVisualizer(visualizerID);
}

VisualizerWidget::VisualizerWidget( const VisualizerPtr& source, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ )
{
    init();
    // blokujemy sygna³y
    titleBar->comboType->blockSignals(true);
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        addVisualizerToTitlebar(vis);
    }
    // ustawiamy wizualizator
    titleBar->comboType->blockSignals(false);
    // ustawiamy wizualizator
    titleBar->setCurrentVisualizer(source);
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
        //layout->insertWidget(0, titleBar);
        // bo jest hidden po usuniêciu
        //titleBar->show();
    } else {
        // przywrócenie toolbara
        UTILS_ASSERT(!titleBarWidget());
        //layout->removeWidget(titleBar);
        //setTitleBarWidget( titleBar );
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
    // usuniêcie widgeta z layoutu, nie w ogóle!
    // dodanie widgata do layoutu powoduje, ¿e ramka staje siê jego rodzicem;
    // dlatego przed usuniêciem itema trzeba zmieniæ rodzica widgetowi
    UTILS_ASSERT(innerWidget->layoutContent->count() <= 1);
    if ( innerWidget->layoutContent->count() > 0 ) {
        QLayoutItem* item = innerWidget->layoutContent->takeAt(0);
        if ( item ) {
            QWidget* widget = item->widget();
            widget->setParent(nullptr);
            delete item;
        }
    }

    if ( visualizer ) {
        QWidget* widget = visualizer->getOrCreateWidget();
        innerWidget->layoutContent->addWidget(widget);
        innerWidget->setWindowTitle( visualizer->getUIName() );
        setWindowTitle( innerWidget->windowTitle() );




//         // ustalenie nazwy widgetu
//         int numVisualizers = VisualizerManager::getInstance()->getNumInsances(visualizer->getID());
//         if ( numVisualizers == 1 ) {
//             setWindowTitle( innerWidget->windowTitle() );
//         } else {
//             // jest wiêcej wizualizatorów, trzeba wszystkim widgetom nadaæ odpowiednie nazwy
//             QList<VisualizerWidget*> widgets = qFindChildren<VisualizerWidget*>(parent());
//             int currVisualizer = 0;
//             BOOST_FOREACH(VisualizerWidget* widget, widgets) {
//                 if ( widget->getTitleBar()->getCurrentVisualizer()->getID() == visualizer->getID() ) {
//                     widget->setWindowTitle( QString("{0} ({1})").arg( core::toString(visualizer->getName())).arg(currVisualizer++));
//                 }
//             }
//         }
//         //setObjectName( );
    } else {
        setWindowTitle( "Empty visualizer");
    }
// 
//     ToolboxMain* mainwnd = qobject_cast<ToolboxMain*>(this->parentWidget());
//     UTILS_ASSERT(mainwnd);
//     if ( mainwnd ) {
//         mainwnd->refreshVisualizerWidgetsNames();
//     }

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

    QObject::connect( titleBar->actionSplitVertically, SIGNAL(triggered()), this, SLOT(splitVertically()));
    QObject::connect( titleBar->actionSplitHorizontally, SIGNAL(triggered()), this, SLOT(splitHorizontally()));

    QObject::connect( this, SIGNAL(topLevelChanged(bool)), titleBar->buttonSplitH, SLOT(setHidden(bool)));
    QObject::connect( this, SIGNAL(topLevelChanged(bool)), titleBar->buttonSplitV, SLOT(setHidden(bool)));

    // niszczymy przy zamykaniu, nie chowamy!
    setAttribute(Qt::WA_DeleteOnClose, true);

    setTitleBarWidget(titleBar);
}

void VisualizerWidget::splitHorizontally()
{
    // to nie bug!
    split(Qt::Vertical);
}

void VisualizerWidget::splitVertically()
{
    // to nie bug!
    split(Qt::Horizontal);
}

void VisualizerWidget::split( Qt::Orientation orientation )
{
    VisualizerWidget* dockWidget;
    VisualizerPtr visualizer = titleBar->getCurrentVisualizer();

    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent());
    UTILS_ASSERT(mainWindow);

    if ( visualizer ) {
        dockWidget = new VisualizerWidget(VisualizerManager::getInstance()->createVisualizer(visualizer->getID()), mainWindow);
    } else {
        dockWidget = new VisualizerWidget(mainWindow);
    }
    mainWindow->splitDockWidget(this, dockWidget, orientation);
    
}

inline void VisualizerWidget::addVisualizerToTitlebar( const core::IVisualizerConstPtr& vis )
{
    titleBar->addVisualizer( VisualizerManager::getInstance()->getIcon(vis->getID()),  toQString(vis->getName()), vis->getID() );
}

void VisualizerWidget::closeEvent( QCloseEvent *event )
{
    ToolboxMain* mainwnd = qobject_cast<ToolboxMain*>(this->parentWidget());
    UTILS_ASSERT(mainwnd);
    if ( mainwnd ) {
        mainwnd->visualizerWidgetClosed(this);
    }
    QDockWidget::closeEvent(event);
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