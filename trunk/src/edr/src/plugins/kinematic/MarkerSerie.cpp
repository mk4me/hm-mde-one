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
	scheme = MarkersVisualizationSchemePtr(new MarkersVisualizationScheme());
	scheme->setMarkers(markersCollection);
	int markersCount = markersCollection->getNumChannels();
	try {
        auto vsk = markersCollection->getVsk();
		scheme->setMarkersDataFromVsk(vsk ? vsk : Vsk::get(static_cast<Vsk::MarkersCount>(markersCount)));
	} catch (...) {}
    	
	markersDrawer->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(3, 0.02f)));
	markersDrawer->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(10, 0.005f)));
    
    trajectoryDrawer = TrajectoryDrawerPtr(new TrajectoryDrawer(osg::Vec4(1, 1, 1, 0.33f), 300));
	markersDrawer->addDrawer(trajectoryDrawer);
	markersDrawer->init(scheme);
    visualizer->trajectoriesDialog->setDrawer(markersDrawer, QString(data->getName().c_str()));
    visualizer->schemeDialog->setDrawer(markersDrawer, QString(data->getName().c_str()));

	transformNode->addChild(markersDrawer->getNode());
    matrixTransform->setMatrix(getInitialMatrix());
}


void MarkerSerie::showGhost( bool visible )
{
    float time = 0.0f;
    if (!ghostNode) {
        ghostNode = new osg::PositionAttitudeTransform();
        MarkerCollectionConstPtr markersCollection = data->get();
        while (time < this->getLength()) {
            MarkersVisualizationSchemePtr scheme(new MarkersVisualizationScheme);
            
            scheme->setMarkers(markersCollection);
            try {
                scheme->setMarkersDataFromVsk(Vsk::get(static_cast<Vsk::MarkersCount>(markersCollection->getNumChannels())));
            } catch (...) {}
            scheme->setTime(time);
            osg::Vec4 color(1.0f, 1.0f, 0.9f, 0.25f);
            OsgSchemeDrawerPtr drawer1(new GlPointSchemeDrawer(3, 0.02f, color));
            OsgSchemeDrawerPtr drawer2(new GlLineSchemeDrawer(10, 0.005f, color));
            drawer1->init(scheme);
            drawer2->init(scheme);
            drawer1->update();
            drawer2->update();
            ghostNode->addChild(drawer1->getNode());
            ghostNode->addChild(drawer2->getNode());
            time += 1.0f;
        }

        transformNode->addChild(ghostNode);
    }

    ghostNode->setNodeMask(visible ? 0xFFFF : 0);
}

void MarkerSerie::setLocalTime( double time )
{
    UTILS_ASSERT(scheme);
    auto shift = scheme->getCurrentPosition();
    scheme->setTime(time);
    markersDrawer->update();
    osg::Matrix m = matrixTransform->getMatrix();
    shift = scheme->getCurrentPosition() - shift;
    osg::Matrix rot = m;
    rot.setTrans(osg::Vec3());
    m.setTrans(m.getTrans() + shift * rot);
    matrixTransform->setMatrix(m);
}

osg::Matrix MarkerSerie::getInitialMatrix() const
{
    osg::Matrix m;
    m.setTrans(scheme->getRootPosition(0.0));
    return m;
}



