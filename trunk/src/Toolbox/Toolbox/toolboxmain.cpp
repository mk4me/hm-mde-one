// Go to http://dmoulding.googlepages.com/vld 
// for the Visual Leak Detector for VisualStudio

//#include <vld.h>        
#include "toolboxmain.h"
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
#include "ConfigurationService.h"
#include "Logger.h"

#include "ServiceManager.h"
#include "AnimationService.h"
#include "ObjectService.h"

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define CONFIG_FILE "Toolbox_config.ini"

//--------------------------------------------------------------------------------------------------
ToolboxMain::ToolboxMain(QWidget *parent)
  : QMainWindow(parent), _ui(new Ui::ToolboxMain), _model2(NULL)
{
    _pServiceManager = ServiceManager::GetInstance();
    _ui->setupUi(this);

    // Console Widget 
    initializeConsoleWidget();
    // Control Widget + TimeLine
    initializeControlWidget();

    // Wczytuje plik konfiguracyjny
    loadConfiguration(); 

    _plugins.clear(); 
    loadPlugins(); 

    // w przysz³oœci setLayout
    QDockWidget *cdock = dynamic_cast<QDockWidget *>(_consoleWidget->parent()); 
    QDockWidget *tldock = dynamic_cast<QDockWidget *>(_timeLine->parent()); 

    if(cdock && tldock)
      tabifyDockWidget(cdock, tldock); 

    readSettings();

    // Inicjalizacja podstawowych serwisów 
    InitializeCoreServices(); 

}

//--------------------------------------------------------------------------------------------------
ToolboxMain::~ToolboxMain()
{
	// ui stuff
	delete _ui;
	std::streambuf *buf = std::cout.rdbuf(_streambuf);
	delete buf;

	// remove all services
	ServiceManager::DestroyInstance();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::loadConfiguration()
{
  /*
  QString path = qApp->applicationDirPath();
  path.append("/"); 
  path.append(CONFIG_FILE); 
  ConfigurationService *config = new ConfigurationService(); 
  config->loadConfiguration( std::string(path.toUtf8()) ); /**/
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       _settingsOrganizationName,
                       _settingsApplicationName);

    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("WindowState").toByteArray());
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::writeSettings()
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
    writeSettings();
    QMainWindow::closeEvent(event);
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::initializeControlWidget()
{
  // MainWidget 
  initializeOGSWidget();

  // ControlWidget
  QDockWidget *dock = new QDockWidget(tr("Control (sample)"), this, Qt::WindowTitleHint);
  dock->setObjectName(QString("Control (sample)"));
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _osgControlWidget = new OsgControlWidget();    
  addDockWidget(Qt::RightDockWidgetArea, dock);
  dock->setWidget((QWidget*)_osgControlWidget);

  osgViewer::Scene* scene = _osgView->getScene(); 
  _osgControlWidget->setScene(scene); 

  // TimeLine
  QDockWidget *tldock = new QDockWidget(tr("Time Line"), this, Qt::WindowTitleHint); 
  tldock->setObjectName(QString("Time Line"));
  tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  _timeLine = new TimeLine(); 
  addDockWidget(Qt::BottomDockWidgetArea, tldock);
  tldock->setWidget((QWidget*)_timeLine); 

  _timeLine->setScene(scene); 

  connect(&_osgView->_timer, SIGNAL(timeout()), _timeLine, SLOT(update())); 

 // inicjalizacja GridWidget
  QDockWidget *gDock = new QDockWidget(tr("GridWidget"), this, Qt::WindowTitleHint);
  gDock->setObjectName(QString("GridWidget"));
  gDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  _gridWidget = new GridWidget();    
  addDockWidget(Qt::RightDockWidgetArea, gDock);
  gDock->setWidget((QWidget*)_gridWidget);

  // Inicjalizacja menu: 
  createActions(); 
  createMenus(); 
                    
  //osgView_->get
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::loadPlugins()
{
  QDir pluginsDir = QDir(qApp->applicationDirPath());

  pluginsDir.cd("plugins");

  foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
  {
    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    QObject *plugin = loader.instance();
    if (plugin) 
    {
      initializePlugin(plugin);
      //pluginFileNames += fileName;
    }
  }
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::initializePlugin(QObject *plugin)
{
  IControlPlugin *iControlPlugin = qobject_cast<IControlPlugin *>(plugin);
  if (iControlPlugin)
  {
    osgViewer::Scene* scene = _osgView->getScene(); 

    _plugins.push_back(plugin);

    QDockWidget *tldock = new QDockWidget(iControlPlugin->getPluginName(), this, Qt::WindowTitleHint); 
    tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, tldock);

    QWidget* widget = iControlPlugin->getDockWidget(0);
    tldock->setWidget(widget); 

    // Nazwa dock widgetu musi byæ unikalna, ¿eby dzia³a³o zapisywanie stanu okien.
    // Na razie tworzona na podstawie nazwy pluginu i widgeta, ale jeœli któraœ
    // z tych nazw mo¿e siê powtarzaæ, trzeba bêdzie to zmodyfikowaæ.
    QString dockWidgetName = iControlPlugin->getPluginName() + widget->objectName();
    tldock->setObjectName(dockWidgetName);

    iControlPlugin->setScene(scene); 
  }
}

//--------------------------------------------------------------------------------------------------
// To ma byæ w pluginach 
void ToolboxMain::createActions()
{
  _openAct = new QAction(tr("&Open..."), this);
  _setModelTest = new QAction(tr("&Set Model"), this);
  _viewWFAct = new QAction(tr("&WireFrame"), this);
  _viewMaterialAct = new QAction(tr("&Material"), this);
  _viewBoneAct = new QAction(tr("&Bone"), this);
  _NoneAct = new QAction(tr("&None"), this);

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

  _NoneAct->setShortcut(tr("Ctrl+N"));
  _viewBoneAct->setShortcut(tr("Ctrl+N"));
  _viewMaterialAct->setShortcut(tr("Ctrl+N"));
  _viewWFAct->setShortcut(tr("Ctrl+W"));
  _openAct->setShortcut(tr("Ctrl+O"));

  connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));
  connect(_setModelTest, SIGNAL(triggered()), this, SLOT(setingModel()));
  connect(_viewWFAct, SIGNAL(triggered()), this, SLOT(wireFrameView()));
  connect(_viewMaterialAct, SIGNAL(triggered()), this, SLOT(materialView()));
  connect(_viewBoneAct, SIGNAL(triggered()), this, SLOT(boneView()));
  connect(_NoneAct, SIGNAL(triggered()), this, SLOT(normalView()));
  connect(_points, SIGNAL(triggered()), this, SLOT(pointViewModel()));
  connect(_lines, SIGNAL(triggered()), this, SLOT(linesViewModel()));
  connect(_line_strip, SIGNAL(triggered()), this, SLOT(line_stripViewModel()));
  connect(_line_loop, SIGNAL(triggered()), this, SLOT(line_loopViewModel()));
  connect(_triangle, SIGNAL(triggered()), this, SLOT(triangleViewModel()));
  connect(_triangle_strip, SIGNAL(triggered()), this, SLOT(triangle_stripViewModel()));
  connect(_triangle_fun, SIGNAL(triggered()), this, SLOT(triangle_funViewModel()));
  connect(_quads, SIGNAL(triggered()), this, SLOT(quadsViewModel()));
  connect(_quad_strip, SIGNAL(triggered()), this, SLOT(quad_stripViewModel()));
  connect(_polygon, SIGNAL(triggered()), this, SLOT(polygonViewModel()));



 // connect(textEdit, SIGNAL(copyAvailable(bool)),
   //   cutAct, SLOT(setEnabled(bool)));
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::createMenus()
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
    _modelMenu->addAction(_NoneAct);     
    _modelMenu->addSeparator();
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

// TODO:  zrobiæ jedn¹ funkcje do tego. - wybor opcjii.
//--------------------------------------------------------------------------------------------------
void ToolboxMain::normalView()
{
    _pScene->addVieModelFlag(MODEL_FLAG::MODEL);
    _model->updateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::boneView()
{
    _pScene->addVieModelFlag(MODEL_FLAG::BONE);
    _model->updateMesh();
}
//--------------------------------------------------------------------------------------------------
void ToolboxMain::wireFrameView()
{
    _pScene->addVieModelFlag(MODEL_FLAG::WIREFRAME);
    _model->updateMesh();
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::materialView()
{
    _pScene->addVieModelFlag(MODEL_FLAG::MATERIAL);
    _model->updateMesh();
}

//--------------------------------------------------------------------------------------------------
// loads model
void ToolboxMain::open()
{
	// TODO:
	// tymczasowe rozwiazanie - powinno sie raczej dac mozliwosc ladowania wiekszej ilosci modeli,
	const QString fileName = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath());

	if (!fileName.isEmpty()) 
	{
		std::cout << " " << std::endl; 
		std::cout << "File Opened: " << std::string(fileName.toUtf8()) << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl; 

		CModelWithSkeleton* model = new CModelWithSkeleton(std::string(fileName.toUtf8()));

	//	if (model->isValid() && model->getRoot().valid())
	//	{
			osg::ref_ptr<osg::Group> group = model->getRoot();

			// test
			model->updateMesh();
			//model->updateSkeletonMesh();

			// create group being root of scene
			osg::ref_ptr<osg::Group> root = new osg::Group();
			root->setName("root");
			// create grid
			root->addChild(createGrid());
			// add model
			root->addChild(group);	

			// add to control
			_osgView->setSceneData(root);

			// add model to service
			ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 

            _pScene = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
			_pScene->addModel(model);

			// manage scene
			osgViewer::Scene* scene = _osgView->getScene(); 
			_osgControlWidget->setScene(scene); 

            _gridWidget->setScene(scene);

			_timeLine->setScene(scene); 

			int pluginCount = _plugins.size(); 
			for (int i=0; i<pluginCount; ++i)
			{
				QObject *plugin = _plugins[i]; 
				IControlPlugin *iControlPlugin = qobject_cast<IControlPlugin*>(plugin);
				if(iControlPlugin)
					iControlPlugin->setScene(scene); 
			}

            _model = dynamic_cast<CModelWithSkeleton*>(_pScene->getModel());  // zawsze mamy tylko jeden model.

            _pScene->setViewModelFlag(MODEL_FLAG::MODEL | MODEL_FLAG::MATERIAL);
            _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLES);

			return;
	//	}

		if (model)
			delete model;
	}

	QMessageBox::warning(this, QString("Error!"), QString("Failed to load model."));
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::initializeConsoleWidget()
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
void ToolboxMain::initializeOGSWidget()
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
	root->addChild(createGrid());

	_osgView->setSceneData(root);

	// osgViewer::Scene* scene = _osgView->getScene(); 
	// _osgControlWidget->setScene(scene); 
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Node> ToolboxMain::createGrid()
{
	osg::ref_ptr<osg::Geode>		geode		= new osg::Geode();
	// create Geometry object to store all the vertices and lines primitive.
	osg::ref_ptr<osg::Geometry>	linesGeom	= new osg::Geometry();

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

const QString ToolboxMain::_settingsOrganizationName = QString("Artifex Mundi");
const QString ToolboxMain::_settingsApplicationName = QString("Toolbox");

//--------------------------------------------------------------------------------------------------
void ToolboxMain::InitializeCoreServices()
{
    ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
    AnimationService* pAnimation = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
    pAnimation->SetScene(_osgView->getScene()); 
}

//--------------------------------------------------------------------------------------------------
void ToolboxMain::setingModel()
{
    ObjectService* object = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
    if(_model2)
    {
        object->addModel(_model2);

        osg::ref_ptr<osg::Group> group = _model2->getRoot();



        // create group being root of scene
        osg::ref_ptr<osg::Group> root = new osg::Group();
        root->setName("root");
        // create grid
        root->addChild(createGrid());
        // add model
        root->addChild(group);	

        // add to control
        _osgView->setSceneData(root);

        // manage scene
        osgViewer::Scene* scene = _osgView->getScene(); 
        _osgControlWidget->setScene(scene); 

        _gridWidget->setScene(scene);

        _timeLine->setScene(scene); 


        _model2->updateMesh();
        _model = (CModelWithSkeleton*)_model2;
        return;
    }

    _model2 = object->getModel();
}