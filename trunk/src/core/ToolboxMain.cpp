#include "CorePCH.h"
// Go to http://dmoulding.googlepages.com/vld 
// for the Visual Leak Detector for VisualStudio
    
#include "ToolboxMain.h"
#include "ui_toolboxmaindeffile.h"
#include <core/QOSGWidget.h>
#include "TimeLine.h"
#include "GridWidget.h"

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
#include "ModelService.h"
#include "UserInterfaceService.h"
#include "RenderService.h"
#include "config/ConfigurationFileService.h"

#include <iostream>

#include "Model.h"
#include "Mesh.h"

#include "FileReader2Motion.h"
#include <utils/Debug.h>

#include <plugins/video/core/PixelFormat.h>

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <functional>

#include "ComputeThread.h"

const QString ToolboxMain::configName = QString("Toolbox_config.ini");
const QString ToolboxMain::organizationName = QString("PJWSTK");
const QString ToolboxMain::applicationName = QString("EDR");

ToolboxMain::ToolboxMain(QWidget *parent)
:   QMainWindow(parent),
    ui(new Ui::ToolboxMain),
    removeOnClick(false)
{
    pluginLoader = new core::PluginLoader();


    ui->setupUi(this);

    registerCoreServices(); 
    registerPluginsServices();


    sceneRoot = new osg::Group();
    sceneRoot->setName("root");

    // inicjalizacja us³ug
    for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
        m_pServiceManager->getService(i)->init(m_pServiceManager, sceneRoot.get());
    }


    InitializeConsoleWidget();          // Console Widget 
    InitializeControlWidget();          // Control Widget + TimeLine
    LoadConfiguration();                // Wczytuje plik konfiguracyjny

    initializeUI();


    ReadSettings();


    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateServices()));
    updateTimer.start(50);

    computeThread = new ComputeThread(m_pServiceManager, 0.02);
    computeThread->start();

	/*
	tworzy managera zasobow. w konstruktorze szuka sciezek do zasobow stalych (shadery i tbs)
	*/
    dataManager = new DataManager();

}

ToolboxMain::~ToolboxMain()
{
    clear();
}

void ToolboxMain::clear()
{
    delete ui;  // ui stuff
    std::streambuf *buf = std::cout.rdbuf(_streambuf);
    delete buf;

    m_pRenderService.reset();
    delete m_pServiceManager;
    delete pluginLoader;
    m_pUserInterfaceService = NULL;
    m_pModelService = NULL;
	if(dataManager)
		delete dataManager;
	dataManager = NULL;
    

    // remove all services
    //m_pServiceManager = NULL;

    delete computeThread;
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

void ToolboxMain::ReadSettings()
{
    QSettings settings(QSettings::IniFormat,
        QSettings::UserScope, organizationName, applicationName);

    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("WindowState").toByteArray());
}

void ToolboxMain::WriteSettings()
{
    QSettings settings(QSettings::IniFormat,
        QSettings::UserScope, organizationName, applicationName);

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

    osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene(); 
    osg::Node* sceneRoot = scene->getSceneData();

    // inicjalizacja GridWidget
    QDockWidget *gDock = new QDockWidget(tr("GridWidget"), this, Qt::WindowTitleHint);
    gDock->setObjectName(QString("GridWidget"));
    gDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _gridWidget = new GridWidget();    
    addDockWidget(Qt::RightDockWidgetArea, gDock);
    gDock->setWidget((QWidget*)_gridWidget);
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

void ToolboxMain::InitializeConsoleWidget()
{
    QDockWidget *dock; 
    dock = new QDockWidget(tr("Console"), this, Qt::WindowTitleHint);
    dock->setObjectName("Console");
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    _consoleWidget = new ConsoleWidget();    
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    dock->setWidget((QWidget*)_consoleWidget); 
    // Inicjalizacja konsoli
    ConsoleWidgetOutputBuffer *ob = new ConsoleWidgetOutputBuffer(_consoleWidget, 256);
    _streambuf = std::cout.rdbuf(ob);
    std::cout << "Console initialized..." << std::endl; 
}

void ToolboxMain::InitializeOGSWidget()
{
//  setCentralWidget(_osgView); 
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->setName("root");
    root->addChild(createGrid());

    m_pRenderService->SetScene(root);
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
    m_pServiceManager->updatePass();
}

void ToolboxMain::registerCoreServices()
{
    //1. Service manafger
    m_pUserInterfaceService = new UserInterfaceService();
    m_pServiceManager = new ServiceManager();
    m_pModelService = new ModelService();
    m_pRenderService = RenderServicePtr(new RenderService());

    //2. Model Service
    m_pServiceManager->registerService(IServicePtr(m_pModelService));

    //3. UserInterface Service
    m_pServiceManager->registerService(IServicePtr(m_pUserInterfaceService));
    m_pUserInterfaceService->setMainWindow(this);

    //4. Render Service
    m_pServiceManager->registerService(IServicePtr(m_pRenderService));
}

void ToolboxMain::registerPluginsServices()
{
    pluginLoader->load();
    for ( size_t i = 0; i < pluginLoader->getNumPlugins(); ++i ) {
        core::PluginPtr plugin = pluginLoader->getPlugin(i);
        for ( size_t j = 0; j < plugin->getNumServices(); ++j ) {
            m_pServiceManager->registerService(plugin->getService(j));
        }
    }
}

void ToolboxMain::onOpen()
{
	const QString fileName = QFileDialog::getExistingDirectory(this, 0, QDir::currentPath()); // TODO: tymczasowo dodaje asf do filtrów Open File
    if (!fileName.isEmpty()) 
    {
        std::string pathVal = fileName.toStdString();
        const std::string& path = pathVal;


        openFile(path);

        return;
    }
    QMessageBox::warning(this, QString("Error!"), QString("Failed to load trials."));
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

void ToolboxMain::initializeUI()
{
    // widget rendeer service - centralny
    setCentralWidget(reinterpret_cast<QWidget*>(m_pRenderService->getWidget()));

    // pozosta³e widgety "p³ywaj¹ce"
    for (int i = 0; i < m_pServiceManager->getNumServices(); ++i) {
        IServicePtr service = m_pServiceManager->getService(i);
        IWidget* widget = service->getWidget();
        if ( widget && service != m_pRenderService ) {
            // dodajemy widget dokowalny
            QDockWidget* dock = new QDockWidget( QString::fromStdString(service->getName()), this, Qt::WindowTitleHint);
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            addDockWidget(Qt::BottomDockWidgetArea, dock);
            // HACK
            QWidget* qwidget = reinterpret_cast<QWidget*>(widget);
            QObject* parent = qwidget->parent();
            dock->setWidget(qwidget);
            dock->setObjectName( QString::fromStdString( service->getName() ) + qwidget->objectName() + "WIDGET" );
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
}

void ToolboxMain::onCustomAction()
{
    QObject* obj = QObject::sender();
    std::string path = obj->objectName().toStdString();
    this->triggerMenuItem(path, false);
}

void ToolboxMain::onCustomAction( bool triggered )
{
    QObject* obj = QObject::sender();
    std::string path = obj->objectName().toStdString();
    this->triggerMenuItem(path, triggered);
}

void ToolboxMain::onRemoveMenuItem( const std::string& path )
{
    // TODO: rekurencyjne usuwanie niepotrzebnych podmenu
    QAction* action = findChild<QAction*>(QString::fromStdString(path));
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
        QString itemName = QString::fromStdString(pathPart);

        if ( next == tokens.end() ) 
        {
            // liœæ
            QAction* action = new QAction(this);
            action->setObjectName( QString::fromStdString(pathPart) );
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
    std::cout << " " << std::endl; 
    std::cout << "File Opened: " << path << std::endl;
    std::cout << "---------------------------------------------------------------" << std::endl; 

	//zaladowanie sciezek prob pomiarowych
	dataManager->setTrialsPath(path);
	dataManager->loadTrials();

    FileReader2Motion::ReadFile(dataManager);

    m_pServiceManager->loadDataPass(dataManager);

    m_pRenderService->AddObjectToRender(createGrid());

    // manage scene
    osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene(); 
    osg::Node* sceneRoot = scene->getSceneData();
    //   _osgControlWidget->SetScene(scene); 
    _gridWidget->SetScene(sceneRoot);
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
//          _gridWidget->SetScene(scene);
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
