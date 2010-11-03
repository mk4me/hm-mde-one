
#include "ChartWidget.h"



ChartWidget::ChartWidget(ChartService* service)
:   service(service)
{
    setupUi(this);

	c3dFile="Przejscie1.c3d";
	//createLargeChart(0);
	createEMGChanels();
	mainViewer=new ChartViewer(this,0,c3dFile);
	mainViewer->setMinimumSize(300,300);
	mainChart->addWidget(mainViewer);
}

ChartWidget::~ChartWidget()
{

}

void ChartWidget::createLargeChart(int chartIndex){
	mainViewer->~ChartViewer();
	mainViewer=new ChartViewer(this,chartIndex,c3dFile);
	mainViewer->setMinimumSize(300,300);
	mainChart->addWidget(mainViewer);


}
//void ChartWidget::createChart(QOSGViewer* viewer, int chartIndex){
//	osg::Vec4 background=osg::Vec4(255.0f/255.0f,252.0f/255.0f,238.0f/255.0f,1);
//	osg::Camera* cam=createHUD();
//	ChartData* data= new ChartData(c3dFile,chartIndex);
//	ChartDecoration* cSys= new ChartDecoration(osg::Vec3(70,70,0),1140,884);
//	LineChart* chart=new LineChart(data,cSys);
//	cam->addChild(cSys);
//	cam->addChild(chart);
//	viewer->getCamera()->setClearColor(background);
//	viewer->setSceneData(cam);
//	viewer->setMinimumSize(300,100);
//	
//
//}
void ChartWidget::createEMGChanels(){
	listViewer=new ChartViewer*[16];
	for(int i=0;i<16;i++){
		listViewer[i] = new ChartViewer(this,i+12,c3dFile);
		//createChart(listViewer[i],i+12);
		allChart->addWidget( listViewer[i] );
	}
}
void ChartWidget::createGRFChanels(){
	listViewer=new ChartViewer*[12];
	for(int i=0;i<12;i++){
		listViewer[i] = new ChartViewer(this,i,c3dFile);
		//createChart(listViewer[i],i);
		allChart->addWidget( listViewer[i] );
	}
	 
}

void ChartWidget::comboBoxChanged(int index){
	switch(index){
		case 0: {
			for(int i = 0; i < 12; i++) 
				delete[] listViewer[i]; 
				delete[] listViewer;
				createEMGChanels();
				break;
			 }
		case 1: {
			for(int i = 0; i < 16; i++) 
				delete[] listViewer[i]; 
				delete[] listViewer;
				createGRFChanels();
				break;
			 }
	}
}
