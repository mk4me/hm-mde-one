#include "MdePCH.h"
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>
#include <QtGui/QSplashScreen>
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
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
#include <coreui/IMdeTab.h>
#include "SimpleTab.h"
#include <coreui/ReportsTab.h>
#include "AnalisisWidget.h"
#include "AnalysisTab.h"
#include "ui_toolboxmaindeffile.h"
#include "MdeServiceWindow.h"
#include "plugins/newCommunication/ICommunicationDataSource.h"
#include <corelib/IVisualizerManager.h>
#include <corelib/IVisualizer.h>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>
#include <corelib/Version.h>

using namespace core;

MdeMainWindow::MdeMainWindow(const CloseUpOperations & closeUpOperations) :
    coreUI::CoreMainWindow(closeUpOperations),
	coreUI::SingleInstanceWindow("medusa_view_{E3F0B695-7E6D-44BF-BDDF-603D2C33B1EF}"),
    controller(this)
{
    ui = new Ui::HMMMain();
    ui->setupUi(this);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    contextPlaceholder = new QTabWidget(this);
    contextPlaceholder->setTabsClosable(false);
    contextPlaceholder->setDocumentMode(true);
    contextPlaceholder->setMovable(false);
    contextPlaceholder->setVisible(false);
    
    ui->tabPlaceholder->layout()->addWidget(contextPlaceholder);
    ui->tabPlaceholder->show();
    contextPlaceholder->show();
    ui->templateButton->hide();
    contextPlaceholder->findChild<QTabBar*>()->setDrawBase(false);

    contextEventFilter = ContextEventFilterPtr(new ContextEventFilter(this));
    analysisModel = AnalisisModelPtr(new AnalisisModel());
	ui->versionLabel->setText(QString("ver. %1").arg(Version::version().c_str()));
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
   auto memoryManager = plugin::getHierarchyManagerReader();
   memoryManager->addObserver(analysisModel);
   trySetStyleByName("hmm");
 
   this->showFullScreen();
   


   


   communication::ICommunicationDataSourcePtr icomm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
   plugin::ISourcePtr commSource = utils::dynamic_pointer_cast<plugin::ISource>(icomm);
   
   auto sourceManager = plugin::getSourceManager();
   for (int i = 0; i < sourceManager->getNumSources(); ++i) {
       auto source = sourceManager->getSource(i);
       QWidget* widget = source->getWidget();
       if (source != commSource && widget) {
           addTab(coreUI::IMdeTabPtr(new SimpleTab(widget, QIcon(":/mde/icons/Badania.png"),tr(source->getName().c_str()))));
       }
   }

   QWidget* commWidget = commSource->getWidget();
   icomm->setCompactMode(true);
   commWidget->setMaximumWidth(304);

   QSplitter* compound = new QSplitter();
   compound->setLayout(new QHBoxLayout);


   AnalisisWidget* aw = new AnalisisWidget(analysisModel, contextEventFilter, nullptr);
   // -----------------------
   compound->layout()->addWidget(commWidget);
   compound->layout()->addWidget(aw);
   addTab(coreUI::IMdeTabPtr(new CompoundAnalysisTab(compound, aw, QIcon(":/mde/icons/Analizy.png"), tr("Analysis"))));
   addTab(coreUI::IMdeTabPtr(new SimpleTab(console, QIcon(":/mde/icons/Operacje.png"),tr("Console"))));

   // TODO : najlepiej byloby przeniesc to do kontrolera
//   bool cc = connect(analysisModel.get(), SIGNAL(reportCreated(const QString&)), reportsTab->getMainWidget(), SLOT(setHtml(const QString&)));
   auto serviceManager = plugin::getServiceManager();
   auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
   for (int i = 0; i < serviceManager->getNumServices(); ++i) {
       plugin::IServicePtr service = serviceManager->getService(i);
       
       if (service != timeline) {
            auto w = createServiceWidget(service);
            if (w) {
                addTab(coreUI::IMdeTabPtr(new SimpleTab(w, QIcon(":/mde/icons/Operacje.png"),tr(service->getName().c_str()))));
                addPropertiesToServiceWindow(service, w);
            }
       }

        core::IFilterProviderPtr filterProvider = utils::dynamic_pointer_cast<core::IFilterProvider>(service);
        if (filterProvider) {
            analysisModel->addFilterBundles(filterProvider->getFilterBundles());
        }
   }

   emit activateTab(*tabs.begin());
}

void MdeMainWindow::addTab( coreUI::IMdeTabPtr tab )
{
    tabs.push_back(tab);
    emit tabAdded(tab);
}

MdeServiceWindow* MdeMainWindow::createServiceWidget( plugin::IServicePtr service )
{
    QWidget* viewWidget = service->getWidget();
    
    MdeServiceWindow* w = nullptr;
    if(viewWidget) {
        QString serviceName = QString::fromStdString(service->getName());
        w = new MdeServiceWindow(serviceName);
        w->setCentralWidget(viewWidget);
        w->addActions(viewWidget->actions());

        /*QWidgetList properites = service->getPropertiesWidgets();
        if (!properites.empty()) {
            w->loadLayout();
            int propNo = 0;
            for (auto it = properites.begin(); it != properites.end(); ++it) {
                w->createMdeDock(serviceName, *it);
            }
        }*/
    }

    return w;
}

void MdeMainWindow::addPropertiesToServiceWindow( plugin::IServicePtr service, MdeServiceWindow * w )
{
    QWidgetList properites = service->getPropertiesWidgets();
    if (!properites.empty()) {
        w->loadLayout();
        int propNo = 0;
        QString serviceName = QString::fromStdString(service->getName());
        for (auto it = properites.begin(); it != properites.end(); ++it) {
            w->createMdeDock(serviceName, *it);
        }
    }
}

void MdeMainWindowController::addTab( coreUI::IMdeTabPtr tab )
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

    auto context = coreUI::IAppUsageContextPtr(new coreUI::MdeEmptyContext());
    tab2Contex[tab] = context;

    coreUI::IAppUsageContextManager* contextManager = dynamic_cast<coreUI::IAppUsageContextManager*>(window);
    contextManager->addContext(context);
    contextManager->addWidgetToContext(context, button);
    coreUI::IContextRootPtr tabContex = utils::dynamic_pointer_cast<coreUI::IContextRoot>(tab);
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
    connect(mw, SIGNAL(activateTab(coreUI::IMdeTabPtr)), this, SLOT(activateTab(coreUI::IMdeTabPtr)));
    connect(mw, SIGNAL(tabAdded(coreUI::IMdeTabPtr)), this, SLOT(addTab(coreUI::IMdeTabPtr)));
}

void MdeMainWindowController::buttonClicked()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    UTILS_ASSERT(button);
    coreUI::IMdeTabPtr tab = button2TabWindow[button];
    activateTab(tab);
    if (window->isContextWidget(tab->getMainWidget())) {
        window->setCurrentContext(tab->getMainWidget());
    } else {
        window->setCurrentContext(button);
    }
}

void MdeMainWindowController::activateTab( coreUI::IMdeTabPtr tab )
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        coreUI::IMdeTabPtr t = it->second;
        t->setActive(t == tab);
        t->getMainWidget()->setVisible(false);
        it->first->setChecked( t == tab);
    }

    tab->getMainWidget()->setVisible(true);
}

void MdeMainWindow::closeEvent(QCloseEvent* event)
{
	bool close = true;

	auto visManager = plugin::getVisualizerManager();

	core::IVisualizerManager::VisualizerInstances visInstances;

	visManager->getVisualizersInstances(visInstances);

	for(auto it = visInstances.begin(); it != visInstances.end(); ++it){

		auto w = (*it)->getWidget();
		if(w != nullptr && w->isWindowModified() == true){
			close = false;
			break;
		}
	}

	if(close == false){
		if(QMessageBox::question( this, tr("Confirm exit"), tr("There are some unsaved data changes. Do You want to close application and discard them?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::No){

				event->ignore();
		}else{

			onClose();
			event->accept();

		}
	}else{
		coreUI::CoreMainWindow::closeEvent(event);
	}
}
