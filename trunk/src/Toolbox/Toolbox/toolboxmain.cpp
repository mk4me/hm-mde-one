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

#define CONFIG_FILE "Toolbox_config.ini"

ToolboxMain::ToolboxMain(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::ToolboxMain)
{
    ui->setupUi(this);

    // Console Widget 
    initializeConsoleWidget();
    // Control Widget + TimeLine
    initializeControlWidget();

    // Wczytuje plik konfiguracyjny
    loadConfiguration(); 

    plugins_.clear(); 
    loadPlugins(); 

    // w przysz³oœci setLayout
    QDockWidget *cdock = dynamic_cast<QDockWidget *>(consoleWidget_->parent()); 
    QDockWidget *tldock = dynamic_cast<QDockWidget *>(timeLine_->parent()); 

    if(cdock && tldock)
      tabifyDockWidget(cdock, tldock); 
}

ToolboxMain::~ToolboxMain()
{
  delete ui;
  std::streambuf *buf = std::cout.rdbuf(streambuf_);
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
  osgControlWidget_ = new OsgControlWidget();    
  addDockWidget(Qt::RightDockWidgetArea, dock);
  dock->setWidget((QWidget*)osgControlWidget_);

  osgViewer::Scene* scene = osgView_->getScene(); 
  osgControlWidget_->setScene(scene); 

  // TimeLine
  QDockWidget *tldock = new QDockWidget(tr("Time Line"), this, Qt::WindowTitleHint); 
  tldock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  timeLine_ = new TimeLine(); 
  addDockWidget(Qt::BottomDockWidgetArea, tldock);
  tldock->setWidget((QWidget*)timeLine_); 

  timeLine_->setScene(scene); 

  connect(&osgView_->_timer, SIGNAL(timeout()), timeLine_, SLOT(update())); 

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
    osgViewer::Scene* scene = osgView_->getScene(); 

    plugins_.push_back(plugin); 

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
  openAct_ = new QAction(tr("&Open..."), this);
  openAct_->setShortcut(tr("Ctrl+O"));
  connect(openAct_, SIGNAL(triggered()), this, SLOT(open()));
}

void ToolboxMain::createMenus()
{
  fileMenu_ = menuBar()->addMenu(tr("&File"));
  fileMenu_->addAction(openAct_);
}

void ToolboxMain::open()
{
  const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
  
  if (!fileName.isEmpty()) 
  {
    std::cout << " " << std::endl; 
    std::cout << "File Opened: " << std::string(fileName.toUtf8()) << std::endl;
    std::cout << "---------------------------------------------------------------" << std::endl; 
    // 
    osg::Node *loadedModel = osgDB::readNodeFile( std::string(fileName.toUtf8()) );
    osg::Node *grid = createGrid(); 

    if(loadedModel->asGroup())
      loadedModel->asGroup()->addChild(grid); 

    osgView_->setSceneData( loadedModel );

    osgViewer::Scene* scene = osgView_->getScene(); 
    osgControlWidget_->setScene(scene); 
    timeLine_->setScene(scene); 

    int pluginCount = plugins_.size(); 
    for (int i=0; i<pluginCount; ++i)
    {
      QObject *plugin = plugins_[i]; 
      IControlPlugin *iControlPlugin = qobject_cast<IControlPlugin *>(plugin);
      if(iControlPlugin)
      {
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
  consoleWidget_ = new ConsoleWidget();    
  addDockWidget(Qt::BottomDockWidgetArea, dock);
  dock->setWidget((QWidget*)consoleWidget_); 

  // Inicjalizacja konsoli
  ConsoleWidgetOutputBuffer *ob = new ConsoleWidgetOutputBuffer(consoleWidget_, 256);
  streambuf_ = std::cout.rdbuf(ob);

  std::cout << "Console initialized..." << std::endl; 
}

void ToolboxMain::initializeOGSWidget()
{
  osgView_ = new ViewerQT(this, 0, 0);
  setCentralWidget(osgView_); 
  osgView_->addEventHandler(new osgViewer::StatsHandler);
  
  osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator(); 
  osgView_->setCameraManipulator(cameraManipulator);

  //cameraManipulator->cente

  osg::Group *root = new osg::Group(); 
  root->addChild(createGrid());
  osgView_->setSceneData( root );

  //osgViewer::Scene* scene = osgView_->getScene(); 
  //osgControlWidget_->setScene(scene); 
}

osg::Node *ToolboxMain::createGrid()
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
  return (osg::Node*)geode; 
}