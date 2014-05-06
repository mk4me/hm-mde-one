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

}

KinematicParser::~KinematicParser()
{

}

void KinematicParser::parse( const std::string & source)
{
	skeletonData = utils::ObjectWrapper::create<kinematic::SkeletalData>();
	core::Filesystem::Path path(source);
    //path = std::string("C:/Users/Wojciech/Desktop/nowyTest/2011-10-28-B0047-S02-T04.amc");
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

void KinematicParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(skeletonData);
}

void KinematicParser::reset()
{
	skeletonData.reset();
}

plugin::IParser* KinematicParser::create() const
{
    return new KinematicParser();
}

void KinematicParser::acceptedExpressions(Expressions & extensions) const
{
    plugin::IParser::ExpressionDescription expDesc;

    expDesc.description = "Acclaim Motion Capture format";
    expDesc.objectsTypes.push_back(typeid(kinematic::SkeletalData));
    extensions[".*\\.amc$"] = expDesc;

    //expDesc.description = "Biovision Hierarchical Data format";
    //extensions["bvh"] = expDesc;
}