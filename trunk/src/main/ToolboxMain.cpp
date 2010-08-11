// Go to http://dmoulding.googlepages.com/vld 
// for the Visual Leak Detector for VisualStudio

//#include <vld.h>        
#include "ToolboxMain.h"
#include "ui_toolboxmaindeffile.h"
#include "ui_OsgWidget.h"
#include "OsgControlWidget.h"
#include "QOSGWidget.h"
#include "AdapterWidget.h"
#include "TimeLine.h"
#include "GridWidget.h"
#include "ModelFacade.h"

#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>

#include <osgViewer/GraphicsWindow>
#include <osgText/Text>
#include <osg/ShapeDrawable>
#include <osgViewer/Scene>
#include <iostream>
//#include <osgGA/UFOManipulator>
//#include <osgGA/NodeTrackerManipulator>
//#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <QDir>

#include "ConsoleWidget.h"
#include "Logger.h"

#include "ServiceManager.h"
#include "AnimationService.h"
#include "ObjectService.h"
#include "../utils/ConfigFileManage/ConfigurationFileService.h"
#include "PluginServices.h"

#include <iostream>

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*
#define CONFIG_FILE "Toolbox_config.ini"

#define OSGTERRAIN_VALIDDATAOPERATOR   1 


//--------------------------------------------------------------------------------------------------
// Statics definitions
const QString ToolboxMain::_settingsOrganizationName = QString("Artifex Mundi");
const QString ToolboxMain::_settingsApplicationName = QString("Toolbox");

//--------------------------------------------------------------------------------------------------
ToolboxMain::ToolboxMain(QWidget *parent): 
  QMainWindow(parent)
, _ui(new Ui::ToolboxMain)
, _model(NULL)
{
    _pServiceManager = ServiceManager::GetInstance();
    _ui->setupUi(this);

    // Console Widget 
    InitializeConsoleWidget();
    // Control Widget + TimeLine
    InitializeControlWidget();

    // Wczytuje plik konfiguracyjny
    LoadConfiguration(); 

    // w przysz�o�ci setLayout
    QDockWidget *cdock = dynamic_cast<QDockWidget *>(_consoleWidget->parent()); 
    QDockWidget *tldock = dynamic_cast<QDockWidget *>(_timeLine->parent()); 

    if(cdock && tldock)
        tabifyDockWidget(cdock, tldock); 

    ReadSettings();

    // Inicjalizacja podstawowych serwis�w 
    InitializeCoreServices(); 

    _plugins.clear(); 
    LoadPlugins(); 
}

//--------------------------------------------------------------------------------------------------
ToolboxMain::~ToolboxMain()
{
    // ui stuff
    delete _ui;
    std::streambuf *buf = std::cout.rdbuf(_streambuf);
    delete buf;

    _pPluginService = NULL;
    // remove all services
    ServiceManager::DestroyInstance();
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
    // MainWidget 
    InitializeOGSWidget();

    // ControlWidget
    QDockWidget *dock = new QDockWidget(tr("Control (sample)"), this, Qt::WindowTitleHint);
    dock->setObjectName(QString("Control (sample)"));
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _osgControlWidget = new OsgControlWidget();    
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setWidget((QWidget*)_osgControlWidget);

    osgViewer::Scene* scene = _osgView->getScene(); 
    _osgControlWidget->SetScene(scene); 

    // TimeLine
    QDockWidget *tldock = new QDockWidget(tr("Time Line"), this, Qt::WindowTitleHint); 
    tldock->setObjectName(QString("Time Line"));
    tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    _timeLine = new TimeLine(); 
    addDockWidget(Qt::BottomDockWidgetArea, tldock);
    tldock->setWidget((QWidget*)_timeLine); 

    _timeLine->SetScene(scene); 

    connect(&_osgView->_timer, SIGNAL(timeout()), _timeLine, SLOT(update())); 

    // inicjalizacja GridWidget
    QDockWidget *gDock = new QDockWidget(tr("GridWidget"), this, Qt::WindowTitleHint);
    gDock->setObjectName(QString("GridWidget"));
    gDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _gridWidget = new GridWidget();    
    addDockWidget(Qt::RightDockWidgetArea, gDock);
    gDock->setWidget((QWidget*)_gridWidget);

    // Inicjalizacja menu: 
    CreateActions(); 
    CreateMenus(); 

    //osgView_->get
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LoadPlugins()
{
    //Nowy spos�b �adownaie lugin�w 6.08.2010
    _pPluginService = ServiceManager::GetInstance()->GetSystemServiceAs<PluginService>();

    PluginList pList = _pPluginService->GetPluginList();

    for(std::vector<IControlPlugin*>::iterator itr = pList.begin(); itr != pList.end(); itr++)
    {
        osgViewer::Scene* scene = _osgView->getScene(); 

        QDockWidget *tldock = new QDockWidget((*itr)->GetPluginName().c_str(), this, Qt::WindowTitleHint); 
        tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        addDockWidget(Qt::BottomDockWidgetArea, tldock);

        QWidget* widget = (*itr)->GetDockWidget(0);
        tldock->setWidget(widget); 

        QString dockWidgetName = (*itr)->GetPluginName().c_str() + widget->objectName();
        tldock->setObjectName(dockWidgetName);

        (*itr)->SetScene(scene); 
    }
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::InitializePlugin(QObject *plugin)
{

}

//--------------------------------------------------------------------------------------------------
// To ma by� w pluginach 
void ToolboxMain::CreateActions()
{
    _openAct = new QAction(tr("&Open..."), this);
    _setModelTest = new QAction(tr("&Set Model"), this);
    _viewWFAct = new QAction(tr("&WireFrame"), this);
    _viewMaterialAct = new QAction(tr("&Material"), this);
    _viewBoneAct = new QAction(tr("&Bone"), this);

    _points = new QAction(tr("&Points"), this);
    _lines = new QAction(tr("&Lines"), this);
    _line_strip = new QAction(tr("&Line strip"), this);
    _line_loop = new QAction(tr("&Line loop"), this);
    _triangle = new QAction(tr("&Triangle"), this);
    _triangle_strip = new QAction(tr("&Triangle strip"), this);
    _triangle_fun = new QAction(tr("&Triangle fun"), this);
    _quads = new QAction(tr("&Quads"), this);
    _quad_strip = new QAction(tr("&Quad strip"), this);
    _polygon = new QAction(tr("&Polygon"), this);

    _viewBoneAct->setShortcut(tr("Ctrl+N"));
    _viewMaterialAct->setShortcut(tr("Ctrl+N"));
    _viewWFAct->setShortcut(tr("Ctrl+W"));
    _openAct->setShortcut(tr("Ctrl+O"));

    connect(_openAct, SIGNAL(triggered()), this, SLOT(Open()));
    connect(_setModelTest, SIGNAL(triggered()), this, SLOT(SettingModel()));
    connect(_viewWFAct, SIGNAL(triggered()), this, SLOT(WireFrameView()));
    connect(_viewMaterialAct, SIGNAL(triggered()), this, SLOT(MaterialView()));
    connect(_viewBoneAct, SIGNAL(triggered()), this, SLOT(BoneView()));
    connect(_points, SIGNAL(triggered()), this, SLOT(PointViewModel()));
    connect(_lines, SIGNAL(triggered()), this, SLOT(LinesViewModel()));
    connect(_line_strip, SIGNAL(triggered()), this, SLOT(LineStripViewModel()));
    connect(_line_loop, SIGNAL(triggered()), this, SLOT(LineLoopViewModel()));
    connect(_triangle, SIGNAL(triggered()), this, SLOT(TriangleViewModel()));
    connect(_triangle_strip, SIGNAL(triggered()), this, SLOT(TriangleStripViewModel()));
    connect(_triangle_fun, SIGNAL(triggered()), this, SLOT(TriangleFunViewModel()));
    connect(_quads, SIGNAL(triggered()), this, SLOT(QuadsViewModel()));
    connect(_quad_strip, SIGNAL(triggered()), this, SLOT(QuadStripViewModel()));
    connect(_polygon, SIGNAL(triggered()), this, SLOT(PolygonViewModel()));

    // connect(textEdit, SIGNAL(copyAvailable(bool)),
    //   cutAct, SLOT(setEnabled(bool)));
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::CreateMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_openAct);
    _fileMenu->addAction(_setModelTest);

    _fileMenu = menuBar()->addMenu(tr("&View"));
    _fileMenu->addAction(_viewWFAct);
    _fileMenu->addAction(_viewBoneAct);
    _fileMenu->addAction(_viewMaterialAct);
    _fileMenu->addSeparator();

    _modelMenu = _fileMenu->addMenu(tr("&Model"));
    _modelMenu->addAction(_points); 
    _modelMenu->addAction(_lines);
    _modelMenu->addAction(_line_strip);
    _modelMenu->addAction(_line_loop);
    _modelMenu->addAction(_triangle);
    _modelMenu->addAction(_triangle_strip);
    _modelMenu->addAction(_triangle_fun);
    _modelMenu->addAction(_quads);
    _modelMenu->addAction(_quad_strip);
    _modelMenu->addAction(_polygon);
}

// TODO:  zrobi� jedn� funkcje do tego. - wybor opcjii.
//--------------------------------------------------------------------------------------------------
void ToolboxMain::BoneView()
{
    _pScene->AddVieModelFlag(ObjectService::BONE);

    if(!(_pScene->GetViewModelFlag() & ObjectService::BONE))
    {
        _model->RemoveGeodes();
        _model->LoadModel();
    }

    _model->Update();
}
//--------------------------------------------------------------------------------------------------
void ToolboxMain::WireFrameView()
{
    _pScene->AddVieModelFlag(ObjectService::WIREFRAME);

    if(!(_pScene->GetViewModelFlag() & ObjectService::WIREFRAME))
        _model->ResetWireFrame();

    _model->Update();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::MaterialView()
{
    _pScene->AddVieModelFlag(ObjectService::MATERIAL);

    if(_pScene->GetViewModelFlag() & ObjectService::MATERIAL)
        _model->ApplyMaterial(0, 0, true);
    else
        _model->ApplyMaterial(0, 0, false);
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

        Model* model = new Model(std::string(fileName.toUtf8()));
        osg::ref_ptr<osg::Group> group = model->GetRoot();

        
        // test
        model->Update();
        //model->updateSkeletonMesh();

        
        // create group being root of scene
        osg::ref_ptr<osg::Group> root = new osg::Group();
        root->setName("root");
        // create grid
        root->addChild(CreateGrid());
        // add model
        root->addChild(group);	

        // add to control
        _osgView->setSceneData(root);

        // add model to service
        ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 

        _pScene = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
        _pScene->AddModel(model);

        // manage scene
        osgViewer::Scene* scene = _osgView->getScene(); 
        _osgControlWidget->SetScene(scene); 

        _gridWidget->SetScene(scene);

        _timeLine->SetScene(scene); 

        PluginList pluginList = _pPluginService->GetPluginList();
        for(std::vector<IControlPlugin* >::const_iterator it = pluginList.begin(); it != pluginList.end(); it++)
        {
            (*it)->SetScene(scene);
        }

        _model = dynamic_cast<Model*>(_pScene->GetModel());  // zawsze mamy tylko jeden model.

        _pScene->SetViewModelFlag(ObjectService::MODEL | ObjectService::MATERIAL);
        _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLES);

        _model = _pScene->GetModel();
        return;
        //	}

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
    _osgView = new ViewerQT(this, 0, 0);
    setCentralWidget(_osgView); 
    _osgView->addEventHandler(new osgViewer::StatsHandler);
    osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator(); 
    _osgView->setCameraManipulator(cameraManipulator);
    // create group being root of scene
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->setName("root");
    // create grid
    root->addChild(CreateGrid());
    _osgView->setSceneData(root);
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
void ToolboxMain::InitializeCoreServices()
{
    ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
    AnimationService* pAnimation = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
    pAnimation->SetScene(_osgView->getScene()); 

    ServiceManager::GetInstance()->RegisterServiceAs<PluginService>();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::SettingModel()
{
     ObjectService* object = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
     if(_model)
     {
         object->AddModel(_model);
         osg::ref_ptr<osg::Group> group = _model->GetRoot();
         // create group being root of scene
         osg::ref_ptr<osg::Group> root = new osg::Group();
         root->setName("root");
         // create grid
         root->addChild(CreateGrid());
         // add model
         root->addChild(group);	
         // add to control
         _osgView->setSceneData(root);
 
         // manage scene
         osgViewer::Scene* scene = _osgView->getScene(); 
         _osgControlWidget->SetScene(scene); 
         _gridWidget->SetScene(scene);
         _timeLine->SetScene(scene); 
         _model->UpdateMesh();
         _model = (Model*)_model;
     }
     _model = object->GetModel();

// TESTING CONFIGURATION_FILE_MANAGER
    ServiceManager::GetInstance()->RegisterServiceAs<ConfigurationFileService>(); 

    ConfigurationFileService* pCFService = _pServiceManager->GetSystemServiceAs<ConfigurationFileService>();
    
    pCFService->LoadConfig("Config.ini");
    std::list<ConfigurationGroup*> groupList = pCFService->GetConfigurationGroupList();
    int zmienna = pCFService->GetIntParameter("MemMaxValue");
    float f = pCFService->GetIntParameter("TimingMaxValue");
    ConfigurationGroup* group = pCFService->GetConfigurationGroup("VisualTracker");
    group->SetParametrVal("PerfMaxValue ", "200");
    pCFService->SetParameter("VisualTracker","PerfMaxValue", "300");
    pCFService->SetParameter("PerfMaxValue", "450");
    ConfigurationGroup* dupa = new ConfigurationGroup("DUPA_TEST");
    dupa->AddNewParametr("walek", "1000");
    Parameter* par = new Parameter("cwel","kozak");
    Parameter* par2 = new Parameter("standardowo-test","dupa_dupa_dupa");
    dupa->AddNewParamter(*par);
    dupa->AddNewParamter(*par2);
    pCFService->AddNewConfigurationGroup(*dupa);
    pCFService->Save();

//     ConfigurationFile::getInstance().LoadConfig("Config.ini");
//     std::list<ConfigurationGroup*> groupList = ConfigurationFile::getInstance().GetConfigurationGroupList();
//     int zmienna = ConfigurationFile::getInstance().GetIntParameter("MemMaxValue");
//     float f = ConfigurationFile::getInstance().GetIntParameter("TimingMaxValue");
//     ConfigurationGroup* group = ConfigurationFile::getInstance().GetConfigurationGroup("VisualTracker");
//     group->SetParametrVal("PerfMaxValue ", "200");
//     ConfigurationFile::getInstance().SetParameter("VisualTracker","PerfMaxValue", "300");
//     ConfigurationFile::getInstance().SetParameter("PerfMaxValue", "450");
//     ConfigurationGroup* dupa = new ConfigurationGroup("DUPA_TEST");
//     dupa->AddNewParametr("walek", "1000");
//     Parameter* par = new Parameter("cwel","kozak");
//     Parameter* par2 = new Parameter("standardowo-test","dupa_dupa_dupa");
//     dupa->AddNewParamter(*par);
//     dupa->AddNewParamter(*par2);
//     ConfigurationFile::getInstance().AddNewConfigurationGroup(*dupa);
//     ConfigurationFile::getInstance().Save();
}

//--------------------------------------------------------------------------------------------------
// TODO:  refactorng KONIECZNIE -  tymczasem dla test�w, spr�bowac zrobi� jedna medote zarz�cdaj�ca tymi pozosta�ymi
void ToolboxMain::PointViewModel()
{
    Model* model;

    ObjectService* pScene = _pServiceManager->GetSystemServiceAs<ObjectService>(); 
    pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::POINTS);
    model = dynamic_cast<Model*>(pScene->GetModel());  // zawsze mamy tylko jeden model.
    model->Update();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LinesViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::LINES);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LineStripViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::LINE_STRIP);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::LineLoopViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::LINE_LOOP);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::TriangleViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLES);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::TriangleStripViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLE_STRIP);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::TriangleFunViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLE_FAN);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::QuadsViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::QUADS);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::QuadStripViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::QUAD_STRIP);
    _model->UpdateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::PolygonViewModel()
{
    _pScene->SetPrimitiveModeFlag(osg::PrimitiveSet::POLYGON);
    _model->UpdateMesh();
}
