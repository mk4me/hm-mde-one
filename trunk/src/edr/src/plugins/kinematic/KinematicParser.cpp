#include "PCH.h"
#include <core/Filesystem.h>
#include "KinematicParser.h"
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "ISchemeDrawer.h"


KinematicParser::KinematicParser()
{
    skeletonData = core::ObjectWrapper::create<kinematic::SkeletalData>();
}

KinematicParser::~KinematicParser()
{

}

void KinematicParser::parseFile(const core::Filesystem::Path& path)
{
	using namespace kinematic;
	using kinematic::AsfParser;

	SkeletalDataPtr dataPtr(new SkeletalData());

	if(core::Filesystem::fileExtension(path).compare(".amc") == 0) {
		AmcParser amc;
		std::string amcFilename = path.string();
		amc.parse(dataPtr, amcFilename);
	} 
	// bvh chwilowo wylaczone
	/*else if (core::Filesystem::fileExtension(path).compare(".bvh") == 0)  {
		BvhParser bvh;
		bvh.parse(modelPtr, dataPtr, path.string());
	}*/
	
	if (  dataPtr && dataPtr->getFrames().size() > 0) {
		skeletonData->set(dataPtr);
	}
}

core::IParser* KinematicParser::create()
{
    return new KinematicParser();
}

void KinematicParser::getSupportedExtensions(Extensions & extensions) const
{
    core::IParser::ExtensionDescription extDesc;

    extDesc.description = "Acclaim Motion Capture format";
    extDesc.types.insert(typeid(kinematic::SkeletalData));
    extensions["amc"] = extDesc;

    //extDesc.description = "Biovision Hierarchical Data format";
    //extensions["bvh"] = extDesc;
}

void KinematicParser::getObjects( core::Objects& objects )
{
	objects.insert(skeletonData);
}
