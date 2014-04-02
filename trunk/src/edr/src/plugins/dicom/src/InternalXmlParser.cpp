#include "DicomPCH.h"
#include "InternalXmlParser.h"
#include <corelib/Filesystem.h>
#include "LayeredImage.h"
#include <plugins/dicom/Dicom.h>
#include "DicomSource.h"
#include "DicomInternalStruct.h"
#include <regex>

using namespace dicom;

InternalXmlParser::InternalXmlParser()
{
    sessionInternal = core::ObjectWrapper::create<DicomInternalStruct>();
}

InternalXmlParser::~InternalXmlParser()
{

}


void InternalXmlParser::parse( const std::string & source  )
{    
    DicomInternalStructPtr inter = DicomLoader::load(source);
    sessionInternal->set(inter);
    //LayeredImagePtr l = utils::make_shared<LayeredImage>(source);
    //image->set(l);
}

plugin::IParser* InternalXmlParser::create() const
{
    return new InternalXmlParser();
}

void InternalXmlParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Internal Dicom Xml";

    expDesc.types.insert(typeid(DicomInternalStruct));
    expressions.insert(Expressions::value_type(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\\.xml$", expDesc));

    //std::cmatch cm;
    //bool test = std::regex_match("C:\\Users\\Wojciech\\Documents\\PJWSTK\\MDE\\tmp\\newCommunication\\data\\zxl2rjizxf56pzxl2rjizxf7i\\data\\2013-08-21-S0007\\2013-08-21-S0007.xml", cm,std::regex(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\.xml$"));
}

void InternalXmlParser::getObjects( core::Objects& objects )
{
    objects.insert(sessionInternal);
}


