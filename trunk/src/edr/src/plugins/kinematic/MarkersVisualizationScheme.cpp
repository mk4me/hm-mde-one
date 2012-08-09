#include "PCH.h"
#include "MarkersVisualizationScheme.h"
#include "UniqueCollection.h"
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
        for (int i = 0; i < count; ++i) {
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
    for (auto it = vskMarkers.first; it != vskMarkers.second; ++it) {
        std::map<std::string, int>::iterator found = markersIndices.find(it->name);
        if (found != markersIndices.end()) {
            states[found->second].color = it->color;
        }
    }
    auto sticks = vsk->getSticks();
    connections.reserve(std::distance(sticks.first, sticks.second));
    for (auto it = sticks.first; it != sticks.second; ++it) {
        Connection c;
        auto it1 = markersIndices.find(it->name1);
        auto it2 = markersIndices.find(it->name2);

        if (it1 != markersIndices.end() && it2 != markersIndices.end()) {
            c.index1 = markersIndices[it->name1];
            c.index2 = markersIndices[it->name2];
            VectorChannelConstPtr channel1 = markers->getChannel(c.index1);
            VectorChannelConstPtr channel2 = markers->getChannel(c.index2);
            c.length = getStickLength(channel1, channel2);
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
	for (int i = 0; i < count; ++i) {
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
    for (int i = 0; i < count; ++i) {
        position += markers->getValue(i, time);
    }
    position *= (1.0f / count);
    return position;
}

float MarkersVisualizationScheme::getStickLength( VectorChannelConstPtr channel1, VectorChannelConstPtr channel2, float epsilon)
{
    std::map<float, int> histogram;
    float length = (std::min)(channel1->getLength(), channel2->getLength());
    float t = 0.0f;
    float delta = length / 100;

    while (t < length) {
        auto i1 = channel1->utils::IRawUniformDataChannelReader<osg::Vec3f,float>::getValueHelper(t).first;
        auto i2 = channel2->utils::IRawUniformDataChannelReader<osg::Vec3f,float>::getValueHelper(t).first;
        float len = (channel1->value(i1) - channel2->value(i2)).length();
        bool found = false;
        for (auto it = histogram.begin(); it != histogram.end(); ++it) {
            if (std::abs(it->first - len) < epsilon) {
                histogram[it->first] += 1;
                found = true;
                break;
            }
        }

        if (!found) {
            histogram[len] = 1;
        }
        t += delta;
    }

    float ret = 0.0f;
    int max = 0;
    for (auto it = histogram.begin(); it != histogram.end(); ++it) {
        if (max < it->second) {
            max = it->second;
            ret = it->first;
        }
    }

    return ret;
}

double MarkersVisualizationScheme::getDuration() const
{
    if (markers) {
        return markers->getLength();
    }
    UTILS_ASSERT(false);
    return 0.0;
}

MarkerCollectionConstPtr MarkersVisualizationScheme::getMarkers() const
{
    return markers;
}

bool MarkersVisualizationScheme::hasData() const
{
    return markers.get();
}

osg::Vec3 MarkersVisualizationScheme::getCurrentPosition() const
{
   return currentPosition;
}

void MarkersVisualizationScheme::update( double time )
{
    updateMarkers(time);
}

