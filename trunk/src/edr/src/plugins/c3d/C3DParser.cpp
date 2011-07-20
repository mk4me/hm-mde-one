#include "C3DPCH.h"

#include "c3dParser.h"
#include <string>
#include <vector>
#include <boost/program_options.hpp>
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
	MarkerChannels = core::ObjectWrapper::create<MarkerCollection>();
	events = core::ObjectWrapper::create<EventsCollection>();
}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile( core::IDataManager* dataManager, const boost::filesystem::path& path )
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
	EventsCollectionPtr events(new EventsCollection());
	for (int i = 0; i < count; i++) {
		events->addEvent(parser->getEvent(i));
	}
	this->events->set(events);

	MarkerCollectionPtr markers(new MarkerCollection); 
	int markersCount = parser->getNumPoints();
	for (int i = 0; i < markersCount; i++) {
		if (parser->getPoint(i)->getType() == c3dlib::C3DParser::IPoint::Marker) 
		{
			MarkerChannelPtr ptr(new MarkerChannel(*parser, i));
			markers->addChannel(ptr);
		}
	}
	MarkerChannels->set(markers);
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
	objects.push_back(MarkerChannels);
	objects.push_back(events);
	objects.push_back(EMGs);
	objects.push_back(GRFs);
}

void C3DParser::saveFile( const boost::filesystem::path& path )
{
	if (parserPtr) {
		parserPtr->save(path.string());
	}
}

