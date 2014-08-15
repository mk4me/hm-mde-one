#include "MdePCH.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QMainWindow>
#include <utils/Debug.h>
#include "MdeMainWindow.h"
#include <coreui/CoreTextEditWidget.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeView>
#include "AboutDialog.h"
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreDockWidget.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <coreui/IMdeTab.h>
#include <coreui/SimpleTab.h>
#include <coreui/ReportsTab.h>
#include <coreui/IMdeTab.h>
#include "AnalisisWidget.h"
#include "AnalysisTab.h"
#include "ui_toolboxmaindeffile.h"
#include "MdeServiceWindow.h"
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <corelib/PluginCommon.h>
#include <corelib/IPath.h>

using namespace core;

MdeMainWindow::MdeMainWindow(const CloseUpOperations & closeUpOperations, const std::string & appName)
	: coreUI::CoreMainWindow(closeUpOperations), coreUI::SingleInstanceWindow(appName),
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

class IMUHMDBSourceView : public hmdbCommunication::IHMDBSource::IHMDBSourceContextView
{
public:
	//! \return Nazwa widoku
	virtual const QString name() const { return QObject::tr("IMU view"); }
	//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych jakim zasilamy widok
	//! \return Widok obs³uguj¹cy kontekst
	virtual QWidget * createView(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext) { return new DataViewWidget(shallowCopyContext); }
	//! \return Czy dany widok wymaga po³¹czenia z us³ugami webowymi
	virtual const bool requiresRemoteContext() const { return true; }
};

void MdeMainWindow::customViewInit(QWidget * console)
{
   auto memoryManager = plugin::getHierarchyManagerReader();
   memoryManager->addObserver(analysisModel);
   trySetStyleByName("hmm");
 
   this->showFullScreen();

   auto sourceManager = plugin::getSourceManager();

   auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sourceManager);

   if (hmdbSource != nullptr){

	   auto hmdbView = new IMUHMDBSourceView;

	   hmdbSource->registerSourceContextViewPrototype(hmdbView);

	   {
		   hmdbCommunication::IHMDBSource::ContextConfiguration ccfg;
		   ccfg.name = tr("Default PJWSTK IMU data connection");
		   ccfg.storageConfiguration.path = QString::fromStdString((plugin::getPaths()->getUserApplicationDataPath() / "db" / "localStorage.db").string());
		   ccfg.storageConfiguration.password = "P,j.W/s<T>k2:0\"1;2";
		   ccfg.motionServicesConfiguration.userConfiguration.user = "";
		   ccfg.motionServicesConfiguration.userConfiguration.password = "";
		   ccfg.motionServicesConfiguration.serviceConfiguration.url = "https://v21.pjwstk.edu.pl/IMU";
		   ccfg.motionServicesConfiguration.serviceConfiguration.caPath = QString::fromStdString((plugin::getPaths()->getResourcesPath() / "v21.pjwstk.edu.pl.crt").string());

		   ccfg.motionDataConfiguration.serviceConfiguration.url = "ftps://v21.pjwstk.edu.pl";
		   ccfg.motionDataConfiguration.serviceConfiguration.caPath = "";
		   ccfg.motionDataConfiguration.userConfiguration.user = "testUser";
		   ccfg.motionDataConfiguration.userConfiguration.password = "testUser";

		   hmdbSource->registerSourceContextViewConfiguration(hmdbView, ccfg);
	   }
   }
   
   for (int i = 0; i < sourceManager->getNumSources(); ++i) {
       auto source = sourceManager->getSource(i);
       QWidget* widget = source->getWidget();
       if (widget) {
           addTab(coreUI::IMdeTabPtr(new SimpleTab(widget, QIcon(":/mde/icons/Badania.png"),tr(source->name().c_str()))));
       }
   }
   
   AnalisisWidget* aw = new AnalisisWidget(analysisModel, contextEventFilter, nullptr);
   // -----------------------

   addTab(coreUI::IMdeTabPtr(new AnalysisTab(aw, QIcon(":/mde/icons/Analizy.png"), tr("Analysis"))));
   coreUI::IMdeTabPtr reportsTab  = coreUI::IMdeTabPtr(new coreUI::ReportsTab(QIcon(":/mde/icons/Raporty.png"), tr("Reports")));
   addTab(reportsTab);
   addTab(coreUI::IMdeTabPtr(new SimpleTab(console, QIcon(":/mde/icons/Operacje.png"),tr("Console"))));

   // TODO : najlepiej byloby przeniesc to do kontrolera
   bool cc = connect(analysisModel.get(), SIGNAL(reportCreated(const QString&)), reportsTab->getMainWidget(), SLOT(setHtml(const QString&)));
   auto serviceManager = plugin::getServiceManager();
   auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
   for (int i = 0; i < serviceManager->getNumServices(); ++i) {
       plugin::IServicePtr service = serviceManager->getService(i);
       
       if (service != timeline) {
            auto w = createServiceWidget(service);
            if (w) {
                addTab(coreUI::IMdeTabPtr(new SimpleTab(w, QIcon(":/mde/icons/Operacje.png"),tr(service->name().c_str()))));
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
        QString serviceName = QString::fromStdString(service->name());
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
        QString serviceName = QString::fromStdString(service->name());
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


