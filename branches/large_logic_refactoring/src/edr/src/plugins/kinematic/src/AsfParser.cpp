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

void AsfParser::parseFile( const core::Filesystem::Path& path )
{
    kinematic::SkeletalModelPtr modelPtr(new kinematic::SkeletalModel);
    kinematic::AsfParser asf;
    asf.parse(modelPtr, path.string());
    skeletalModel->set(modelPtr);
}

core::IParser* AsfParser::create()
{
    return new AsfParser();
}

void AsfParser::getSupportedExtensions(Extensions & extensions) const
{
    core::IParser::ExtensionDescription extDesc;
    extDesc.description = "Acclaim Skeleton File format";

    extDesc.types.insert(typeid(kinematic::SkeletalModel));

    extensions["asf"] = extDesc;
}

void AsfParser::getObjects( core::Objects& objects )
{
    objects.insert(this->skeletalModel);
}

