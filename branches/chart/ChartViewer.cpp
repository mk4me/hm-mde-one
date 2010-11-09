#include "ChartViewer.h"
#include "ChartWidget.h"
osg::Camera* createCamera()
{

	osg::Camera* camera = new osg::Camera;
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280,0,1024));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(false);

	return camera;
}
ChartViewer::ChartViewer(ChartWidget* chartWidget,int index,std::string c3dFile){
	this->index=index;
	this->c3dFile=c3dFile;
	this->chartWidget=chartWidget;
	osg::Vec4 background=osg::Vec4(255.0f/255.0f,252.0f/255.0f,238.0f/255.0f,1);
	 cam=createCamera();
	data= new ChartData(c3dFile,index);
	cSys= new ChartDecoration(osg::Vec3(70,70,0),1140,884);
	chart=new LineChart(data,cSys);
	cam->addChild(cSys);
	cam->addChild(chart);
	getCamera()->setClearColor(background);
	setSceneData(cam);
	setMinimumSize(300,100);
			
			 getCamera()->setGraphicsContext(getGraphicsWindow());
	        setThreadingModel(osgViewer::Viewer::SingleThreaded);
			connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
			_timer.start(34);
}

ChartViewer::~ChartViewer(){


}

void ChartViewer::mouseDoubleClickEvent(QMouseEvent* event){
	chartWidget->createLargeChart(index);

}
void ChartViewer::changeView(int index){
data= new ChartData(c3dFile,index);
cSys= new ChartDecoration(osg::Vec3(70,70,0),1140,884);
	chart=new LineChart(data,cSys);
	cam=createCamera();
	cam->addChild(cSys);
	cam->addChild(chart);
	setSceneData(cam);

}
LineChart* ChartViewer::getChart(){

	return chart;
}
ChartData* ChartViewer::getData(){
return data;
}