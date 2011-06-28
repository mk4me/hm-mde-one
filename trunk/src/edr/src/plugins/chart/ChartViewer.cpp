#include "ChartViewer.h"
#include "ChartWidget.h"
#include "C3DChartData.h"
#include <core/Chart.h>

osg::Camera* createCamera()
{

	osg::Camera* camera = new osg::Camera;
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0,500,0,300));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(false);

	return camera;
}
ChartViewer::ChartViewer(ChartWidget* chartWidget,int index){

	this->index=index;

	this->chartWidget=chartWidget;
	osg::Vec4 background=osg::Vec4(255.0f/255.0f,252.0f/255.0f,238.0f/255.0f,1);
	cam=createCamera();
	chart=new Chart(40,40,500,250);
	cam->addChild(chart);
	C3DChartData* c3dData=new C3DChartData("Przejscie1.c3d",0);

	chart->	addChartSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	c3dData=new C3DChartData("Przejscie1.c3d",2);
	chart->	addChartSeries(c3dData,osg::Vec4(0.0f,0.0f,1.0f,1.0f));
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


Chart* ChartViewer::getChart(){

	return chart;
}
