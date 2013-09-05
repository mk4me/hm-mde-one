#include "C3DPCH.h"
#include <plugins/c3d/C3DCollections.h>
#include <kinematiclib/VskParser.h>

bool C3DEventsCollection::EventFunctor::operator()(const EventPtr& e1, const EventPtr& e2)
{
	return e1->getTime() < e2->getTime();
}

C3DEventsCollection::C3DEventsCollection()
{

}

C3DEventsCollection::C3DEventsCollection(const C3DEventsCollection& es)
{
	int count = static_cast<int>(es.events.size());
	events.resize(count);
	for (int i = 0; i < count; ++i) {
		events[i] = EventPtr(es.events[i]->clone());
	}
}

int C3DEventsCollection::getNumEvents() const
{
	return events.size();
}

C3DEventsCollection::EventPtr C3DEventsCollection::getEvent(int index)
{
	UTILS_ASSERT(index >= 0 && index < getNumEvents());
	return events[index];
}

C3DEventsCollection::EventConstPtr C3DEventsCollection::getEvent(int index) const
{
	UTILS_ASSERT(index >= 0 && index < getNumEvents());
	return events[index];
}

C3DEventsCollection::iterator C3DEventsCollection::begin()
{
	return events.begin();
}

C3DEventsCollection::iterator C3DEventsCollection::end()
{
	return events.end();
}

C3DEventsCollection::const_iterator C3DEventsCollection::cbegin() const
{
	return events.cbegin();
}

C3DEventsCollection::const_iterator C3DEventsCollection::cend() const
{
	return events.cend();
}

void C3DEventsCollection::addEvent(EventPtr event)
{
	events.push_back(event);
	// wymuszenie kolejnoœci zwi¹zanej z czasem
	std::sort(events.begin(), events.end(), EventFunctor());
}

C3DEventsCollection::EventConstPtr C3DEventsCollection::getNextEvent(EventConstPtr event, Context context) const
{
	// szukanie elementu 'event' w kolekcji
	std::vector<EventPtr>::const_iterator it = events.cbegin();
	for (; it != events.cend(); ++it) {
		if (*it == event) {
			break;
		}
	}
	// jeœli znaleziono event;
	if (it != events.cend()) {
		for (it++; it != events.cend(); ++it) {
			if ((*it)->getContext() == context) {
				return *it;
			}
		}
	}

	return EventConstPtr();
}

C3DEventsCollection::EventConstPtr C3DEventsCollection::getEvent(float t, Context context) const
{
	// przefiltrowanie eventów wzglêdem kontekstu
	std::vector<EventPtr> temp;
	for (auto it = events.cbegin(); it != events.cend(); ++it) {
		if ((*it)->getContext() == context) {
			temp.push_back(*it);
		}
	}

	EventConstPtr found;
	// poszukiwanie odpowiedniego eventu
	for (auto it = temp.begin(); it != temp.end(); ++it) {

		if ((*it)->getTime() <= t) {
			auto check = it;
			++check;
			if (check != temp.end()) {
				found = *it;
			} else {
				// event znaleziony, ale jest ostatni
				return EventConstPtr();
			}
		}

	}
	// nie znaleziono odpowiedniego eventu
	return found;
}

std::vector<FloatPairPtr> getTimeSegments(EventsCollectionConstPtr events, C3DEventsCollection::Context context)
{
	std::vector<FloatPairPtr> ret;

	FloatPairPtr currentSegment;
	for( auto it = events->cbegin(); it != events->cend(); ++it) {
		C3DEventsCollection::EventConstPtr event = *it;
		if (event->getContext() == context) {
			if (event->getLabel() == "Foot Strike") {
				if (currentSegment) {
					currentSegment->second = static_cast<float>(event->getTime());
					ret.push_back(currentSegment);
				}

				currentSegment.reset(new std::pair<float, float>());
				currentSegment->first = static_cast<float>(event->getTime());
			} else if (currentSegment && event->getLabel() == "Foot Off") {
				//currentSegment->event2 = event;
			}
		}
	}

	return ret;
}


const IForcePlatformCollection& GRFCollection::getPlatforms() const
{
	return platforms;
}

void GRFCollection::setPlatforms(const IForcePlatformCollection& val)
{
	platforms = val;
}

GRFChannelConstPtr GRFCollection::getGRFChannel(GRFChannel::Type type) const
{
	for (auto it = channels.begin(); it != channels.end(); ++it) {
		GRFChannelConstPtr channel = utils::dynamic_pointer_cast<const GRFChannel>(*it);
		if (channel->getType() == type) {
			return channel;
		}
	}

	throw std::runtime_error("Wrong GRF channel type");
}

MarkerCollection::MarkerCollection(kinematic::VskParserPtr vsk) :
	VectorChannelCollection(), vsk(vsk)
{

}

const std::string& MarkerCollection::getMarkerName(int markerNo) const
{
	return this->getChannel(markerNo)->getName();
}

VectorChannelConstPtr MarkerCollection::tryGetChannelByName(const std::string& name)
{
	for (int i = this->getNumChannels() - 1; i >= 0; --i) {
		if (getMarkerName(i) == name) {
			return this->getChannel(i);
		}
	}

	return VectorChannelConstPtr();
}

kinematic::VskParserConstPtr MarkerCollection::getVsk() const
{
	return vsk;
}