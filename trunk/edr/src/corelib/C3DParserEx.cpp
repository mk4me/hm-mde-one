#include "CorePCH.h"
#include <core/C3DParserEx.h>
#include <core/C3DChannels.h>

using namespace core;

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


}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile(const boost::filesystem::path& path)
{

    this->path = path;

    // parsowanie pliku
    c3dParser parser;
    scoped_ptr<C3D_Data> data(parser.parseData(path.string()));

    for (int i = 0; i < 12; ++i) {
        GRFChannels[i]->set<GRFChannel>(GRFChannelPtr(new GRFChannel(*data , i)));
    }
    for (int i = 12; i < 28; ++i) {
        EMGChannels[i-12]->set<EMGChannel>(EMGChannelPtr(new EMGChannel(*data , i)));
    }

    //object = core::ObjectWrapper::createWrapper<C3D_Data>(parser.parseData(path.string()));
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
}