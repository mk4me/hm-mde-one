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

	MarkerChannels = core::ObjectWrapper::create<MarkerSet>();
}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile( core::IDataManager* dataManager, const boost::filesystem::path& path )
{
	c3dlib::C3DParser parser;
    
    std::vector<const std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parser.importFrom(files, importWarnings);

	for (int i = 0; i < 12; ++i) {
		GRFChannelPtr ptr(new GRFChannel(parser , i));
		GRFChannels[i]->set(ptr);
		GRFChannels[i]->setName(ptr->getName());
		GRFChannels[i]->setSource(path.string());
	}
	for (int i = 12; i < 28; ++i) {
		EMGChannelPtr ptr(new EMGChannel(parser , i));
		EMGChannels[i-12]->set(ptr);
		EMGChannels[i-12]->setName(ptr->getName());
		EMGChannels[i-12]->setSource(path.string());
	}

	MarkerSetPtr markers(new MarkerSet); 
	int markersCount = parser.getNumPoints();
	for (int i = 0; i < markersCount; i++) {
		if (parser.getPoint(i)->getType() == c3dlib::C3DParser::IPoint::Marker) 
		{
			MarkerChannelPtr ptr(new MarkerChannel(parser, i));
			markers->addMarker(ptr);
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
}
