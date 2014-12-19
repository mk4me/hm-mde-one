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
#include <plugins/hmdbCommunication/OperationProgressWidget.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/DataSourceDefaultContent.h>
#include <corelib/PluginCommon.h>
#include <corelib/IPath.h>
#include <plugins/hmdbCommunication/GeneralSourceViewWidget.h>
#include <plugins/hmdbCommunication/DataViewConfigurationWidget.h>

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

class IMUHMDBSourceView : public hmdbCommunication::IHMDBSourceViewManager::IHMDBSourceContextView
{
public:
	//! \return Nazwa widoku
	virtual const QString name() const { return QObject::tr("IMU view"); }
	//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych jakim zasilamy widok
	//! \return Widok obs³uguj¹cy kontekst
	virtual QWidget * createView(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext, hmdbCommunication::IHMDBSourceViewManager * viewManager) {

		auto ret = new GeneralSourceViewWidget(shallowCopyContext);

		hmdbCommunication::IDataSourcePerspective * perspective = nullptr;
		hmdbCommunication::IDataSourceContent * content = nullptr;

		if (viewManager->perspectivesCount(name()) > 0){
			perspective = viewManager->perspective(0, name());
		}
		else if(viewManager->perspectivesCount() > 0){
			perspective = viewManager->perspective(0);
		}

		ret->dataView()->setPerspective(perspective);

		if (viewManager->contentsCount(name()) > 0){
			content = viewManager->content(0, name());
		}
		else if (viewManager->contentsCount() > 0){
			content = viewManager->content(0);
		}

		ret->dataView()->setContent(content);

		ret->dataViewConfiguration()->setVisible(false);

		return ret;
	}
	//! \return Czy dany widok wymaga po³¹czenia z us³ugami webowymi
	virtual const bool requiresRemoteContext() const { return true; }
};

class IMUPerspective : public hmdbCommunication::IDataSourcePerspective
{
public:
	IMUPerspective() {}
	virtual ~IMUPerspective() {}

	virtual const QString name() const{ return QObject::tr("Subject"); }

	virtual void rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const hmdbCommunication::ShallowCopy & shallowCopy)
	{
		QFont sessionFont;
		sessionFont.setBold(true);
		QFont motionFont;
		motionFont.setItalic(true);

		auto subjectsITEnd = shallowCopy.motionShallowCopy.performers.end();
		for (auto subjectIT = shallowCopy.motionShallowCopy.performers.begin(); subjectIT != subjectsITEnd; ++subjectIT){
			//jeœli pusty pacjent to go pomijamy
			if (subjectIT->second->performerConfs.empty() == true){
				continue;
			}

			//generuje item pacjenta
			QTreeWidgetItem * item = nullptr;
			if (subjectIT->second->patient != nullptr){
				item = new hmdbCommunication::TreeWidgetContentItem(hmdbCommunication::PatientContent, subjectIT->second->patient->patientID);
			}
			else{
				item = new hmdbCommunication::TreeWidgetContentItem(hmdbCommunication::SubjectContent, subjectIT->second->performerID);
			}

			treeWidgetItem->addChild(item);

			auto perfConfsITEnd = subjectIT->second->performerConfs.end();
			for (auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != perfConfsITEnd; ++perfConfIT){
				if (perfConfIT->second->session->trials.empty() == true){
					continue;
				}

				//generuje item sesji
				auto sessionItem = new hmdbCommunication::TreeWidgetContentItem(hmdbCommunication::SessionContent, perfConfIT->second->session->sessionID);
				sessionItem->setFont(0, sessionFont);

				item->addChild(sessionItem);

				auto motionsITEnd = perfConfIT->second->session->trials.end();
				for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
					if (motionIT->second->files.empty() == true){
						continue;
					}

					//generuje item motiona
					auto motionItem = new hmdbCommunication::TreeWidgetContentItem(hmdbCommunication::MotionContent, motionIT->second->trialID);
					motionItem->setFont(0, motionFont);
					sessionItem->addChild(motionItem);
				}
			}
		}
	}
};


void MdeMainWindow::customViewInit(QWidget * console)
{
	auto name2icon = [](const std::string& name, bool source) -> QIcon {
		if (name == "Scripting")  return QIcon(":/mde/icons/skrypty.png");
		if (name == "Data Flow Service")  return QIcon(":/mde/icons/dataFlow.png");
		if (name == "IMUCostume")  return QIcon(":/mde/icons/imu.png");
		if (name == "HMDB Source") return QIcon(":/mde/icons/communication.png");
		return source ? QIcon(":/mde/icons/Badania.png") : QIcon(":/mde/icons/Operacje.png");
	};
	auto name2name = [](const std::string& name) -> std::string {
		if (name == "Data Flow Service")  return "Data flow";
		if (name == "IMUCostume")  return "Suit";
		if (name == "HMDB Source") return "Data";
		return name;
	};
   auto memoryManager = plugin::getHierarchyManagerReader();
   memoryManager->addObserver(analysisModel);
   //trySetStyleByName("hmm");
 
   this->showFullScreen();
   this->setFixedSize(this->width(), this->height());

   auto sourceManager = plugin::getSourceManager();

   auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sourceManager);

   if (hmdbSource != nullptr){
	   auto vm = hmdbSource->viewManager();
	   auto hmdbView = new IMUHMDBSourceView;

	   vm->registerViewPrototype(hmdbView);

	   {
		   hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration ccfg;
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

		   vm->registerConfiguration(ccfg, hmdbView->name());
	   }

	   vm->registerPerspective(new IMUPerspective, hmdbView->name());
	   vm->registerContent(new hmdbCommunication::DataSourceDefaultContent, hmdbView->name());
   }
   
   for (int i = 0; i < sourceManager->getNumSources(); ++i) {
       auto source = sourceManager->getSource(i);
       QWidget* widget = source->getWidget();
       if (widget) {
		   auto name = source->name();
           addTab(coreUI::IMdeTabPtr(new SimpleTab(widget, name2icon(name, true),tr(name2name(name).c_str()))));
       }
   }
   
   AnalisisWidget* aw = new AnalisisWidget(analysisModel, contextEventFilter, nullptr);
   // -----------------------
   /*QIcon analysisIcon;
   analysisIcon.addPixmap(QPixmap(":/mde/icons/Analizy.png"),QIcon::Active);
   analysisIcon.addPixmap(QPixmap(":/mde/icons/AnalizyH.png"),QIcon::Disabled);
   auto analysisTab = utils::make_shared<AnalysisTab>(aw, analysisIcon, tr("Analysis"));
   analysisTab->setEnabled(false);
   addTab(analysisTab);*/

   addTab(coreUI::IMdeTabPtr(new AnalysisTab(aw, QIcon(":/mde/icons/Analizy.png"), tr("Analysis"))));
   /*QIcon reportsIcon;
   reportsIcon.addPixmap(QPixmap(":/mde/icons/Raporty.png"),QIcon::Active);
   reportsIcon.addPixmap(QPixmap(":/mde/icons/RaportyH.png"),QIcon::Disabled);
   auto reportsTab = utils::make_shared<coreUI::ReportsTab>(reportsIcon, tr("Reports"));
   connect(aw, SIGNAL(reportCreated(const QString& )), reportsTab.get(), SLOT(enable()));
   reportsTab->setEnabled(false);
   addTab(reportsTab);
   addTab(coreUI::IMdeTabPtr(new SimpleTab(console, QIcon(":/mde/icons/Operacje.png"),tr("Console"))));
*/
   // TODO : najlepiej byloby przeniesc to do kontrolera
   auto serviceManager = plugin::getServiceManager();
   auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
   for (int i = 0; i < serviceManager->getNumServices(); ++i) {
       plugin::IServicePtr service = serviceManager->getService(i);
       
       if (service != timeline) {
            auto w = createServiceWidget(service);
            if (w) {
				auto name = service->name();
                addTab(coreUI::IMdeTabPtr(new SimpleTab(w, name2icon(name, false),tr(name2name(name).c_str()))));
                addPropertiesToServiceWindow(service, w);
            }
       }

        core::IFilterProviderPtr filterProvider = utils::dynamic_pointer_cast<core::IFilterProvider>(service);
        if (filterProvider) {
            analysisModel->addFilterBundles(filterProvider->getFilterBundles());
        }
   }

   coreUI::IMdeTabPtr reportsTab  = coreUI::IMdeTabPtr(new coreUI::ReportsTab(QIcon(":/mde/icons/Raporty.png"), tr("Reports")));
   addTab(reportsTab);
   addTab(coreUI::IMdeTabPtr(new SimpleTab(console, QIcon(":/mde/icons/log.png"),tr("Log"))));

   bool cc = connect(analysisModel.get(), SIGNAL(reportCreated(const QString&)), reportsTab->getMainWidget(), SLOT(setHtml(const QString&)));
	emit modelChanged();
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
    button->setEnabled(tab->isEnabled());
    QObject* obj = dynamic_cast<QObject*>(tab.get());
    if (obj) {
        connect(obj, SIGNAL(tabEnabled(bool)), this, SLOT(update()));
        connect(obj, SIGNAL(tabActivated(bool)), this, SLOT(activateTab()));
    } else {
        UTILS_ASSERT(false, "IMdeTab powinien byc zaimplementowany przez jakis QObject, bo implementacja zaklada uzycie sygnalow");
    }
   
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
    connect(mw, SIGNAL(modelChanged()), this, SLOT(update()));
    connect(mw, SIGNAL(activateTab(coreUI::IMdeTabPtr)), this, SLOT(activateTab(coreUI::IMdeTabPtr)));
    connect(mw, SIGNAL(tabAdded(coreUI::IMdeTabPtr)), this, SLOT(addTab(coreUI::IMdeTabPtr)));
}

void MdeMainWindowController::buttonClicked()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    UTILS_ASSERT(button);
    coreUI::IMdeTabPtr tab = button2TabWindow[button];
    activateTab(tab);
    handleContext(tab, button);

}

void MdeMainWindowController::activateTab( coreUI::IMdeTabPtr tab )
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        coreUI::IMdeTabPtr t = it->second;
        QObject* obj = dynamic_cast<QObject*>(t.get());
        if (obj) {
            obj->blockSignals(true);
            t->setActive(t == tab);
            obj->blockSignals(false);
            t->getMainWidget()->setVisible(false);
            it->first->setChecked( t == tab);
        } else {
            UTILS_ASSERT(false, "obecna implementacja wymaga aby realizacja IMdeTab byla QObject");
        }
    }

    tab->getMainWidget()->setVisible(true);
}

void MdeMainWindowController::activateTab()
{
    QObject* obj = sender();
    coreUI::IMdeTab* tab = dynamic_cast<coreUI::IMdeTab*>(obj);
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        if (it->second.get() == tab) {
            activateTab(it->second);
            handleContext(it->second, it->first);
            return;
        }
    }
}

void MdeMainWindowController::update()
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        coreUI::IMdeTabPtr t = it->second;
        it->first->setEnabled( t->isEnabled());
    }
}

void MdeMainWindowController::handleContext( coreUI::IMdeTabPtr tab, QToolButton* button )
{
    if (window->isContextWidget(tab->getMainWidget())) {
        window->setCurrentContext(tab->getMainWidget());
    } else {
        window->setCurrentContext(button);
    }
}


void MdeMainWindow::onAbout()
{
	std::unique_ptr<AboutDialog> dialog(new AboutDialog);
	dialog->exec();
}
