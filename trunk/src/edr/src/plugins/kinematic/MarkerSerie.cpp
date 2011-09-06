#include "PCH.h"
#include "MarkerSerie.h"
#include <QtGui/QTableView>

void MarkerSerie::setSerieData( const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(MarkerCollection));
	connect(visualizer->actionTrajectories, SIGNAL(triggered()), this, SLOT(trajectoriesDialog()));
	MarkerCollectionConstPtr markersCollection = data->get();
	scheme = SkeletalVisualizationScheme::create();
	scheme->setMarkers(markersCollection);
	int markersCount = markersCollection->getNumChannels();
	try {
		scheme->setMarkersDataFromVsk(Vsk::get(static_cast<Vsk::MarkersCount>(markersCount)));
	} catch (...) {}

	//SchemeDrawerContainerPtr markersDrawer = visualizer->drawersByName["Markers"];
	
	markersDrawer->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawMarkers, 3, 0.02f)));
	markersDrawer->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawMarkers, 10, 0.005f)));
	markersDrawer->addDrawer(trajectoryDrawer);
	//visualizer->currentDrawer = markersDrawer;
	//visualizer->actionByName["Markers"]->setVisible(true);
	markersDrawer->init(scheme);
	visualizer->transformNode->addChild(markersDrawer->getNode());
	dialog->setMarkers(markersCollection);
}

void MarkerSerie::trajectoriesDialog()
{
	dialog->show();
}


