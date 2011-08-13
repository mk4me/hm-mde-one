#include "C3DPCH.h"

#include "c3dParser.h"
#include <string>
#include <vector>
#include <c3dlib/c3dparser.h>
#include <plugins/c3d/C3DChannels.h>

C3DParser::C3DParser()
{
	for(int i=0; i<12; i++){
		GRFChannels.push_back(core::ObjectWrapper::create<GRFChannel>());
	}
	for(int i=12;i<28;i++){
		EMGChannels.push_back(core::ObjectWrapper::create<EMGChannel>());
	}

	GRFs = core::ObjectWrapper::create<GRFCollection>();
	EMGs = core::ObjectWrapper::create<EMGCollection>();
	markerChannels = core::ObjectWrapper::create<MarkerCollection>();
	forceChannels  = core::ObjectWrapper::create<ForceCollection>();
	angleChannels  = core::ObjectWrapper::create<AngleCollection>();
	momentChannels = core::ObjectWrapper::create<MomentCollection>();
	powerChannels  = core::ObjectWrapper::create<PowerCollection>();
	
	events = core::ObjectWrapper::create<C3DEventsCollection>();
}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile( core::IDataManager* dataManager, const core::Filesystem::Path& path )
{
	ParserPtr parser(new c3dlib::C3DParser());
	parserPtr = parser;
    
    std::vector<const std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parser->importFrom(files, importWarnings);

	GRFCollectionPtr g(new GRFCollection());
	for (int i = 0; i < 12; ++i) {
		GRFChannelPtr ptr(new GRFChannel(*parser , i));
		GRFChannels[i]->set(ptr);
		GRFChannels[i]->setName(ptr->getName());
		GRFChannels[i]->setSource(path.string());
		g->addChannel(ptr);
	}
	GRFs->set(g);

	EMGCollectionPtr e(new EMGCollection());
	for (int i = 12; i < 28; ++i) {
		EMGChannelPtr ptr(new EMGChannel(*parser , i));
		EMGChannels[i-12]->set(ptr);
		EMGChannels[i-12]->setName(ptr->getName());
		EMGChannels[i-12]->setSource(path.string());
		e->addChannel(ptr);
	}
	EMGs->set(e);

	int count = parser->getNumEvents();
	EventsCollectionPtr events(new C3DEventsCollection());
	for (int i = 0; i < count; i++) {
		events->addEvent(parser->getEvent(i));
	}
	this->events->set(events);

	MarkerCollectionPtr markers(new MarkerCollection); 
	ForceCollectionPtr forces(new ForceCollection);
	AngleCollectionPtr angles(new AngleCollection);
	MomentCollectionPtr moments(new MomentCollection);
	PowerCollectionPtr powers(new PowerCollection);

#define CHANNEL_CASE(name, collection)						  	 \
	case c3dlib::C3DParser::IPoint::##name:					  	 \
		{														 \
			name##ChannelPtr ptr(new name##Channel(*parser, i)); \
			collection->addChannel(ptr);						 \
		}														 \
		break;

	int markersCount = parser->getNumPoints();
	for (int i = 0; i < markersCount; i++) {
		switch (parser->getPoint(i)->getType()) {
			CHANNEL_CASE(Angle, angles);
			CHANNEL_CASE(Marker, markers);
			CHANNEL_CASE(Force, forces);
			CHANNEL_CASE(Moment, moments);
			CHANNEL_CASE(Power, powers);
		}
	}
	markerChannels->set(markers);
	forceChannels->set(forces);
	angleChannels->set(angles);
	momentChannels->set(moments);
	powerChannels->set(powers);
}

core::IParser* C3DParser::create()
{
    return new C3DParser();
}

std::string C3DParser::getSupportedExtensions() const
{
    return "c3d";
}

void C3DParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
	objects.insert(objects.end(), GRFChannels.begin(), GRFChannels.end());
	objects.insert(objects.end(), EMGChannels.begin(), EMGChannels.end());
	objects.push_back(markerChannels);
	objects.push_back(events);
	objects.push_back(EMGs);
	objects.push_back(GRFs);
	objects.push_back(forceChannels );
	objects.push_back(angleChannels );
	objects.push_back(momentChannels);
	objects.push_back(powerChannels );
}

void C3DParser::saveFile( const core::Filesystem::Path& path )
{
	if (parserPtr) {
		parserPtr->save(path.string());
	}
}

