#include "DicomPCH.h"
#include "LayersXmlParser.h"
#include "LayeredImage.h"
#include <corelib/Filesystem.h>
#include <plugins/dicom/Dicom.h>
#include "DicomSource.h"

using namespace dicom;

LayersXmlParser::LayersXmlParser()
{
    layers = core::ObjectWrapper::create<LayersVector>();
}

LayersXmlParser::~LayersXmlParser()
{

}


void LayersXmlParser::parse( const std::string & source  )
{
    LayersVectorPtr layers = DicomLoader::loadLayers(source);
    this->layers->set(layers);
}

plugin::IParser* LayersXmlParser::create() const
{
    return new LayersXmlParser();
}

void LayersXmlParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Layers Xml";

    expDesc.types.insert(typeid(LayersVector));
    expressions.insert(Expressions::value_type(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}-T[0-9]{4}\\.xml$", expDesc));
    expressions.insert(Expressions::value_type(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}-T[0-9]{4}\\..*\\.xml$", expDesc));
}

void LayersXmlParser::getObjects( core::Objects& objects )
{
    objects.insert(layers);
}


