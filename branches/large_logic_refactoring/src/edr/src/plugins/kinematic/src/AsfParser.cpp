#include "PCH.h"
#include <corelib/Filesystem.h>
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "AsfParser.h"
#include "ISchemeDrawer.h"

AsfParser::AsfParser()
{
    this->skeletalModel = core::ObjectWrapper::create<kinematic::SkeletalModel>();
}

AsfParser::~AsfParser()
{

}

void AsfParser::parse( const std::string & source )
{
	core::Filesystem::Path path(source);
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

    expDesc.types.insert(typeid(kinematic::SkeletalModel));

    expressions[".*\.asf$"] = expDesc;
}

void AsfParser::getObjects( core::Objects& objects )
{
    objects.insert(this->skeletalModel);
}

