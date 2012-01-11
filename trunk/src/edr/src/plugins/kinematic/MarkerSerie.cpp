#include "PCH.h"
#include "MarkerSerie.h"
#include <QtGui/QTableView>

void MarkerSerie::setData( const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(MarkerCollection));
    this->data = data;
    visualizer->actionGhost->setVisible(true);
    connect(visualizer->actionGhost, SIGNAL(triggered(bool)), this, SLOT(showGhost(bool)));
	MarkerCollectionConstPtr markersCollection = data->get();
	scheme = SkeletalVisualizationSchemePtr(new SkeletalVisualizationScheme());
	scheme->setMarkers(markersCollection);
	int markersCount = markersCollection->getNumChannels();
	try {
		scheme->setMarkersDataFromVsk(Vsk::get(static_cast<Vsk::MarkersCount>(markersCount)));
	} catch (...) {}

	//SchemeDrawerContainerPtr markersDrawer = visualizer->drawersByName["Markers"];
	
	markersDrawer->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawMarkers, 3, 0.02f)));
	markersDrawer->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawMarkers, 10, 0.005f)));
    
    trajectoryDrawer = TrajectoryDrawerPtr(new TrajectoryDrawer(osg::Vec4(1, 1, 1, 1), 300));
	markersDrawer->addDrawer(trajectoryDrawer);
	//visualizer->currentDrawer = markersDrawer;
	//visualizer->actionByName["Markers"]->setVisible(true);
	markersDrawer->init(scheme);
    visualizer->trajectoriesDialog->setMarkers(trajectoryDrawer, QString(data->getName().c_str()));
	visualizer->transformNode->addChild(markersDrawer->getNode());
	
}


void MarkerSerie::showGhost( bool visible )
{
    float time = 0.0f;
    if (!ghostNode) {
        ghostNode = new osg::PositionAttitudeTransform();
        MarkerCollectionConstPtr markersCollection = data->get();
        while (time < this->getLength()) {
            SkeletalVisualizationSchemePtr scheme(new SkeletalVisualizationScheme);
            
            scheme->setMarkers(markersCollection);
            try {
                scheme->setMarkersDataFromVsk(Vsk::get(static_cast<Vsk::MarkersCount>(markersCollection->getNumChannels())));
            } catch (...) {}
            scheme->setTime(time);
            osg::Vec4 color(1.0f, 1.0f, 0.9f, 0.25f);
            OsgSchemeDrawerPtr drawer1(new GlPointSchemeDrawer(DrawMarkers, 3, 0.02f, color));
            OsgSchemeDrawerPtr drawer2(new GlLineSchemeDrawer(DrawMarkers, 10, 0.005f, color));
            drawer1->init(scheme);
            drawer2->init(scheme);
            drawer1->update();
            drawer2->update();
            ghostNode->addChild(drawer1->getNode());
            ghostNode->addChild(drawer2->getNode());
            time += 1.0f;
        }

        visualizer->transformNode->addChild(ghostNode);
    }

    ghostNode->setNodeMask(visible ? 0xFFFF : 0);
}


