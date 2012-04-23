#include "PCH.h"
#include "MarkersVisualizationScheme.h"
#include "uniqueCollection.h"
#include "ISchemeDrawer.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

void MarkersVisualizationScheme::updateMarkers( double normalizedTime )
{
	UTILS_ASSERT(markers);
    if (markers) {
        double time = normalizedTime * markers->getLength();
        currentPosition = getRootPosition(time);

        int count =  markers->getNumChannels();
        for (int i = 0; i < count; i++) {
            states[i].position = markers->getValue(i, time) - currentPosition;
        }
    }
}

void MarkersVisualizationScheme::setMarkersDataFromVsk( kinematic::VskParserConstPtr vsk )
{
    UTILS_ASSERT(markers, "There are no markers in kinematic model");

    std::map<std::string, int> markersIndices;
    for (int i = markers->getNumChannels() - 1; i >= 0; --i) {
        markersIndices[markers->getMarkerName(i)] = i;
    }
   
    auto vskMarkers = vsk->getMarkers();
    for (auto it = vskMarkers.first; it != vskMarkers.second; it++) {
        std::map<std::string, int>::iterator found = markersIndices.find(it->name);
        if (found != markersIndices.end()) {
            states[found->second].color = it->color;
        }
    }
    auto sticks = vsk->getSticks();
    connections.reserve(std::distance(sticks.first, sticks.second));
    for (auto it = sticks.first; it != sticks.second; it++) {
        Connection c;
        auto it1 = markersIndices.find(it->name1);
        auto it2 = markersIndices.find(it->name2);
        if (it1 != markersIndices.end() && it2 != markersIndices.end()) {
            c.index1 = markersIndices[it->name1];
            c.index2 = markersIndices[it->name2];
            c.color = it->color;
            connections.push_back(c);
        }
    }
}

void MarkersVisualizationScheme::setMarkers( MarkerCollectionConstPtr val )
{
	UTILS_ASSERT(!markers && val);
	this->markers = val;
    int count =  markers->getNumChannels();
	if (count && states.size() != count) {
		states.resize(count);
	}
	osg::Vec4 blue(0,0,1,1);
	for (int i = 0; i < count; i++) {
        states[i].color = blue;
        states[i].name = markers->getMarkerName(i);
	}

    updateMarkers(0.0);
}

osg::Vec3 MarkersVisualizationScheme::getRootPosition( double time ) const
{
    int count =  markers->getNumChannels();
    osg::Vec3 position;
    position.set(0.0, 0.0, 0.0);
    for (int i = 0; i < count; i++) {
        position += markers->getValue(i, time);
    }
    position *= (1.0f / count);
    return position;
}


