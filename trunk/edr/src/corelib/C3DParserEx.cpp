#include "CorePCH.h"
#include <core/C3DParserEx.h>

C3DParser::C3DParser()
{
    object = core::ObjectWrapper::createWrapper<C3D_Data>();
}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile(const boost::filesystem::path& path)
{
    this->path = path;
    c3dParser parser;
    object = core::ObjectWrapper::createWrapper<C3D_Data>(parser.parseData(path.string()));
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
    return object->get<C3D_Data>().get();
}

core::ObjectWrapperPtr C3DParser::getObject()
{
    return object;
}
