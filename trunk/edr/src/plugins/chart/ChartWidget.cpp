#include "ChartPCH.h"

#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/MultiView.h>
#include <core/MultiViewWidgetItem.h>
#include <core/OsgSceneDump.h>

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
    viewer = new core::QOsgDefaultWidget(this);
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


	
    viewer->addEventHandler( new core::OsgSceneDumpFileHandler("dump_chart.txt") );
	viewer->addEventHandler( new osgWidget::MouseHandler(multiView) );
	viewer->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    //viewer->addEventHandler( new osgViewer::WindowSizeHandler );
    viewer->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie do widgeta
	mainChart->addWidget( viewer );
    //multiView->addChild(new osgWidget::Box("osgWidgetHACK"));



	
}

ChartWidget::~ChartWidget() 
{

}




// void ChartWidget::deprecated_addChart(C3DChartData* c3dData){
// 
// 	
// 	if(c3dData->getRNumber()>0){
// 	item.push_back(new Chart(40,40,500,250));
// 	itItem= item.end()-1;
// 	(*itItem)->deprecated_addChartSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
//     (*itItem)->setShowLabel(false);
//     (*itItem)->setShowBorder(false);
// 	multiView->addChild(*itItem);
// 	previewItem.push_back(new Chart(40,40,500,250));
// 	itPItem= previewItem.end()-1;
// 	(*itPItem)->deprecated_addChartPreviewSeries(c3dData,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
// 	multiView->addChild((*itPItem));
// 	multiView->addItem(new core::MultiViewChartItem(*itItem,multiView),new core::MultiViewChartItem((*itPItem),multiView));
// 	}
// }



void ChartWidget::addChart( const core::ScalarChannelPtr& channel )
{
    Chart* chart = new Chart(40, 40, 500, 250);
    item.push_back( chart );
    chart->addChannel(channel, osg::Vec4(0.0f,1.0f,0.0f,1.0f));
    chart->setShowLabel(false);
    chart->setShowBorder(false);
    multiView->addChild(chart);

    Chart* preview = new Chart(40, 40, 500, 250);
    previewItem.push_back(preview);
    preview->addChannel(channel, osg::Vec4(0.0f,1.0f,0.0f,1.0f));
    multiView->addChild(preview);

    multiView->addItem(new core::MultiViewChartItem(chart, multiView), new core::MultiViewChartItem(preview, multiView));
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
// 	if(item.size()>0){
// 	itItem=item.begin();
// 	return (double)(*itItem)->deprecated_getFrameNumber()/(double)(*itItem)->deprecated_getFPS();}
// 	else
// 		return 0;
    if ( item.size() ) {
        return item.front()->getLength();
    } else {
        return 0;
    }
}
void ChartWidget::clear(){
	multiView->removeAllItems();
	itItem=item.begin();
	itPItem=previewItem.begin();

	for(; itItem < item.end(); itItem++,itPItem++){
	multiView->removeChild(multiView->getChildIndex((*itItem)));
	multiView->removeChild(multiView->getChildIndex((*itPItem)));
	}
	item.clear();
	previewItem.clear();
	//	multiView->addChild(new osgWidget::Box("osgWidgetHACK"));
}