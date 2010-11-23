#include <fstream>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osg/Camera>
#include "ChartService.h"

//! Funkcja tworzaca kamere do widoku 2d
//osg::Camera* createHUD()
//{
//
//	osg::Camera* camera = new osg::Camera;
//	camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280,0,1024));
//	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//	camera->setViewMatrix(osg::Matrix::identity());
//	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
//	camera->setRenderOrder(osg::Camera::POST_RENDER);
//	camera->setAllowEventFocus(false);
//
//	return camera;
//}


//void createEMGChanels(std::string c3dFile,osg::Camera* cam){
//int chartCount=16;
//float x=1280/4;
//float y=1024/4;
//	LineChart **chart= new LineChart*[chartCount];
//	ChartDecoration **cSys= new ChartDecoration*[chartCount];
//	ChartData **data= new ChartData*[chartCount];
//	for(int i=1;i<=4;i++){
//		for(int j=1;j<=4;j++){
//				cSys[i*j]=new ChartDecoration(osg::Vec3(x*i-x+60,y*j-y+60,0),x-120,y-120);
//				cam->addChild(cSys[i*j]);
//				data[i*j]=new ChartData(c3dFile,i*j+11);
//				chart[i*j]=new LineChart(data[i*j],cSys[i*j]);
//				cam->addChild(chart[i*j]);
//		}
//	}
//
//
//
//
//}
//void createGRFChanels(std::string c3dFile,osg::Camera* cam){
//	int chartCount=12;
//	float x=1280/3;
//	float y=1024/4;
//	LineChart **chart= new LineChart*[chartCount];
//	ChartDecoration **cSys= new ChartDecoration*[chartCount];
//	ChartData **data= new ChartData*[chartCount];
//	for(int i=1;i<=3;i++){
//		for(int j=1;j<=4;j++){
//				cSys[i*j]=new ChartDecoration(osg::Vec3(x*i-x+60,y*j-y+60,0),x-120,y-120);
//				cam->addChild(cSys[i*j]);
//				data[i*j]=new ChartData(c3dFile,i*j-1);
//				chart[i*j]=new LineChart(data[i*j],cSys[i*j]);
//				cam->addChild(chart[i*j]);
//		}
//	}
//
//
//
//}
int main( int argc, char **argv )
{

//	osg::ArgumentParser arguments(&argc,argv);
//
//
////	osg::Camera* cam=createHUD();
//	
//	osg::Vec4 background=osg::Vec4(255.0f/255.0f,252.0f/255.0f,238.0f/255.0f,1);
//	int chartCount =0,actualChart=0,cSystemCount=0,actualCSys=0;
//	for (int i=1;i<arguments.argc();i++) {
//	/*	if(arguments.match(i,"-f")){
//			chartCount++;
//		}
//		if(arguments.match(i,"-c")){
//			cSystemCount++;
//		}*/
//		if(arguments.match(i,"-EMG")){
//			createEMGChanels(arguments[i+1],cam);
//		}
//		if(arguments.match(i,"-GRF")){
//			createGRFChanels(arguments[i+1],cam);
//		}
//	}
////osg::Vec3Array** chartVertices=new osg::Vec3Array* ;
//	//LineChart **chart= new LineChart*[chartCount];
//	//ChartDecoration **cSys= new ChartDecoration*[cSystemCount];
//	//ChartData **data= new ChartData*[chartCount];
//	//for (int i=1;i<arguments.argc();i++) {
		//if(arguments.match(i,"-c")){
		//	if (arguments.isNumber(i+5)){
		//		cSys[actualCSys]=new ChartDecoration(
		//			osg::Vec3(atof(arguments[i+1]),atof(arguments[i+2]),0),atof(arguments[i+3]),
		//			atof(arguments[i+4]),osg::Vec4(atof(arguments[i+5]),atof(arguments[i+6]),
		//			atof(arguments[i+7]),atof(arguments[i+8])));
		//		cam->addChild(cSys[actualCSys]);
		//		actualCSys++;
		//		i+=8;
		//	}else{
		//		cSys[actualCSys]=new ChartDecoration(
		//			osg::Vec3(atof(arguments[i+1]),atof(arguments[i+2]),0),atof(arguments[i+3]),
		//			atof(arguments[i+4]));
		//		cam->addChild(cSys[actualCSys]);
		//		actualCSys++;
		//		i+=4;
		//	}
		//}
		//else if(arguments.match(i,"-f")){//pobranie danych z pliku ascii

		//	data[actualChart]=new ChartData(arguments[i+1],atoi(arguments[i+2]));
		//	chart[actualChart]=new LineChart(data[actualChart],cSys[actualCSys-1]);
		//	actualChart++;
		//}

		//else if(arguments.match(i,"--color")){

		//	chart[actualChart-1]->setColor(atof(arguments[i+1]),atof(arguments[i+2]),atof(arguments[i+3]),atof(arguments[i+4]));

		//	i+=4;
		//}
		//else if(arguments.match(i,"-b")){

		//	background=osg::Vec4(atof(arguments[i+1]),atof(arguments[i+2]),atof(arguments[i+3]),atof(arguments[i+4]));

		//	i+=4;
//	//	}
//	//}
//	//for(int i=0;i<chartCount;i++){
//	//	//chart[i]->drawChart();
//	//	cam->addChild(chart[i]);
//	//}
//	osgViewer::Viewer viewer;
//
//	viewer.getCamera()->setClearColor(background);
//	viewer.setUpViewInWindow( 10, 10, 640, 480 );
//	viewer.setSceneData(cam);
//	viewer.run();
	//ChartService* service=new ChartService();
	return 0;
}
