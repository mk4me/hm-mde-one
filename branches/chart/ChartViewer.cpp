#include "ChartViewer.h"
#include "ChartWidget.h"
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
ChartViewer::ChartViewer(ChartWidget* chartWidget,int index,std::string c3dFile){
	chartCount=1;
	cSysCount=1;
	this->index=index;
	this->c3dFile=c3dFile;
	this->chartWidget=chartWidget;
	osg::Vec4 background=osg::Vec4(255.0f/255.0f,252.0f/255.0f,238.0f/255.0f,1);
	cam=createCamera();
	cSys= new ChartDecoration*[cSysCount];
	data= new ChartData*[chartCount];
	chart= new LineChart*[chartCount];
	data[0]= new ChartData(c3dFile,index);
	cSys[0]= new ChartDecoration(osg::Vec3(20,20,0),500,250,data[0]);
	chart[0]=new LineChart(data[0],cSys[0]);
	
	cam->addChild(cSys[0]);
	cam->addChild(chart[0]);
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
void ChartViewer::resizeEvent(QResizeEvent* event){
	int w = this->width();
	int h = this->height();
cam->setProjectionMatrix(osg::Matrix::ortho2D(0,w,0,h));
this->getGraphicsWindow()->resized(0,0,w,h);
}
void ChartViewer::changeView(int index){
//data= new ChartData(c3dFile,index);
//cSys= new ChartDecoration(osg::Vec3(10,10,0),500,250);
//	chart=new LineChart(data,cSys);
//	cam=createCamera();
//	cam->addChild(cSys);
//	cam->addChild(chart);
//	setSceneData(cam);

}
LineChart* ChartViewer::getChart(int index){

	return chart[index];
}
ChartData* ChartViewer::getData(int index){
return data[index];
}
ChartDecoration* ChartViewer::getDecoration(int index){
return cSys[index];
}
void ChartViewer::addCSys(int index){
	ChartData** newData= new ChartData*[chartCount+1];
	ChartDecoration** newCSys= new ChartDecoration*[cSysCount+1];
	LineChart** newChart=new LineChart*[chartCount+1];
	for(int i=0;i<chartCount;i++){
		newData[i]=data[i];
		newChart[i]=chart[i];
	}
	for(int i=0;i<chartCount;i++){
		newCSys[i]=cSys[i];

	}
	newData[chartCount]= new ChartData(c3dFile,index);
	newCSys[cSysCount]= new ChartDecoration(osg::Vec3(540,20,0),500,250,newData[chartCount]);
	newChart[chartCount]=new LineChart(newData[chartCount],newCSys[cSysCount]);
	cam->addChild(newCSys[cSysCount]);
	cam->addChild(newChart[chartCount]);

	data=newData;
	cSys=newCSys;
	chart=newChart;
	chartCount++;
	cSysCount++;
}

int ChartViewer::getChartCount(){
return chartCount;
}
void ChartViewer::setChartCount(int chartCount){
	this->chartCount=chartCount;
}
int ChartViewer::getCSysCount(){
return cSysCount;
}
void ChartViewer::setCSysCount(int cSysCount){
	this->cSysCount=cSysCount;
}