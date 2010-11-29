#include "ChartPCH.h"

#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/MultiView.h>
#include <core/MultiViewWidgetItem.h>
#include "C3DChartData.h"
#include <core/Chart.h>
#include "MultiViewChartItem.h"
#ifdef _DEBUG
#define WM_FLAGS 0// osgWidget::WindowManager::WM_PICK_DEBUG
#else
#define WM_FLAGS 0
#endif

ChartWidget::ChartWidget(IService* service)
:   service(service)
{
    setupUi(this);
	viewer = new QOSGViewer(this, "OsgViewer");
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

	
   	multiView = new core::MultiView(viewer, traits->width, traits->height, 0xF0000000, WM_FLAGS);
	 // stworzenie kamery
    osg::Camera* multiViewCamera = multiView->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(multiViewCamera);
    viewer->setSceneData(root);
	mainViewer=new ChartViewer(this,0);


	

	viewer->addEventHandler( new osgWidget::MouseHandler(multiView) );
//   viewer->addEventHandler( new osgUI::StaticKeyboardHandler(multiView) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    //viewer->addEventHandler( new osgViewer::WindowSizeHandler );
    viewer->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie do widgeta
	mainChart->addWidget( viewer );



//	mainChart->addWidget(mainViewer);
	
}

ChartWidget::~ChartWidget() 
{

}

void ChartWidget::createLargeChart(int chartIndex){
//mainViewer->changeView(chartIndex);
	//mainViewer=new ChartViewer(this,chartIndex,c3dFile);
	////mainViewer->setMinimumSize(300,300);
	//mainChart->addWidget(mainViewer);



}

void ChartWidget::createEMGChanels(){
	//listViewer=new ChartViewer*[16];
	//for(int i=0;i<16;i++){
	//	listViewer[i] = new ChartViewer(this,i+12,c3dFile);
	//	//createChart(listViewer[i],i+12);
	//	allChart->addWidget( listViewer[i] );
	//}
}
void ChartWidget::createGRFChanels(){
	//listViewer=new ChartViewer*[12];
	//for(int i=0;i<12;i++){
	//	listViewer[i] = new ChartViewer(this,i,c3dFile);
	//	//createChart(listViewer[i],i);
	//	allChart->addWidget( listViewer[i] );
	//}
	 
}

void ChartWidget::comboBoxChanged(int index){
	/*switch(index){
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
	*/
}
ChartViewer* ChartWidget::getViewer(){
return mainViewer;
}

void ChartWidget::addChart(int index){
	C3DChartData* c3dData=new C3DChartData("Przejscie1.c3d",index);
	Chart* chart=new Chart(40,40,500,250);
	chart->	addChartSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	multiView->addChild(chart);
	Chart* chart1=new Chart(40,40,500,250);
	chart1->	addChartSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	multiView->addChild(chart1);
	multiView->addItem(new core::MultiViewChartItem(chart),new core::MultiViewChartItem(chart1));
}