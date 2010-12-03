#include "ChartPCH.h"

#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/MultiView.h>
#include <core/MultiViewWidgetItem.h>
#include "C3DChartData.h"
#include <core/Chart.h>
#include "MultiViewChartItem.h"
#ifdef _DEBUG
#define WM_FLAGS osgWidget::WindowManager::WM_PICK_DEBUG
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
	multiView->addChild(new osgWidget::Box("osgWidgetHACK"));
	 // stworzenie kamery
    osg::Camera* multiViewCamera = multiView->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(multiViewCamera);
    viewer->setSceneData(root);


	

	viewer->addEventHandler( new osgWidget::MouseHandler(multiView) );
//   viewer->addEventHandler( new osgUI::StaticKeyboardHandler(multiView) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    //viewer->addEventHandler( new osgViewer::WindowSizeHandler );
    viewer->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie do widgeta
	mainChart->addWidget( viewer );
    multiView->addChild(new osgWidget::Box("osgWidgetHACK"));


//	mainChart->addWidget(mainViewer);
	
}

ChartWidget::~ChartWidget() 
{

}




void ChartWidget::addChart(int index){
	
	C3DChartData* c3dData=new C3DChartData("Przejscie1.c3d",index);
	if(c3dData->getRNumber()>0){
	item.push_back(new Chart(40,40,500,250));
	 itItem= item.end()-1;
	(*itItem)->addChartSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	(*itItem)->setXNumReps(2);
	(*itItem)->setYNumReps(2);
	(*itItem)->setBorderSize(5);
	(*itItem)->setLabelVisable(false);
	multiView->addChild(*itItem);
	previewItem.push_back(new Chart(40,40,500,250));
	itPItem= previewItem.end()-1;
	(*itPItem)->	addChartSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	multiView->addChild((*itPItem));
	multiView->addItem(new core::MultiViewChartItem(*itItem),new core::MultiViewChartItem((*itPItem)));
	}
}

void ChartWidget::update(double targetTime){
	if(item.size()>0){
	itItem=item.begin();
	itPItem=previewItem.begin();

	for(; itItem < item.end(); itItem++,itPItem++){
	  (*itItem)->updatePointer(targetTime);
	  (*itPItem)->updatePointer(targetTime);
	}
	}
}

double ChartWidget::getLenght(){
	if(item.size()>0){
	itItem=item.begin();
	return (*itItem)->getFrameNumber()/(*itItem)->getFPS();}
	else
		return 0;
}