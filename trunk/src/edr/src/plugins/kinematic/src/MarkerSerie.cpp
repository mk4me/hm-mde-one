#include "PCH.h"
#include "MarkerSerie.h"
#include "TrajectoriesDrawer.h"
#include "VisualizationScheme.h"

MarkerSerie::MarkerSerie(KinematicVisualizer * visualizer,
	const core::TypeInfo & requestedType,
	const core::ObjectWrapperConstPtr & data) : 
	visualizer(visualizer), requestedType(requestedType),
	data(data), lastUpdateTime(std::numeric_limits<double>::min()),
	name("Markers"),
	pointsDrawer(new PointsDrawer(3)),
	ghostDrawer(new GhostSchemeDrawer(3, 10)),	
	trajectoriesManager(new TrajectoryDrawerManager)
{
	markersCollection = data->get();

	int markersCount = markersCollection->getNumChannels();
	auto vsk = markersCollection->getVsk();
	if(vsk == nullptr){
		try {		
			vsk = Vsk::get(static_cast<Vsk::MarkersCount>(markersCount));
		} catch (...)
		{

		}
	}

	pointsDrawer->init(markersCollection->getNumChannels());
	pointsDrawer->setColor(osg::Vec4(0,0,1,1));
	pointsDrawer->setSize(0.02);

	
	std::vector<osg::Vec4> connectionsColors;
	std::map<unsigned int, osg::Vec4> pointsColors;

	if(vsk != nullptr){

		//mapujemy markery i po³¹czenia dla drawera
		std::vector<std::string> mapping;
		mapping.reserve(markersCollection->getNumChannels());

		for(unsigned int i = 0; i < markersCollection->getNumChannels(); ++i){
			mapping.push_back(markersCollection->getChannel(i)->getName());
		}

		auto sticks = vsk->getSticks();
		auto connectionsNumber = std::distance(sticks.first, sticks.second);

		connectionsConfigurations.reserve(connectionsNumber);
		connectionsColors.reserve(connectionsNumber);

		for (auto it = sticks.first; it != sticks.second; ++it) {
			
			auto it1 = std::find(mapping.begin(), mapping.end(),it->name1);
			auto it2 = std::find(mapping.begin(), mapping.end(),it->name2);

			if (it1 != mapping.end() && it2 != mapping.end()) {

				SegmentDescriptor cd;
				cd.range.first = std::distance(mapping.begin(), it1);
				cd.range.second = std::distance(mapping.begin(), it2);				
				cd.length = getStickLength(markersCollection->getChannel(cd.range.first),
					markersCollection->getChannel(cd.range.second));
				connectionsColors.push_back(it->color);
				connectionsConfigurations.push_back(cd);
			}
		}

		if(connectionsNumber > 0){
			connectionsDrawer.reset(new ConnectionsDrawer(10));
			connectionsDrawer->init(connectionsConfigurations);
			connectionsDrawer->setSize(0.005);
			for(unsigned int i = 0; i < connectionsColors.size(); ++i){
				connectionsDrawer->setColor(i, connectionsColors[i]);
			}
		}

		//kolorowanie
		auto vskMarkers = vsk->getMarkers();		

		for (auto it = vskMarkers.first; it != vskMarkers.second; ++it) {
			auto found = std::find(mapping.begin(), mapping.end(), it->name);
			if (found != mapping.end()) {
				auto d = std::distance(mapping.begin(), found);
				pointsColors[d] = it->color;
				pointsDrawer->setColor(d, it->color);
			}
		}
	}

	matrixTransform->addChild(pointsDrawer->getNode());
	if(connectionsDrawer != nullptr){
		matrixTransform->addChild(connectionsDrawer->getNode());
	}

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for(unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

    if (!connectionsConfigurations.empty()) {
	    ghostDrawer->init(pointsPositions, connectionsConfigurations);
	    ghostDrawer->pointsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	    ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	    ghostDrawer->pointsDrawer()->setSize(0.02);
	    ghostDrawer->connectionsDrawer()->setSize(0.005);
        matrixTransform->addChild(ghostDrawer->getNode());
	    setGhostVisible(false);
    }

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzê z klatek po czasie do klatek po stawach - generalnie transpozycja

	std::vector<std::vector<osg::Vec3>> trajectories(markersCount);

	for(auto it = allPointsPositions.begin(); it != allPointsPositions.end(); ++it){
		for(unsigned int i = 0; i < markersCount; ++i){
			trajectories[i].push_back((*it)[i]);
		}
	}

	trajectoriesManager->initialize(trajectories);
	//trajectoriesManager->setVisible(true);
	trajectoriesManager->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	matrixTransform->addChild(trajectoriesManager->getNode());

	setTime(0.0);
}

const std::vector<std::vector<osg::Vec3>> MarkerSerie::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret;

	const auto step = markersCollection->getChannel(0)->getLength() / (double)density;

	double t = 0.0;

	for(unsigned int i = 0; i < density; ++i){
		ret.push_back(markersCollection->getValues(step * (double)i));
	}

	return ret;
}

float MarkerSerie::getStickLength( VectorChannelConstPtr channel1, VectorChannelConstPtr channel2, float epsilon)
{
	std::map<float, int> histogram;
	const auto s = std::min(channel1->size(), channel2->size());
	const float epsilon2 = std::pow(epsilon, 2.0f);
	const auto delta = std::max<unsigned int>(1, s / 100);

	histogram[(channel1->value(0) - channel2->value(0)).length2()] = 1;

	for(unsigned int i = delta; i < s; i += delta){		
		float len2 = (channel1->value(i) - channel2->value(i)).length2();

		auto it = histogram.lower_bound(len2);

		if(it == histogram.end()){
			--it;
		}

		if(std::abs(it->first - len2) < epsilon2){
			++(it->second);
		}else{
			histogram[len2] = 1;
		}
	}

	float ret = 0.0f;
	int max = 0;
	for (auto it = histogram.begin(); it != histogram.end(); ++it) {
		if (max < it->second) {
			max = it->second;
			ret = it->first;
		}
	}

	return std::sqrt(ret);
}

void MarkerSerie::setLocalTime( double time )
{
	if( (lastUpdateTime == std::numeric_limits<double>::min()) ||
		(std::abs(time - lastUpdateTime) >= markersCollection->getChannel(0)->getSampleDuration())){
			lastUpdateTime = time;
			requestUpdate();
	}
}

void MarkerSerie::update()
{
	auto t = std::max(lastUpdateTime, 0.0);

	auto positions = markersCollection->getValues(t);
	pointsDrawer->update(positions);

	if(connectionsDrawer != nullptr){

		std::set<int> toShow;

		for(unsigned int i = 0; i < connectionsConfigurations.size(); ++i){

			const auto pointAIDX = connectionsConfigurations[i].range.first;
			const auto pointBIDX = connectionsConfigurations[i].range.second;

			const auto dist = (positions[pointAIDX] - positions[pointBIDX]).length();			
			const auto diff = std::abs(dist - connectionsConfigurations[i].length) / 2.0;			
			
			//if( diff > std::min(pointsDrawer->size(pointAIDX), pointsDrawer->size(pointBIDX)) ){
			//	connectionsDrawer->setVisible(i, false);
			//}else{
				toShow.insert(i);
			//}
		}

		connectionsDrawer->update(positions);

		for(auto it = toShow.begin(); it != toShow.end(); ++it){
			connectionsDrawer->setVisible(*it, true);
		}
	}
}

osg::Matrix MarkerSerie::getInitialMatrix() const
{
    //osg::Matrix m;
    //m.setTrans(scheme->getRootPosition(0.0));
    return osg::Matrix();
}

double MarkerSerie::getLength() const
{
    return markersCollection->getLength();
}

double MarkerSerie::getBegin() const
{
	return 0.0;
}

double MarkerSerie::getEnd() const
{
	return getLength();
}

const core::ObjectWrapperConstPtr & MarkerSerie::getData() const
{
    return data;
}

const std::string MarkerSerie::getName() const
{
    return name;
}

void MarkerSerie::setName( const std::string & name )
{
    this->name = name;
}

const utils::TypeInfo & MarkerSerie::getRequestedDataType() const
{
	return requestedType;
}

const bool MarkerSerie::ghostVisible() const
{
	return false;
}

void MarkerSerie::setGhostVisible(const bool visible)
{
	ghostDrawer->getNode()->setNodeMask( visible == true ? 1 : 0);
}

TrajectoryDrawerManagerPtr MarkerSerie::getTrajectoriesManager() const
{
    return trajectoriesManager;
}

IConnectionDrawerWithDescriptors MarkerSerie::getConnectionsDrawer() const
{
    return std::make_pair(connectionsDrawer, connectionsConfigurations);
}

IPointsSchemeDrawerPtr MarkerSerie::getPointsDrawer() const
{
    return pointsDrawer;
}

