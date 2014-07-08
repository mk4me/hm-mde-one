#include "PCH.h"
#include <corelib/Filesystem.h>
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "AsfParser.h"

AsfParser::AsfParser()
{

}

AsfParser::~AsfParser()
{

}

void AsfParser::parse( const std::string & source )
{
	skeletalModel = utils::ObjectWrapper::create<kinematic::SkeletalModel>();
	core::Filesystem::Path path(source);
    //path = std::string("C:/Users/Wojciech/Desktop/nowyTest/KrakowiakAdam.asf");
  
    kinematic::SkeletalModelPtr modelPtr(new kinematic::SkeletalModel);
    kinematic::AsfParser asf;
    asf.parse(modelPtr, path.string());
    skeletalModel->set(modelPtr);
}

plugin::IParser* AsfParser::create() const
{
    return new AsfParser();
}

void AsfParser::acceptedExpressions(Expressions & expressions) const
{
    plugin::IParser::ExpressionDescription expDesc;
    expDesc.description = "Acclaim Skeleton File format";

    expDesc.objectsTypes.push_back(typeid(kinematic::SkeletalModel));

    expressions[".*\\.asf$"] = expDesc;
}

void AsfParser::reset()
{
	skeletalModel.reset();
}

void AsfParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(skeletalModel);
}