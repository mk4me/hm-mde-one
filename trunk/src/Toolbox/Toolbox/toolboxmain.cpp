// Go to http://dmoulding.googlepages.com/vld 
// for the Visual Leak Detector for VisualStudio

//#include <vld.h>        
#include "toolboxmain.h"
#include "UI/ui_toolboxmain.h"
#include "UI/ui_OsgWidget.h"
#include "OsgControlWidget.h"
#include "QOSGWidget.h"
#include "AdapterWidget.h"
#include "TimeLine.h"
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

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define CONFIG_FILE "Toolbox_config.ini"

ToolboxMain::ToolboxMain(QWidget *parent)
  : QMainWindow(parent), m_ui(new Ui::ToolboxMain)
{
    m_ui->setupUi(this);

    // Console Widget 
    initializeConsoleWidget();
    // Control Widget + TimeLine
    initializeControlWidget();

    // Wczytuje plik konfiguracyjny
    loadConfiguration(); 

    m_plugins.clear(); 
    loadPlugins(); 

    // w przysz³oœci setLayout
    QDockWidget *cdock = dynamic_cast<QDockWidget *>(m_consoleWidget->parent()); 
    QDockWidget *tldock = dynamic_cast<QDockWidget *>(m_timeLine->parent()); 

    if(cdock && tldock)
      tabifyDockWidget(cdock, tldock); 
}

ToolboxMain::~ToolboxMain()
{
  delete m_ui;
  std::streambuf *buf = std::cout.rdbuf(m_streambuf);
  delete buf; 
}

void ToolboxMain::loadConfiguration()
{
  /*
  QString path = qApp->applicationDirPath();
  path.append("/"); 
  path.append(CONFIG_FILE); 
  ConfigurationService *config = new ConfigurationService(); 
  config->loadConfiguration( std::string(path.toUtf8()) ); /**/
}

void ToolboxMain::initializeControlWidget()
{
  // MainWidget 
  initializeOGSWidget();

  // ControlWidget
  QDockWidget *dock = new QDockWidget(tr("Control (sample)"), this, Qt::WindowTitleHint);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  m_osgControlWidget = new OsgControlWidget();    
  addDockWidget(Qt::RightDockWidgetArea, dock);
  dock->setWidget((QWidget*)m_osgControlWidget);

  osgViewer::Scene* scene = m_osgView->getScene(); 
  m_osgControlWidget->setScene(scene); 

  // TimeLine
  QDockWidget *tldock = new QDockWidget(tr("Time Line"), this, Qt::WindowTitleHint); 
  tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  m_timeLine = new TimeLine(); 
  addDockWidget(Qt::BottomDockWidgetArea, tldock);
  tldock->setWidget((QWidget*)m_timeLine); 

  m_timeLine->setScene(scene); 

  connect(&m_osgView->_timer, SIGNAL(timeout()), m_timeLine, SLOT(update())); 

  // Inicjalizacja menu: 
  createActions(); 
  createMenus(); 
                    
  //osgView_->get
}

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

void ToolboxMain::initializePlugin(QObject *plugin)
{
  IControlPlugin *iControlPlugin = qobject_cast<IControlPlugin *>(plugin);
  if (iControlPlugin)
  {
    osgViewer::Scene* scene = m_osgView->getScene(); 

    m_plugins.push_back(plugin); 

    QDockWidget *tldock = new QDockWidget(iControlPlugin->getPluginName(), this, Qt::WindowTitleHint); 
    tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, tldock);
    tldock->setWidget(iControlPlugin->getDockWidget(0)); 

    iControlPlugin->setScene(scene); 
  }
}

// To ma byæ w pluginach 
void ToolboxMain::createActions()
{
  m_openAct = new QAction(tr("&Open..."), this);
  m_openAct->setShortcut(tr("Ctrl+O"));
  connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));
}

void ToolboxMain::createMenus()
{
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_openAct);
}

//////////////////////////////////////////////////////////////////////////
// draws bone
void ToolboxMain::drawBone(pPat bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode)
{
	osg::Vec3d bpos = (*parentRot) * bone->getPosition() + *parentPos;
	osg::Quat  brot = bone->getAttitude() * (*parentRot);

	// recursively draw other bones
	for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
		drawBone((pPat)bone->getChild(b), &bpos, &brot, geode);

	// draw actual bone
	osg::Geometry*  geometry = new osg::Geometry();

	// vertices
	osg::Vec3Array* vertices = new osg::Vec3Array();
	vertices->push_back(*parentPos);
	vertices->push_back(bpos);

	// indices
	osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	line->push_back(0);
	line->push_back(1);
	
	// set geometry data
	geometry->setVertexArray(vertices);
	geometry->addPrimitiveSet(line);

	// set colors
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
	colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
	colorIndexArray->push_back(0);
	colorIndexArray->push_back(1);
	geometry->setColorArray(colors);
	geometry->setColorIndices(colorIndexArray);
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	// add to geode
	geode->addDrawable(geometry);
}

//////////////////////////////////////////////////////////////////////////
// draws skeleton
void ToolboxMain::drawSkeletonFromGroup(osg::Group* root)
{
	int childNum = root->getNumChildren();
	for (int i = 0; i < childNum; ++i)
	{
		// find skeleton
		if (!root->getChild(i)->getName().compare("skeleton"))
		{
			pPat root_bone = (pPat)(root->getChild(i)->asGroup()->getChild(0));

			osg::Geode* geode = new osg::Geode();
			geode->setName("skeleton_geode");

			// recursively draw skeleton
			for (unsigned int b = 0; b < root_bone->getNumChildren(); ++b)
				drawBone((pPat)root_bone->getChild(b), &root_bone->getPosition(), &root_bone->getAttitude(), geode);
			
			root->addChild(geode);

			// we did what we want
			return;
		}
	}

	MessageBox(0, L"No skeleton?", L"info", MB_OK);
}

//////////////////////////////////////////////////////////////////////////
// loads model
void ToolboxMain::open()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

	if (!fileName.isEmpty()) 
	{
		std::cout << " " << std::endl; 
		std::cout << "File Opened: " << std::string(fileName.toUtf8()) << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl; 
		
		// calls external function loading model <- look into osgDBPlugin
		osg::Node *loadedModel = osgDB::readNodeFile( std::string(fileName.toUtf8()) );
	
		if (loadedModel)
		{
			drawSkeletonFromGroup(loadedModel->asGroup());

			m_root->addChild(loadedModel);
			m_osgView->setSceneData(m_root);

			osgViewer::Scene* scene = m_osgView->getScene(); 
			m_osgControlWidget->setScene(scene); 
			m_timeLine->setScene(scene); 

			int pluginCount = m_plugins.size(); 
			for (int i=0; i<pluginCount; ++i)
			{
				QObject *plugin = m_plugins[i]; 
				IControlPlugin *iControlPlugin = qobject_cast<IControlPlugin*>(plugin);
				if(iControlPlugin)
					iControlPlugin->setScene(scene); 
			}
		}
	}
}

void ToolboxMain::initializeConsoleWidget()
{
  QDockWidget *dock; 
  dock = new QDockWidget(tr("Console"), this, Qt::WindowTitleHint);
  dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  m_consoleWidget = new ConsoleWidget();    
  addDockWidget(Qt::BottomDockWidgetArea, dock);
  dock->setWidget((QWidget*)m_consoleWidget); 

  // Inicjalizacja konsoli
  ConsoleWidgetOutputBuffer *ob = new ConsoleWidgetOutputBuffer(m_consoleWidget, 256);
  m_streambuf = std::cout.rdbuf(ob);

  std::cout << "Console initialized..." << std::endl; 
}

void ToolboxMain::initializeOGSWidget()
{
	m_osgView = new ViewerQT(this, 0, 0);
	setCentralWidget(m_osgView); 
	m_osgView->addEventHandler(new osgViewer::StatsHandler);

	osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator(); 
	m_osgView->setCameraManipulator(cameraManipulator);

	m_root = new osg::Group();
	m_root->setName("root");
	m_root->addChild(createGrid());
	m_osgView->setSceneData( m_root );

	// osgViewer::Scene* scene = m_osgView->getScene(); 
	// m_osgControlWidget->setScene(scene); 
}

osg::Node* ToolboxMain::createGrid()
{
	osg::Geode* geode = new osg::Geode();
	// create Geometry object to store all the vertices and lines primitive.
	osg::Geometry* linesGeom = new osg::Geometry();

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

	return (osg::Node*)geode; 
}