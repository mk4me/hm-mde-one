#include "CorePCH.h"
#include <algorithm>
#include <core/C3DParserEx.h>
#include <core/C3DChannels.h>
#include <core/IMarker.h>
#include <core/Marker.h>

using namespace core;

static const int markersSlots = 39;

C3DParser::C3DParser()
{
    //object = core::ObjectWrapper::createWrapper<C3D_Data>();
    // dodanie kana³ów
    for(int i=0; i<12; i++){
        GRFChannels.push_back(ObjectWrapper::createWrapper<GRFChannel>());
    }
    for(int i=12;i<28;i++){
        EMGChannels.push_back(ObjectWrapper::createWrapper<EMGChannel>());
    }

    MarkerChannels = ObjectWrapper::createWrapper<MarkerSet>();

    //MarkerChannels.resize(markersSlots);
    //for(int i = 0; i < markersSlots; i++){
    //    MarkerChannels[i] = ObjectWrapper::createWrapper<MarkerChannel>();
    //}


}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile(core::IDataManager* /*dataManager*/, const boost::filesystem::path& path)
{
    this->path = path;
    // parsowanie pliku
    c3dParser parser;
    scoped_ptr<C3D_Data> data(parser.parseData(path.string()));

    for (int i = 0; i < 12; ++i) {
        GRFChannelPtr ptr(new GRFChannel(*data , i));
        GRFChannels[i]->set<GRFChannel>(ptr);
        GRFChannels[i]->setName(ptr->getName());
        GRFChannels[i]->setSource(path.string());
    }
    for (int i = 12; i < 28; ++i) {
        EMGChannelPtr ptr(new EMGChannel(*data , i));
        EMGChannels[i-12]->set<EMGChannel>(ptr);
        EMGChannels[i-12]->setName(ptr->getName());
        EMGChannels[i-12]->setSource(path.string());
    }

    MarkerSetPtr markers(new MarkerSet);
    const ::GroupData* videoGroup = data->getParameters()->getGroup("POINT");
    int markersCount = data->getHeader()->getNumberOfC3DPoints();
    /*std::vector<short> dimensions = videoGroup->getParameter("LABELS2")->getDimensions();
    markersCount -= dimensions[1];
    dimensions =  videoGroup->getParameter("FORCES")->getDimensions();
    markersCount -= dimensions[1];
    dimensions =  videoGroup->getParameter("MOMENTS")->getDimensions();
    markersCount -= dimensions[1];
    dimensions =  videoGroup->getParameter("ANGLES")->getDimensions();
    markersCount -= dimensions[1];                  
    dimensions =  videoGroup->getParameter("POWERS")->getDimensions();
    markersCount -= dimensions[1];*/

    markersCount = markersSlots < markersCount ? markersSlots : markersCount;
    //MarkerChannels.resize(markersCount);
    for (int i = 0; i < markersCount; ++i) {
        MarkerChannelPtr ptr(new MarkerChannel(*data, i));
        markers->addMarker(ptr);
    }
    MarkerChannels->set<MarkerSet>(markers);
}

core::IParser* C3DParser::create()
{
    return new C3DParser();
}

std::string C3DParser::getSupportedExtensions() const
{
    return "c3d";
}

C3D_Data* C3DParser::getC3dData()
{
    return nullptr;
}

void C3DParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
    objects.insert(objects.end(), GRFChannels.begin(), GRFChannels.end());
    objects.insert(objects.end(), EMGChannels.begin(), EMGChannels.end());
    objects.push_back(MarkerChannels);

}