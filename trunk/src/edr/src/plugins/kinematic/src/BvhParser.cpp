#include "PCH.h"
#include <corelib/Filesystem.h>
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "BvhParser.h"

BvhParser::BvhParser()
{

}

BvhParser::~BvhParser()
{

}

void BvhParser::parse( const std::string & source )
{
	//skeletalModel = utils::ObjectWrapper::create<kinematic::JointAnglesCollection>();
	skeletalModel = utils::ObjectWrapper::create<BVHData>();
	core::Filesystem::Path path(source);  
    kinematic::SkeletalModelPtr modelPtr = utils::make_shared<kinematic::SkeletalModel>();
	kinematic::SkeletalDataPtr dataPtr = utils::make_shared<kinematic::SkeletalData>();
    kinematic::BvhParser bvh;
    bvh.parse(modelPtr, dataPtr, path.string());

	auto pairPtr = utils::make_shared<BVHData>(std::make_pair(modelPtr, dataPtr));
	skeletalModel->set(pairPtr);
	//kinematic::JointAnglesCollectionPtr jc = utils::make_shared<kinematic::JointAnglesCollection>();
	//kinematic::hAnimSkeletonPtr hanim = jc->getHAnimSkeleton();

	//kinematic::SkeletonMappingScheme::jointsMappingDict jointMappingDictionary;
	//jointMappingDictionary["root"] = "HumanoidRoot";
	////jointMappingDictionary["hip"] = "vl5";
	//jointMappingDictionary["hip"] = "HumanoidRoot";
	//jointMappingDictionary["abdomen"] = "vt12";
	//jointMappingDictionary["abdomen2"] = "vt1";
	//jointMappingDictionary["prechest"] = "vt2";
	//jointMappingDictionary["chest"] = "vc7";
	//jointMappingDictionary["neck"] = "vc1";
	//jointMappingDictionary["head"] = "skullbase";
	//jointMappingDictionary["rCollar"] = "r_sternoclavicular";
	//jointMappingDictionary["rShldr"] = "r_shoulder";
	//jointMappingDictionary["rForeArm"] = "l_elbow";
	//jointMappingDictionary["rHand"] = "r_wrist";
	//jointMappingDictionary["lCollar"] = "l_sternoclavicular";
	//jointMappingDictionary["lShldr"] = "l_shoulder";
	//jointMappingDictionary["lForeArm"] = "r_elbow";
	//jointMappingDictionary["lHand"] = "l_wrist";
	//jointMappingDictionary["rButtock"] = "r_hip";
	//jointMappingDictionary["rShin"] = "r_knee";
	//jointMappingDictionary["rFoot"] = "r_ankle";
	//jointMappingDictionary["rToe"] = "r_metatarsal";
	//jointMappingDictionary["lButtock"] = "l_hip";
	//jointMappingDictionary["lShin"] = "l_knee";
	//jointMappingDictionary["lFoot"] = "l_ankle";
	//jointMappingDictionary["lToe"] = "l_metatarsal";
	//
	//kinematic::SkeletonMappingSchemePtr scheme = kinematic::SkeletonMappingScheme::create(jointMappingDictionary);
	///*for (auto& frame : dataPtr->getFrames()) {
	//	for (auto& jd : frame->jointsData) {
	//		for (auto& c : jd->channelValues) {
	//			c *= -1;
	//		}
	//	}
	//}*/
	//jc->setSkeletal(modelPtr, dataPtr, scheme);
	
    //skeletalModel->set(jc);
}

plugin::IParser* BvhParser::create() const
{
    return new BvhParser();
}

void BvhParser::acceptedExpressions(Expressions & expressions) const
{
    plugin::IParser::ExpressionDescription expDesc;
    expDesc.description = "Biovision Skeleton File format";

    //expDesc.objectsTypes.push_back(typeid(kinematic::JointAnglesCollection));
	expDesc.objectsTypes.push_back(typeid(BVHData));
    expressions[".*\\.bvh$"] = expDesc;
}

void BvhParser::reset()
{
	skeletalModel.reset();
}

void BvhParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(skeletalModel);
}