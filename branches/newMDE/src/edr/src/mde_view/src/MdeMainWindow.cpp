#include "MdePCH.h"
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QSplashScreen>
#include <QtGui/QMainWindow>
#include <utils/Debug.h>
#include "MdeMainWindow.h"
#include <coreui/CoreTextEditWidget.h>
#include <QtGui/QApplication>
#include <QtGui/QTreeView>
#include "AboutDialog.h"
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreDockWidget.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include "IMdeTab.h"
#include "SimpleTab.h"
#include "ReportsTab.h"
#include "AnalisisWidget.h"
#include "AnalysisTab.h"
#include "ui_toolboxmaindeffile.h"

using namespace core;

MdeMainWindow::MdeMainWindow(const CloseUpOperations & closeUpOperations) :
    coreUI::CoreMainWindow(closeUpOperations),
    controller(this)
{
    ui = new Ui::HMMMain();
    ui->setupUi(this);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    contextPlaceholder = new QTabWidget(this);
    contextPlaceholder->setTabsClosable(false);
    //contextPlaceholder->setDocumentMode(true);
    contextPlaceholder->setMovable(false);
    contextPlaceholder->setVisible(false);

    //connect(contextPlaceholder, SIGNAL(currentChanged(int)), this, SLOT(onContextChange(int)));
    //connect(this, SIGNAL(onSwitchToAnalysis()), this, SLOT(safeSwitchToAnalysis()), Qt::QueuedConnection);

    ui->tabPlaceholder->layout()->addWidget(contextPlaceholder);
    ui->tabPlaceholder->show();
    contextPlaceholder->show();
    ui->templateButton->hide();
    //contextPlaceholder->findChild<QTabBar*>()->setDrawBase(false);

    contextEventFilter = ContextEventFilterPtr(new ContextEventFilter(this));
    analysisModel = AnalisisModelPtr(new AnalisisModel());
}

MdeMainWindow::~MdeMainWindow()
{
}

void MdeMainWindow::initializeSplashScreen(QSplashScreen * splashScreen)
{
    splashScreen->setPixmap(QPixmap(":/resources/splashscreen/splash.png"));
}

void MdeMainWindow::showSplashScreenMessage(const QString & message)
{
    splashScreen()->showMessage(message, Qt::AlignBottom | Qt::AlignLeft, Qt::white);
}

void MdeMainWindow::customViewInit(QWidget * console)
{
   auto memoryManager = plugin::getDataManagerReader();
   memoryManager->addObserver(analysisModel);
   trySetStyleByName("hmm");
 
   auto sourceManager = plugin::getSourceManager();
   for (int i = 0; i < sourceManager->getNumSources(); ++i) {
       auto source = sourceManager->getSource(i);
       QWidget* widget = source->getWidget();
       if (widget) {
           addTab(IMdeTabPtr(new SimpleTab(widget, QIcon(":/mde/icons/Badania.png"),tr(source->getName().c_str()))));
       }
   }
   
   AnalisisWidget* aw = new AnalisisWidget(analysisModel, contextEventFilter, nullptr);
   // -----------------------

   addTab(IMdeTabPtr(new AnalysisTab(aw, QIcon(":/mde/icons/Analizy.png"), tr("Analysis"))));
   IMdeTabPtr reportsTab  = IMdeTabPtr(new ReportsTab(QIcon(":/mde/icons/Raporty.png"), tr("Reports")));
   addTab(reportsTab);
   addTab(IMdeTabPtr(new SimpleTab(console, QIcon(":/mde/icons/Operacje.png"),tr("Console"))));

   // TODO : najlepiej byloby przeniesc to do kontrolera
   bool cc = connect(analysisModel.get(), SIGNAL(reportCreated(const QString&)), reportsTab->getMainWidget(), SLOT(setHtml(const QString&)));
   auto serviceManager = plugin::getServiceManager();
   auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
   for (int i = 0; i < serviceManager->getNumServices(); ++i) {
       plugin::IServicePtr service = serviceManager->getService(i);
       
       if (service != timeline) {
            auto w = createServiceWidget(service);
            if (w) {
                addTab(IMdeTabPtr(new SimpleTab(w, QIcon(":/mde/icons/Operacje.png"),tr(service->getName().c_str()))));
            }
       }

        core::IFilterProviderPtr filterProvider = utils::dynamic_pointer_cast<core::IFilterProvider>(service);
        if (filterProvider) {
            analysisModel->addFilterBundles(filterProvider->getFilterBundles());
        }
   }

   emit activateTab(*tabs.begin());
}

void MdeMainWindow::addTab( IMdeTabPtr tab )
{
    tabs.push_back(tab);
    emit tabAdded(tab);
}

// todo przeniesc
QWidget* MdeMainWindow::createServiceWidget( plugin::IServicePtr service )
{
    QWidget* viewWidget = service->getWidget();
    
    QMainWindow* w = nullptr;
     if(viewWidget) {
        w = new QMainWindow();
        w->setCentralWidget(viewWidget);
        w->addActions(viewWidget->actions());

        QWidgetList properites = service->getPropertiesWidgets();
        if (!properites.empty()) {
            auto it = properites.begin();
            coreUI::CoreDockWidget* first = new coreUI::CoreDockWidget((*it)->objectName());
            first->setWidget(*it);
            w->addDockWidget(Qt::RightDockWidgetArea, first);
            for (++it; it != properites.end(); ++it) {
                coreUI::CoreDockWidget* dw = new coreUI::CoreDockWidget((*it)->objectName());
                dw->setWidget(*it);
                w->addDockWidget(Qt::RightDockWidgetArea, dw);
                w->tabifyDockWidget(first, dw);
            }
        }
    }

    return w;
}

void MdeMainWindowController::addTab( IMdeTabPtr tab )
{
    QToolButton* templateB = window->ui->templateButton;
    QToolButton* button = new QToolButton();
    button->setText(tab->getLabel());
    button->setIcon(tab->getIcon());
    button->setIconSize(templateB->iconSize());
    button->setToolButtonStyle(templateB->toolButtonStyle());
    button->setStyleSheet(templateB->styleSheet());
    button->setMinimumSize(templateB->minimumSize());
    button->setCheckable(true);
    button->setFixedWidth(templateB->width());

    QLayout* layout = window->ui->toolBar->layout();
    auto count = layout->count();
    auto item = layout->itemAt(count - 1);
    layout->removeItem(item);
    layout->addWidget(button);
    layout->addItem(item);

    button2TabWindow[button] = tab;
    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    auto context = IAppUsageContextPtr(new MdeEmptyContext());
    tab2Contex[tab] = context;

    IAppUsageContextManager* contextManager = dynamic_cast<IAppUsageContextManager*>(window);
    contextManager->addContext(context);
    contextManager->addWidgetToContext(context, button);
    IContextRootPtr tabContex = utils::dynamic_pointer_cast<IContextRoot>(tab);
    if (tabContex) {
        tabContex->registerConxtext(contextManager, context, window->contextPlaceholder);
    }

    QWidget* widget = tab->getMainWidget();
    window->ui->mainArea->layout()->addWidget(widget);
    widget->hide();
}

MdeMainWindowController::MdeMainWindowController( MdeMainWindow* mw ) : 
window(mw)
{
    connect(mw, SIGNAL(activateTab(IMdeTabPtr)), this, SLOT(activateTab(IMdeTabPtr)));
    connect(mw, SIGNAL(tabAdded(IMdeTabPtr)), this, SLOT(addTab(IMdeTabPtr)));
}

void MdeMainWindowController::buttonClicked()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    UTILS_ASSERT(button);
    IMdeTabPtr tab = button2TabWindow[button];
    activateTab(tab);
    if (window->isContextWidget(tab->getMainWidget())) {
        window->setCurrentContext(tab->getMainWidget());
    } else {
        window->setCurrentContext(button);
    }
}

void MdeMainWindowController::activateTab( IMdeTabPtr tab )
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        IMdeTabPtr t = it->second;
        t->setActive(t == tab);
        t->getMainWidget()->setVisible(t == tab);
        it->first->setChecked( t == tab);
    }
}


