#include "DicomPCH.h"
#include "PngParser.h"
#include <corelib/Filesystem.h>
#include "LayeredImage.h"
#include <plugins/dicom/Dicom.h>

using namespace dicom;

PngParser::PngParser()
{
    image = core::ObjectWrapper::create<LayeredImage>();
}

PngParser::~PngParser()
{

}


void PngParser::parse( const std::string & source  )
{
	//QPixmap p(source.c_str());
    LayeredImagePtr l = utils::make_shared<LayeredImage>(source);
    image->set(l);
}

plugin::IParser* PngParser::create() const
{
    return new PngParser();
}

void PngParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Png format";

    expDesc.types.insert(typeid(LayeredImage));
    expressions.insert(Expressions::value_type(".*\.png$", expDesc));
}

void PngParser::getObjects( core::Objects& objects )
{
    objects.insert(image);
}


