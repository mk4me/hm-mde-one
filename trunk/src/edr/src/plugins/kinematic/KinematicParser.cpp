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
	//SkeletalModelPtr modelPtr(new SkeletalModel);
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
	
	if ( /*modelPtr &&*/ dataPtr && dataPtr->getFrames().size() > 0) {
		skeletonData->set(dataPtr);
		//KinematicModelPtr kin(new KinematicModel);
		/*JointAnglesCollectionPtr kinematicSkeleton(new JointAnglesCollection);
		kinematicSkeleton->setSkeletal(modelPtr, dataPtr);
		skeletonData->set(kinematicSkeleton)*/;
		/*kin->setSkeleton(kinematicSkeleton);
		kin->setMarkers(markers[0]);
		kinematicMarkersSkeleton->set(kin);
		kinematicMarkersSkeleton->setName(path.filename().string());
		kinematicMarkersSkeleton->setSource(path.filename().string());*/
		/*SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
		scheme->setMarkers(markers[0]);
		scheme->setJoints(kinematicSkeleton);
		if (vsk) {
			scheme->setMarkersDataFromVsk(vsk);
		}
		schemeMarkersSkeleton->set(scheme);
		schemeMarkersSkeleton->setName(path.filename().string());
		schemeMarkersSkeleton->setSource(path.string());*/
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

    extDesc.description = "Biovision Hierarchical Data format";

    extensions["bvh"] = extDesc;
}

void KinematicParser::getObjects( core::Objects& objects )
{
	objects.insert(skeletonData);
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

AsfParser::AsfParser()
{
	this->skeletalModel = core::ObjectWrapper::create<kinematic::SkeletalModel>();
}

AsfParser::~AsfParser()
{

}
