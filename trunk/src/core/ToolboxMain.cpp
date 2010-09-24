// Go to http://dmoulding.googlepages.com/vld 
// for the Visual Leak Detector for VisualStudio
    
#include "ToolboxMain.h"
#include "ui_toolboxmaindeffile.h"
#include "QOSGWidget.h"
#include "AdapterWidget.h"
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

#include "ConsoleWidget.h"
#include "Logger.h"

#include "ServiceManager.h"
// Zakomentowa³em wszystkie u¿ycia AnimationService w tym pliku - P. Gwiazdowski
//#include "../anim/AnimationService.h"
#include "PluginServices.h"
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


// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*
#define CONFIG_FILE "Toolbox_config.ini"

#define OSGTERRAIN_VALIDDATAOPERATOR 1 




//--------------------------------------------------------------------------------------------------
// Statics definitions
const QString ToolboxMain::_settingsOrganizationName = QString("Artifex Mundi");
const QString ToolboxMain::_settingsApplicationName = QString("Toolbox");

//--------------------------------------------------------------------------------------------------
ToolboxMain::ToolboxMain(QWidget *parent): 
  QMainWindow(parent)
, _ui(new Ui::ToolboxMain)
{
    _ui->setupUi(this);

    InitializeCoreServices(); 
    InitializeConsoleWidget();          // Console Widget 
    InitializeControlWidget();          // Control Widget + TimeLine
    LoadConfiguration();                // Wczytuje plik konfiguracyjny

    // w przysz³oœci setLayout
    QDockWidget *cdock = dynamic_cast<QDockWidget *>(_consoleWidget->parent()); 
    QDockWidget *tldock = dynamic_cast<QDockWidget *>(_timeLine->parent()); 

    if(cdock && tldock)
        tabifyDockWidget(cdock, tldock); 

    _plugins.clear(); 
    LoadPlugins(); 
    ReadSettings();
    CreateMenus();          // Inicjalizacja menu: 
    LoadWindowMenuWidget();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::InitializeCoreServices()
{
    //1. Service manafger
    m_pUserInterfaceService = new UserInterfaceService();
    m_pServiceManager = new ServiceManager();
    m_pModelService = new ModelService();
    m_pRenderService = new RenderService();
    m_pPluginService = new PluginService();


    //2. Model Service
    m_pServiceManager->RegisterServiceAs(m_pModelService, ModelService::CLASS_ID);

    //3. UserInterface Service
    m_pServiceManager->RegisterServiceAs(m_pUserInterfaceService, UserInterfaceService::CLASS_ID);
    m_pUserInterfaceService->InicializeServices("QT",this);

    //4. Render Service
    m_pServiceManager->RegisterServiceAs(m_pRenderService, RenderService::CLASS_ID);

    //5. Plugin Service
    m_pServiceManager->RegisterServiceAs(m_pPluginService, PluginService::CLASS_ID); 

    //6. Animation Service from plugin
    //m_pAnimationService = dynamic_cast<AnimationService*>(m_pServiceManager->GetSystemService(AnimationService::CLASS_ID));
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::CreateMenus()
{
    m_pUserInterfaceService->AddMenu("File");

    m_pUserInterfaceService->AddAction("File", "Open...", "Open()");
    m_pUserInterfaceService->AddAction("File", "Set Model", "SettingModel()");

    m_pUserInterfaceService->AddMenu("View");
    m_pUserInterfaceService->AddAction("View", "Wireframe", "WireFrameView()");
    m_pUserInterfaceService->AddAction("View", "Bone", "BoneView()");
    m_pUserInterfaceService->AddAction("View", "Material", "MaterialView()");
    //  _fileMenu->addSeparator();

    m_pUserInterfaceService->AddMenu("View/Model");
    m_pUserInterfaceService->AddAction("View/Model", "Points", "PointViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Lines", "LinesViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Line strip", "LineStripViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Line loop", "LineLoopViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Triangle", "TriangleViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Triangle strip", "TriangleStripViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Triangle fun", "TriangleFunViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Quads", "QuadsViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Quad strip", "QuadStripViewModel()");
    m_pUserInterfaceService->AddAction("View/Model", "Polygon", "PolygonViewModel()");


    _windowMenu = menuBar()->addMenu(tr("&Window"));
}

//--------------------------------------------------------------------------------------------------
ToolboxMain::~ToolboxMain()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::Clear()
{
    delete _ui;  // ui stuff
    std::streambuf *buf = std::cout.rdbuf(_streambuf);
    delete buf;

    m_pPluginService = NULL;
    m_pUserInterfaceService = NULL;
    //m_pAnimationService = NULL;
    m_pModelService = NULL;
    m_pRenderService = NULL;

    // remove all services
    m_pServiceManager = NULL;
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LoadConfiguration()
{
    /*
    QString path = qApp->applicationDirPath();
    path.append("/"); 
    path.append(CONFIG_FILE); 
    ConfigurationService *config = new ConfigurationService(); 
    config->loadConfiguration( std::string(path.toUtf8()) ); /**/
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::ReadSettings()
{
    QSettings settings(QSettings::IniFormat,
        QSettings::UserScope,
        _settingsOrganizationName,
        _settingsApplicationName);

    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("WindowState").toByteArray());
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::WriteSettings()
{
    QSettings settings(QSettings::IniFormat,
        QSettings::UserScope,
        _settingsOrganizationName,
        _settingsApplicationName);

    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::closeEvent(QCloseEvent* event)
{
    WriteSettings();
    QMainWindow::closeEvent(event);
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::InitializeControlWidget()
{
    InitializeOGSWidget();  // MainWidget 

    osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene(); 

    // TimeLine
    QDockWidget *tldock = new QDockWidget(tr("Time Line"), this, Qt::WindowTitleHint); 
    tldock->setObjectName(QString("Time Line"));
    tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    _timeLine = new TimeLine(); 
    addDockWidget(Qt::BottomDockWidgetArea, tldock);
    tldock->setWidget((QWidget*)_timeLine); 

    _timeLine->SetScene(scene); 

    connect(&(m_pRenderService->GetMainAdapterWidget()->_timer), SIGNAL(timeout()), _timeLine, SLOT(update())); 

    // inicjalizacja GridWidget
    QDockWidget *gDock = new QDockWidget(tr("GridWidget"), this, Qt::WindowTitleHint);
    gDock->setObjectName(QString("GridWidget"));
    gDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _gridWidget = new GridWidget();    
    addDockWidget(Qt::RightDockWidgetArea, gDock);
    gDock->setWidget((QWidget*)_gridWidget);
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LoadPlugins()
{
    PluginList pList = m_pPluginService->GetPluginList();

    for(std::vector<ISystemPlugin*>::iterator itr = pList.begin(); itr != pList.end(); itr++)
    {
        osgViewer::Scene* scene =  m_pRenderService->GetMainWindowScene();

        QDockWidget *tldock = new QDockWidget((*itr)->GetPluginName().c_str(), this, Qt::WindowTitleHint); 
        tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        addDockWidget(Qt::BottomDockWidgetArea, tldock);

        for(int i = 0; i < (*itr)->GetWidgetsCount(); i++)
        {
            QWidget* widget = (QWidget*)(*itr)->GetDockWidget(i);
            tldock->setWidget(widget); 

            QString dockWidgetName = (*itr)->GetPluginName().c_str() + widget->objectName();
            tldock->setObjectName(dockWidgetName);

            (*itr)->SetScene(scene, m_pServiceManager); 
        }
    }
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LoadWindowMenuWidget()
{
    //    QMainWindowPrivate * const d = reinterpret_cast<QMainWindowPrivate *>(qGetPtrHelper(d_ptr)); 

    QList<QDockWidget *> dockwidgets = qFindChildren<QDockWidget *>(this);
    if (dockwidgets.size()) 
    {
        for (int i = 0; i < dockwidgets.size(); ++i) 
        {
            QDockWidget *dockWidget = dockwidgets.at(i);
            if (dockWidget->parentWidget() == this) 
            {
                _windowMenu->addAction(dockwidgets.at(i)->toggleViewAction());
            }
        }
        _windowMenu->addSeparator();
    }

    QList<QToolBar *> toolbars = qFindChildren<QToolBar *>(this);
    if (toolbars.size()) 
    {
        for (int i = 0; i < toolbars.size(); ++i) 
        {
            QToolBar *toolBar = toolbars.at(i);
            _windowMenu->addAction(toolbars.at(i)->toggleViewAction());
        }
    }
}

// TODO:  zrobiæ jedn¹ funkcje do tego. - wybor opcjii.
//--------------------------------------------------------------------------------------------------
void ToolboxMain::BoneView()
{

}
//--------------------------------------------------------------------------------------------------
void ToolboxMain::WireFrameView()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::MaterialView()
{

}

//--------------------------------------------------------------------------------------------------
// loads model
void ToolboxMain::Open()
{
    // TODO:
    // tymczasowe rozwiazanie - powinno sie raczej dac mozliwosc ladowania wiekszej ilosci modeli,
    const QString fileName = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath());

    if (!fileName.isEmpty()) 
    {
        std::cout << " " << std::endl; 
        std::cout << "File Opened: " << std::string(fileName.toUtf8()) << std::endl;
        std::cout << "---------------------------------------------------------------" << std::endl; 


        Model* model = new Model();
		FileReader2Motion::ReadFile(fileName.toStdString(), model);
       
        m_pRenderService->SetScene(model);
        m_pRenderService->AddObjectToRender(CreateGrid());

        m_pModelService->Clear();
        m_pModelService->AddModel(model);
        //m_pAnimationService->LoadAnimation(model);


        // manage scene
        osgViewer::Scene* scene = m_pRenderService->GetMainWindowScene(); 
     //   _osgControlWidget->SetScene(scene); 
        _gridWidget->SetScene(scene);
        _timeLine->SetScene(scene); 

        PluginList pluginList = m_pPluginService->GetPluginList();
        for(std::vector<ISystemPlugin* >::const_iterator it = pluginList.begin(); it != pluginList.end(); it++)
        {
            (*it)->SetScene(scene, m_pServiceManager);
        }

        return;

        if (model)
            delete model;
    }
    QMessageBox::warning(this, QString("Error!"), QString("Failed to load model."));
}

//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
void ToolboxMain::InitializeOGSWidget()
{
//  setCentralWidget(_osgView); 
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->setName("root");
    root->addChild(CreateGrid());

    m_pRenderService->SetScene(root);
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Node> ToolboxMain::CreateGrid()
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

//--------------------------------------------------------------------------------------------------
void ToolboxMain::SettingModel()
{
//      ObjectService* object = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
//      if(m_model)
//      {
//          object->AddModel(m_model);
//          osg::ref_ptr<osg::Group> group = m_model->GetRoot();
//          // create group being root of scene
//          osg::ref_ptr<osg::Group> root = new osg::Group();
//          root->setName("root");
//          // create grid
//          root->addChild(CreateGrid());
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

    m_pRenderService->CreateNewWindow("TEST");
    m_pRenderService->CreateNewWindow("TEST1");
    m_pRenderService->CreateNewWindow("TEST2");

// TESTING CONFIGURATION_FILE_MANAGER
//     ServiceManager::GetInstance()->RegisterServiceAs<ConfigurationFileService>(); 
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
//     Parameter* par2 = new Parameter("standardowo-test","d_d_d");
//     dp->AddNewParamter(*par);
//     dp->AddNewParamter(*par2);
//     pCFService->AddNewConfigurationGroup(*dp);
//     pCFService->Save();

}

//--------------------------------------------------------------------------------------------------
// TODO:  refactorng KONIECZNIE -  tymczasem dla testów, spróbowac zrobiæ jedna medote zarz¹cdaj¹ca tymi pozosta³ymi
void ToolboxMain::PointViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LinesViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LineStripViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LineLoopViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::TriangleViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::TriangleStripViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::TriangleFunViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::QuadsViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::QuadStripViewModel()
{

}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::PolygonViewModel()
{

}

