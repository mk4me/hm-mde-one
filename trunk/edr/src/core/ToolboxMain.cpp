#include "CorePCH.h"
// Go to http://dmoulding.googlepages.com/vld 
// for the Visual Leak Detector for VisualStudio
    
#include "ToolboxMain.h"
#include "ui_toolboxmaindeffile.h"
#include <core/QOsgWidgets.h>
#include "SceneGraphWidget.h"

#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>

#include <osgViewer/GraphicsWindow>
#include <osgText/Text>
#include <osg/ShapeDrawable>
#include <osgViewer/Scene>
#include <iostream>
#include <osgGA/TerrainManipulator>

#include <QtCore/QDir>

#include "C3DModel.h"
#include "ConsoleWidget.h"
#include "Logger.h"

#include "ServiceManager.h"
#include <core/IAnimationService.h>
#include <core/C3DParserEx.h>
#include "UserInterfaceService.h"
#include "RenderService.h"
#include "config/ConfigurationFileService.h"

#include <iostream>

#include "Model.h"
#include "Mesh.h"

#include "FileReader2Motion.h"
#include <utils/Debug.h>

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "ComputeThread.h"

#include <core/Log.h>
#include <core/StringTools.h>
#include <boost/foreach.hpp>

DEFINE_DEFAULT_LOGGER("edr.core");

/**
 *	U¿ytkowa klasa, u¿ywana tam, gdzie mêcz¹ca jest zmiana wartoœci jakiejœ zmiennej
 *  a nastêpnie rêczne jej przywracanie; jest to uci¹¿liwe zw³aszcza gdy funkcja ma
 *  wiele punktów wyjœcia.
 */
template <class T>
class Push
{
private:
    //! Poprzednia wartoœæ.
    T oldValue;
    //! Zmienna.
    T& variable;
public:
    //! \param variable
    //! \param newValue
    Push(T& variable, const T& newValue) : oldValue(variable), variable(variable)
    {
        variable = newValue;
    }
    //! 
    ~Push()
    {
        variable = oldValue;
    }
};

ToolboxMain::ToolboxMain(QWidget *parent)
:   QMainWindow(parent),
    ui(new Ui::ToolboxMain),
    removeOnClick(false),
    updateEnabled(true)
{
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    pluginLoader = new core::PluginLoader();


//     LoggerPtr root = Logger::getRootLogger();
//     static const LogString TTCC_CONVERSION_PATTERN(LOG4CXX_STR("%r [%t] %p %c %x - %m%n"));
//     //LayoutPtr layout(new PatternLayout(TTCC_CONVERSION_PATTERN));
//     AppenderPtr appender(new OutputDebugStringAppender(/*layout*/));
//     appender->setLayout(new PatternLayout(LOG4CXX_STR("%F(%L): %m\n")));
//     root->addAppender(appender);



    ui->setupUi(this);

    registerCoreServices();

	pluginLoader->load();
    registerPluginsServices();
	registerPluginsParsers();


    sceneRoot = new osg::Group();
    sceneRoot->setName("root");

	dataManager->findResources();
    dataManager->findLocalTrials();
    dataManager->loadResources();

    // inicjalizacja us³ug
    for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
        m_pServiceManager->getService(i)->init(m_pServiceManager, dataManager, sceneRoot.get(), this);
    }


    initializeConsole();          // Console Widget 
    InitializeControlWidget();          // Control Widget + TimeLine
    LoadConfiguration();                // Wczytuje plik konfiguracyjny

    initializeUI();


    readSettings(QSettings(), true);


    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateServices()));
    updateTimer.start(20);

    computeThread = new ComputeThread(m_pServiceManager, 0.02);
    computeThread->start();


    if ( getNumViews() ) {
        connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
        _timer.start( 20 );
    }
}

ToolboxMain::~ToolboxMain()
{
    clear();

}

void ToolboxMain::clear()
{
    delete computeThread;

    delete ui;  // ui stuff
    //std::streambuf *buf = std::cout.rdbuf(_streambuf);
    //delete buf;

    //m_pRenderService.reset();
    delete m_pServiceManager;
    delete pluginLoader;
    m_pUserInterfaceService = NULL;
	if(dataManager)
		delete dataManager;
	dataManager = NULL;
    

    // remove all services
    //m_pServiceManager = NULL;

    
}

void ToolboxMain::LoadConfiguration()
{
    /*
    QString path = qApp->applicationDirPath();
    path.append("/"); 
    path.append(configName); 
    ConfigurationService *config = new ConfigurationService(); 
    config->loadConfiguration( std::string(path.toUtf8()) ); /**/
}

void ToolboxMain::readSettings( const QSettings& settings, bool readGeometry )
{
    if ( settings.contains("WindowState") ) {
        restoreState(settings.value("WindowState").toByteArray());
    }
    if ( readGeometry && settings.contains("Geometry") ) {
        restoreGeometry(settings.value("Geometry").toByteArray());
    }
}

void ToolboxMain::WriteSettings()
{
    QSettings settings;
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());
}

void ToolboxMain::closeEvent(QCloseEvent* event)
{
    WriteSettings();
    QMainWindow::closeEvent(event);
}

void ToolboxMain::InitializeControlWidget()
{
    InitializeOGSWidget();  // MainWidget 

//     osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene(); 
//     osg::Node* sceneRoot = scene->getSceneData();

    // inicjalizacja GridWidget
    QDockWidget *gDock = new QDockWidget(tr("Service scene graph"), this, Qt::WindowTitleHint);
    gDock->setObjectName(QString("GridWidget"));
    gDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    sceneGraphWidget = new SceneGraphWidget();    
    addDockWidget(Qt::LeftDockWidgetArea, gDock);
    gDock->setWidget((QWidget*)sceneGraphWidget);
    sceneGraphWidget->addServices(m_pServiceManager);
}

void ToolboxMain::populateWindowMenu(QMenu* target)
{
    QList<QDockWidget *> dockwidgets = qFindChildren<QDockWidget*>(this);
    if (dockwidgets.size()) {
        for (int i = 0; i < dockwidgets.size(); ++i) {
            QDockWidget *dockWidget = dockwidgets.at(i);
            if (dockWidget->parentWidget() == this) {
                target->addAction(dockWidget->toggleViewAction());
            }
        }
        target->addSeparator();
    }

    QList<QToolBar *> toolbars = qFindChildren<QToolBar *>(this);
    if (toolbars.size()) {
        for (int i = 0; i < toolbars.size(); ++i) {
            QToolBar *toolBar = toolbars.at(i);
            target->addAction(toolBar->toggleViewAction());
        }
    }
}

void ToolboxMain::initializeConsole()
{
   

    QDockWidget *dock; 
    dock = new QDockWidget(tr("Console"), this, Qt::WindowTitleHint);
    dock->setObjectName("Console");
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);
    _consoleWidget = new ConsoleWidget();    
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    dock->setWidget((QWidget*)_consoleWidget); 
    // Inicjalizacja konsoli
//     ConsoleWidgetOutputBuffer *ob = new ConsoleWidgetOutputBuffer(_consoleWidget, 256);
//     _streambuf = std::cout.rdbuf(ob);
}

void ToolboxMain::InitializeOGSWidget()
{
//  setCentralWidget(_osgView); 
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->setName("root");
    root->addChild(createGrid());

    //m_pRenderService->SetScene(root);
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Node> ToolboxMain::createGrid()
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    // create Geometry object to store all the vertices and lines primitive.
    osg::ref_ptr<osg::Geometry>	linesGeom = new osg::Geometry();
    float size = 0.5f; 
    int sizeX = 21; 
    int sizeY = 21; 
    // this time we'll preallocate the vertex array to the size we
    // need and then simple set them as array elements, 8 points
    // makes 4 line segments.
    osg::Vec3Array* vertices = new osg::Vec3Array((sizeX+sizeY)*2);
    for (int i=0; i<sizeX; ++i)
    {
        float a_x = (float(i - sizeX/2) )*size; 
        float a_y = -1.0f*(sizeX/2)*size; 
        float b_x = (float(i - sizeX/2) )*size; 
        float b_y = 1.0f*(sizeX/2)*size; 
        (*vertices)[2*i].set( a_x, a_y, 0.0f);
        (*vertices)[2*i+1].set( b_x, b_y, 0.0f);
    }

    for (int i=0; i<sizeY; ++i)
    {
        float a_x = -1.0f*(sizeY/2)*size; 
        float a_y = (float(i - sizeY/2) )*size; 
        float b_x = 1.0f*(sizeY/2)*size; 
        float b_y = (float(i - sizeY/2) )*size; 
        (*vertices)[sizeX*2+2*i].set( a_x, a_y, 0.0f);
        (*vertices)[sizeX*2+2*i+1].set( b_x, b_y, 0.0f);
    }
    // pass the created vertex array to the points geometry object.
    linesGeom->setVertexArray(vertices);
    // set the colors as before, plus using the above
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(0.5f, 0.5f, 0.5f, 0.5f) );
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding( osg::Geometry::BIND_OVERALL );
    // set the normal in the same way color.
    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
    linesGeom->setNormalArray(normals);
    linesGeom->setNormalBinding( osg::Geometry::BIND_OVERALL );
    // This time we simply use primitive, and hardwire the number of coords to use 
    // since we know up front,
    linesGeom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, (sizeX+sizeY)*2) );
    // add the points geometry to the geode.
    geode->addDrawable(linesGeom);
    //osgView_->setSceneData((osg::Node *)geode);
    geode->setName("grid");
    return geode; 
}

void ToolboxMain::updateServices()
{
	if(dataManager->isLoadLocalTrialData())
	{
		loadData();
	}
    _consoleWidget->flushQueue();
    if ( updateEnabled ) {
        m_pServiceManager->updatePass();
    }
    _consoleWidget->flushQueue();
}

void ToolboxMain::registerCoreServices()
{
    //1. Service manafger
    m_pUserInterfaceService = new UserInterfaceService();
    m_pServiceManager = new ServiceManager();
   // m_pRenderService = RenderServicePtr(new RenderService());

	/*
	tworzy managera zasobow. w konstruktorze szuka sciezek do zasobow stalych (shadery i tbs)
	*/
    dataManager = new DataManager();

    //3. UserInterface Service
    m_pServiceManager->registerService(IServicePtr(m_pUserInterfaceService));
    m_pUserInterfaceService->setMainWindow(this);

    //4. Render Service
   // m_pServiceManager->registerService(IServicePtr(m_pRenderService));
}

void ToolboxMain::registerPluginsServices()
{
    for ( size_t i = 0; i < pluginLoader->getNumPlugins(); ++i ) {
        core::PluginPtr plugin = pluginLoader->getPlugin(i);
        for ( size_t j = 0; j < plugin->getNumServices(); ++j ) {
            m_pServiceManager->registerService(plugin->getService(j));
        }
    }
}

void ToolboxMain::registerPluginsParsers()
{
    for (size_t i = 0; i < pluginLoader->getNumPlugins(); ++i)
    {
        core::PluginPtr plugin = pluginLoader->getPlugin(i);
        int z = static_cast<int>(plugin->getNumParsers());
        for(size_t j = 0; j < z; ++j)
        {
            dataManager->registerParser(plugin->getParser(j));
        }
    }
    core::IParserPtr c3dParser = core::shared_ptr<C3DParser>(new C3DParser());
    dataManager->registerParser(c3dParser);
}

void ToolboxMain::onOpen()
{
    Push<bool> pushed(updateEnabled, false);
    const QString fileName = QFileDialog::getExistingDirectory(this, 0, QDir::currentPath().append("/data/trials"));
    if (!fileName.isEmpty()) 
    {
        std::string pathVal = core::toStdString(fileName);
        const std::string& path = pathVal;


        openFile(path);

        return;
    }
}

void ToolboxMain::onExit()
{
    close();
}

void ToolboxMain::onMaterial()
{
    
}

void ToolboxMain::onBones()
{

}

void ToolboxMain::onWireframe()
{

}


QDockWidget* ToolboxMain::embeddWidget( QWidget* widget, const QString& name, const QString& style, const QString& sufix, Qt::DockWidgetArea area /*= Qt::AllDockWidgetAreas*/ )
{
    // dodajemy widget dokowalny
    QDockWidget* dock = new QDockWidget( name, this, Qt::WindowTitleHint);        
    dock->setAllowedAreas(area);
    dock->setObjectName(name + widget->objectName() + "WIDGET" + sufix);
    dock->setStyleSheet(style);
    dock->setWidget(widget);
    QObject::connect( dock, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );
    return dock;
}

void ToolboxMain::initializeUI()
{
	//ladowanie styli qt
	QString style;
	if(dataManager->getApplicationSkinsFilePathCount() > 0)
	{
		//style qt
		QFile file(QString::fromAscii(dataManager->getApplicationSkinsFilePath(0).c_str(), dataManager->getApplicationSkinsFilePath(0).size()));
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			style = file.readAll();
			file.close();
		}
	}
	setStyleSheet(style);

	// widget rendeer service - centralny
    // setCentralWidget(reinterpret_cast<QWidget*>(m_pRenderService->getWidget()));
    //centralWidget()->widget

    setDockOptions( AllowNestedDocks | AllowTabbedDocks );
    setDocumentMode(true);
    //setCentralWidget(NULL);

    // pozosta³e widgety "p³ywaj¹ce"
    for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
        IServicePtr service = m_pServiceManager->getService(i);

        // HACK
        QWidget* viewWidget = reinterpret_cast<QWidget*>(service->getWidget());
        QWidget* controlWidget = reinterpret_cast<QWidget*>(service->getControlWidget());
        QWidget* settingsWidget = reinterpret_cast<QWidget*>(service->getSettingsWidget());

        if ( viewWidget/* && service != m_pRenderService*/ ) {

//            this->ui->tabWidget->addTab(viewWidget, string_cast(service->getName()));

//              centralWidget()->layout()->addWidget(viewWidget);
//             addDockWidget(Qt::RightDockWidgetArea, embeddWidget(
//                 viewWidget, 
//                 core::toQString(service->getName()), 
//                 style,
//                 Qt::RightDockWidgetArea));
		}
        if ( controlWidget ) {
            addDockWidget(Qt::BottomDockWidgetArea, embeddWidget(
                controlWidget, 
                core::toQString(service->getName()), 
                style,
                "Control",
                Qt::BottomDockWidgetArea));
        }
        if ( settingsWidget ) {
            addDockWidget(Qt::LeftDockWidgetArea, embeddWidget(
                settingsWidget, 
                core::toQString(service->getName()), 
                style,
                "Settings",
                Qt::LeftDockWidgetArea));
        }
	}

    // uzupe³nienie podmenu z mo¿liwymi oknami
    populateWindowMenu(ui->menuWindow);

    // testowe opcje
    onTestItemClickedPtr.reset( new core::Window::ItemPressed(boost::bind(&ToolboxMain::onTestItemClicked, this, _1, _2 )));
    onTestRemoveToggledPtr.reset(new core::Window::ItemPressed(boost::bind(&ToolboxMain::onTestRemoveToggled, this, _1, _2 )));
    m_pUserInterfaceService->getMainWindow()->addMenuItem("Callback test/Option", onTestItemClickedPtr);
    m_pUserInterfaceService->getMainWindow()->addMenuItem("Callback test/Nested/Option", onTestItemClickedPtr);
    m_pUserInterfaceService->getMainWindow()->addMenuItem("Callback test/Nested/Option2", onTestItemClickedPtr);
    m_pUserInterfaceService->getMainWindow()->addMenuItem("Callback test/Nested2/Option", onTestItemClickedPtr);
    m_pUserInterfaceService->getMainWindow()->addMenuItem("Callback test/Remove on click?", onTestRemoveToggledPtr, true, removeOnClick);

    ui->actionTabbedView->setChecked(true);
}

void ToolboxMain::onCustomAction()
{
    QObject* obj = QObject::sender();
    std::string path = core::toStdString(obj->objectName());
    this->triggerMenuItem(path, false);
}

void ToolboxMain::onCustomAction( bool triggered )
{
    QObject* obj = QObject::sender();
    std::string path = core::toStdString(obj->objectName());
    this->triggerMenuItem(path, triggered);
}

void ToolboxMain::onRemoveMenuItem( const std::string& path )
{
    // TODO: rekurencyjne usuwanie niepotrzebnych podmenu
    QAction* action = findChild<QAction*>(core::toQString(path));
    if ( action ) 
    {
        delete action;
    }
    
    /*
    // "prawie dziala", ale przerwaca siê na zagnie¿d¿onych menu
    if ( object ) 
    {
        QObject* last;
        do 
        {
            QObject* parent = object->parent();
            delete object;
            object = parent;
        } 
        while ( object && object->children().size() <= 2 );
    }*/
}

void ToolboxMain::onTestItemClicked(const std::string& sender, bool state)
{
    if (removeOnClick) {
        m_pUserInterfaceService->getMainWindow()->removeMenuItem(sender);
    }
}

void ToolboxMain::onTestRemoveToggled(const std::string& sender, bool state )
{
    removeOnClick = state;
}

void ToolboxMain::onAddMenuItem( const std::string& path, bool checkable, bool initialState )
{
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(path, boost::char_separator<char>("/"));
    tokenizer::iterator next;
    tokenizer::iterator token = tokens.begin();

    QWidget* currentMenu = menuBar();
    std::string pathPart;

    for (token = tokens.begin(); token != tokens.end(); token = next ) 
    {        
        // ustawiamy nastêpny (przyda siê)
        next = token;
        ++next;

        // aktualizacja bie¿¹cej œcie¿ki
        if ( !pathPart.empty() ) 
        {
            pathPart.append("/");
        }
        pathPart += *token;

        // wyszukanie dziecka
        QString itemName = core::toQString(pathPart);

        if ( next == tokens.end() ) 
        {
            // liœæ
            QAction* action = new QAction(this);
            action->setObjectName( core::toQString(pathPart) );
            action->setText(QApplication::translate("ToolboxMain", token->c_str(), 0, QApplication::UnicodeUTF8));
            currentMenu->addAction(action);
            if ( checkable ) 
            {
                action->setCheckable(true);
                action->setChecked(initialState);
                QObject::connect(action, SIGNAL(toggled(bool)), this , SLOT(onCustomAction(bool)));
            } 
            else 
            {
                QObject::connect(action, SIGNAL(triggered()), this , SLOT(onCustomAction()));
            }
        } 
        else 
        {
            if ( QMenu* menu = currentMenu->findChild<QMenu*>(itemName) )
            {
                // menu ju¿ istnieje
                currentMenu = menu;
            }
            else 
            {
                // ga³¹Ÿ
                menu = new QMenu(currentMenu);
                menu->setObjectName( itemName );
                menu->setTitle(QApplication::translate("ToolboxMain", token->c_str(), 0, QApplication::UnicodeUTF8));
                currentMenu->addAction( menu->menuAction() );
                currentMenu = menu;
            }
        }
    }
}

void ToolboxMain::openFile( const std::string& path )
{
    LOG_INFO("Opening file: " << path);
	dataManager->loadLocalTrial(path);
}

void ToolboxMain::loadData()
{
	//m_pRenderService->GetFactory()->Clear();
	m_pServiceManager->loadDataPass(dataManager);

	// m_pRenderService->AddObjectToRender(createGrid());

	// manage scene
// 	osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene(); 
// 	osg::Node* sceneRoot = scene->getSceneData();
	dataManager->setLoadLocalTrialData(false);
}

void ToolboxMain::onTabbedViewSelected(bool toggled)
{
    if ( toggled ) {
        ui->actionDockableView->setChecked(false);
        ui->actionDockableView->setEnabled(true);
        ui->actionTabbedView->setEnabled(false);
        reorganizeWidgets(WidgetsOrganizationTabbed);
    }
}

void ToolboxMain::onDockableViewSelected(bool toggled)
{
    if ( toggled ) {
        ui->actionTabbedView->setChecked(false);
        ui->actionTabbedView->setEnabled(true);
        ui->actionDockableView->setEnabled(false);
        reorganizeWidgets(WidgetsOrganizationDocked);
    }
}

void ToolboxMain::reorganizeWidgets( WidgetsOrganization organization )
{
    // usuwamy stare œmieci
    for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
        IServicePtr service = m_pServiceManager->getService(i);
        if ( QWidget* viewWidget = reinterpret_cast<QWidget*>(service->getWidget()) ) {
            // co musimy usun¹æ?
            if ( QDockWidget* dock = qobject_cast<QDockWidget*>(viewWidget->parent()) ) {
                viewWidget->setParent(NULL);
                removeDockWidget(dock);
                delete dock;
            }
        }   
    }

    // za³adowanie styli
    QString style;
    if(dataManager->getApplicationSkinsFilePathCount() > 0) {
        QFile file(core::toQString(dataManager->getApplicationSkinsFilePath(0)));
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            style = file.readAll();
            file.close();
        }
    }
    setStyleSheet(style);

    setDockOptions( AllowNestedDocks | AllowTabbedDocks );
    setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
    setCentralWidget(NULL);

    // tworzymy listê dokowalnych widgetów
    std::vector< QDockWidget* > widgets;
    for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
        IServicePtr service = m_pServiceManager->getService(i);
        if ( QWidget* viewWidget = reinterpret_cast<QWidget*>(service->getWidget()) ) {
            widgets.push_back(embeddWidget(
                viewWidget, 
                core::toQString(service->getName()), 
                style,
                "",
                Qt::RightDockWidgetArea)
            );
        }
    }
    
    if ( organization == WidgetsOrganizationDocked ) {
        // dodajemy pierwszy...
        std::vector< QDockWidget* >::iterator iter = widgets.begin();
        if ( iter != widgets.end() ) {
            addDockWidget( Qt::RightDockWidgetArea, *iter );
            // kolejne tabujemy
            while ( ++iter != widgets.end() ) {
                splitDockWidget( widgets.front(), *iter, Qt::Horizontal );
            }
        }
    } else if ( organization == WidgetsOrganizationTabbed ) {
        // dodajemy pierwszy...
        std::vector< QDockWidget* >::iterator iter = widgets.begin();
        if ( iter != widgets.end() ) {
            addDockWidget( Qt::RightDockWidgetArea, *iter );
            // kolejne tabujemy
            while ( ++iter != widgets.end() ) {
                tabifyDockWidget( widgets.front(), *iter );
            }
        }

    } else {
        UTILS_ASSERT(false);
    }




//     // usuniêcie starych "œmieci"
//     for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
//         IServicePtr service = m_pServiceManager->getService(i);
//         if ( QWidget* viewWidget = reinterpret_cast<QWidget*>(service->getWidget()) ) {
//             // co musimy usun¹æ?
//             QDockWidget* dock = qobject_cast<QDockWidget*>(viewWidget->parent());
//             int index = ui->tabWidget->indexOf(viewWidget);
//             if ( dock ) {
//                 viewWidget->setParent(NULL);
//                 removeDockWidget(dock);
//                 delete dock;
//                 
//                 
// 
//             }
//             if ( index != -1 ) {
//                 ui->tabWidget->removeTab(index);
//             }
//         }
//     }
// 
//     // usniêcie starych "œmieci"
//     if ( organization == WidgetsOrganizationDocked ) {
//         centralWidget()->hide();
//     } else {
//         //
//         centralWidget()->show();
//     }
// 
// 
//     // za³adowanie styli
//     QString style;
//     if(dataManager->getApplicationSkinsFilePathCount() > 0) {
//         QFile file(core::toQString(dataManager->getApplicationSkinsFilePath(0)));
//         if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//             style = file.readAll();
//             file.close();
//         }
//     }
//     setStyleSheet(style);
// 
// 
//     // widget rendeer service - centralny
//     // setCentralWidget(reinterpret_cast<QWidget*>(m_pRenderService->getWidget()));
//     //centralWidget()->widget
// 
//     QDockWidget *dock = new QDockWidget("test");
//     addDockWidget(Qt:TopDockWidgetArea, dock);
//     
// 
// 
// 
//     setDockOptions( AllowNestedDocks | AllowTabbedDocks );
//     setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
//     
// 
//     // pozosta³e widgety "p³ywaj¹ce"
//     for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
//         IServicePtr service = m_pServiceManager->getService(i);
//         if ( QWidget* viewWidget = reinterpret_cast<QWidget*>(service->getWidget()) ) {
//             if ( organization == WidgetsOrganizationDocked ) {
//                 // dodanie dokowalnego widgetu
//                 addDockWidget(Qt::RightDockWidgetArea, embeddWidget(
//                     viewWidget, 
//                     core::toQString(service->getName()), 
//                     style,
//                     Qt::RightDockWidgetArea));
//             } else {
//                 // dodanie strony do zak³adek
//                 ui->tabWidget->addTab(viewWidget, core::toQString(service->getName()));
//             }
//         }
//     }
}

void ToolboxMain::paintEvent( QPaintEvent* event )
{
    if ( getNumViews() ) {
        frame();
    }
}

void ToolboxMain::onDockWidgetVisiblityChanged( bool visible )
{
    if ( QDockWidget* sender = qobject_cast<QDockWidget*>(QObject::sender()) ) {
        // wyszukanie us³ugi do której widget nale¿y
        for ( int i = 0; i < m_pServiceManager->getNumServices(); ++i ) {
            IServicePtr service = m_pServiceManager->getService(i);
            if ( reinterpret_cast<QWidget*>(service->getWidget()) == sender->widget() ) {
                service->visibilityChanged( service->getWidget(), visible );
            }
        }
    }

}

void ToolboxMain::onSaveLayout()
{
    // TODO: czy na pewno ma wychodziæ gdy nie uda siê sprawdziæ, czy katalog istnieje?
    QString dir = QDir::currentPath().append("/data/layouts");
    if ( QDir(dir).exists() || QDir().mkdir(dir) ) {        
        const QString fileName = QFileDialog::getSaveFileName(this, 0, dir, "*.layout");
        if ( !fileName.isEmpty() ) {
            QSettings settings(fileName, QSettings::IniFormat);
            settings.setValue("Geometry", saveGeometry());
            settings.setValue("WindowState", saveState());
        }
    } else {
        LOG_ERROR("Could not create directory: "<<core::toStdString(dir));
    }
}

void ToolboxMain::openLayout( const QString& path )
{
    readSettings(QSettings(path, QSettings::IniFormat), false);
}

void ToolboxMain::onOpenLayout()
{
    // TODO: czy na pewno ma wychodziæ gdy nie uda siê sprawdziæ, czy katalog istnieje?
    QString dir = QDir::currentPath().append("/data/layouts");        
    Push<bool> pushed(updateEnabled, false);
    const QString fileName = QFileDialog::getOpenFileName(this, 0, dir, "*.layout");
    if ( !fileName.isEmpty() ) {
        openLayout(fileName);
    }
}

void ToolboxMain::onShowSavedLayouts()
{
    // usuniêcie starych akcji
    ui->menuLoad_layout->clear();
    ui->menuLoad_layout->addAction(ui->actionLayout_open);

    // layouty wbudowane
    addLayoutsToMenu(QDir(QDir::currentPath().append("/data/resources/layouts")));
    // layouty zdefiniowane przez u¿ytkownika
    addLayoutsToMenu(QDir(QDir::currentPath().append("/data/layouts")));
}



void ToolboxMain::onLayoutTriggered()
{
    if ( QAction* action = qobject_cast<QAction*>(QObject::sender()) ) {
        openLayout( action->data().toString() );
    }
}

void ToolboxMain::addLayoutsToMenu( QDir &dir )
{
    if ( dir.exists() ) {     
        QStringList files = dir.entryList( QStringList("*.layout"), QDir::Files | QDir::Readable );
        if ( files.size() ) {
            ui->menuLoad_layout->addSeparator();
            Q_FOREACH(const QString& file, files) {
                QAction* action = new QAction(ui->menuLoad_layout);
                action->setText(file);
                action->setData(QVariant(dir.absoluteFilePath(file)));
                ui->menuLoad_layout->addAction(action);
                QObject::connect(action, SIGNAL(triggered()), this, SLOT(onLayoutTriggered()));
            }
        }
    }
}

// void ToolboxMain::SettingModel()
// {
//      ObjectService* object = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
//      if(m_model)
//      {
//          object->AddModel(m_model);
//          osg::ref_ptr<osg::Group> group = m_model->GetRoot();
//          // create group being root of scene
//          osg::ref_ptr<osg::Group> root = new osg::Group();
//          root->setName("root");
//          // create grid
//          root->addChild(createGrid());
//          // add model
//          root->addChild(group);	
//          // add to control
//          m_pRenderService->SetScene(root);
//  
//          // manage scene
//          osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene();
//          _osgControlWidget->SetScene(scene); 
//          sceneGraphWidget->SetScene(scene);
//          _timeLine->SetScene(scene); 
//          m_model->UpdateMesh();
//          m_model = (Model*)m_model;
//      }
//      m_model = object->GetModel();

//     m_pRenderService->CreateNewWindow("TEST");
//     m_pRenderService->CreateNewWindow("TEST1");
//     m_pRenderService->CreateNewWindow("TEST2");

// TESTING CONFIGURATION_FILE_MANAGER
//     ServiceManager::GetInstance()->registerService<ConfigurationFileService>(); 
// 
//     ConfigurationFileService* pCFService = m_pServiceManager->GetSystemServiceAs<ConfigurationFileService>();
//     
//     pCFService->LoadConfig("Config.ini");
//     std::list<ConfigurationGroup*> groupList = pCFService->GetConfigurationGroupList();
//     int zmienna = pCFService->GetIntParameter("MemMaxValue");
//     float f = pCFService->GetIntParameter("TimingMaxValue");
//     ConfigurationGroup* group = pCFService->GetConfigurationGroup("VisualTracker");
//     group->SetParametrVal("PerfMaxValue ", "200");
//     pCFService->SetParameter("VisualTracker","PerfMaxValue", "300");
//     pCFService->SetParameter("PerfMaxValue", "450");
//     ConfigurationGroup* dp = new ConfigurationGroup("_TEST");
//     dp->AddNewParametr("vval", "1000");
//     Parameter* par = new Parameter("cl","ak");
//     Parameter* par2 = new Parameter("standardowo-onClicked","d_d_d");
//     dp->AddNewParamter(*par);
//     dp->AddNewParamter(*par2);
//     pCFService->AddNewConfigurationGroup(*dp);
//     pCFService->Save();
// 
// }
