#include "PCH.h"
#include <corelib/Filesystem.h>
#include "KinematicParser.h"
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>


KinematicParser::KinematicParser()
{
    skeletonData = core::ObjectWrapper::create<kinematic::SkeletalData>();
}

KinematicParser::~KinematicParser()
{

}

void KinematicParser::parse( const std::string & source)
{
	core::Filesystem::Path path(source);
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

plugin::IParser* KinematicParser::create() const
{
    return new KinematicParser();
}

void KinematicParser::acceptedExpressions(Expressions & extensions) const
{
    plugin::IParser::ExpressionDescription expDesc;

    expDesc.description = "Acclaim Motion Capture format";
    expDesc.types.insert(typeid(kinematic::SkeletalData));
    extensions[".*\.amc$"] = expDesc;

    //expDesc.description = "Biovision Hierarchical Data format";
    //extensions["bvh"] = expDesc;
}

void KinematicParser::getObjects( core::Objects& objects )
{
	objects.insert(skeletonData);
}
