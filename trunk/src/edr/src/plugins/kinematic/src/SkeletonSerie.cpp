#include "PCH.h"
#include "SkeletonSerie.h"
#include "OsgSchemeDrawer.h"
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <utils/StreamData.h>

#include <osgAnimation/Bone>
#include <osgAnimation/BoneMapVisitor>
#include <osgAnimation/Skeleton>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osgAnimation/BasicAnimationManager>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonSerie::SkeletonSerie( KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data ) : 
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
    xyzAxis(false),
	name("Skeleton"),
	pointsDrawer(new PointsDrawer(3)),
	connectionsDrawer(new ConnectionsDrawer(10)),
	jointsMapping(new SkeletonJointsMapping),
	localRootNode(new osg::PositionAttitudeTransform)
{	
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(kinematic::JointAnglesCollection));
	data->getMetadata("core/name", name);	
	jointAngles = data->get();

	std::vector<std::string> mapping;
	mapping.reserve(jointAngles->getNumChannels());

	for(unsigned int i = 0; i < jointAngles->getNumChannels(); ++i){
		mapping.push_back(jointAngles->getChannel(i)->getName());
	}

	jointsMapping->init(jointAngles->getHAnimSkeleton(), mapping);

	skeletonSchemeHelper.reset(new SkeletalVisualizationSchemeHelper(pointsDrawer.get(),
		connectionsDrawer.get()));

	auto connections = jointsMapping->generateMappedConnectionsDescription();

	skeletonSchemeHelper->init(jointsMapping.get(), connections);

	pointsDrawer->setSize(0.02);
	connectionsDrawer->setSize(0.005);

	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));

	localRootNode->addChild(pointsDrawer->getNode());
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	setTime(0.0);

	rootPosition = jointAngles->getRootPosition(0.0);

	setAxis(true);
}

const std::vector<std::vector<osg::Vec3>> SkeletonSerie::createPointsPositions(const unsigned int density) const
{
	std::vector<std::vector<osg::Vec3>> ret(density);

	const double delta = jointAngles->getLength() / (double)density;

	double time = 0.0;

	for(unsigned int i = 0; i < density; ++i, time = delta * i){
		auto rotations = jointAngles->getValues(time);
		auto rootPosition = jointAngles->getRootPosition(time / jointAngles->getLength());
		std::vector<osg::Vec3> positions;
		skeletonSchemeHelper->calculatePointsPositions(positions,
			rootPosition,
			rotations);

		ret[i] = positions;
	}

	return ret;
}

void SkeletonSerie::setAxis( bool xyz)
{
	//TODO - sprawdzic jak to dziala
    localRootNode->setAttitude( xyz == true ? invQXYZ : osg::Quat());

	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}

void SkeletonSerie::setName( const std::string & name )
{
    this->name = name;
}

const std::string SkeletonSerie::getName() const
{
    return name;
}

const core::VariantConstPtr & SkeletonSerie::getData() const
{
    return data;
}

double SkeletonSerie::getLength() const
{
    return jointAngles->getLength();
}

double SkeletonSerie::getBegin() const
{
	return 0.0;
}

double SkeletonSerie::getEnd() const
{
	return getLength();
}

const osg::Vec3 SkeletonSerie::pivotPoint() const
{
	return rootPosition * lToW;
}

void SkeletonSerie::update()
{
	auto t = std::max(lastUpdateTime, 0.0);

	auto rotations = jointAngles->getValues(lastUpdateTime);
	rootPosition = jointAngles->getRootPosition(lastUpdateTime / jointAngles->getLength());

	skeletonSchemeHelper->updateJointTransforms(rootPosition, rotations);
}

void SkeletonSerie::setLocalTime(double time)
{
	if( (lastUpdateTime == std::numeric_limits<double>::min()) ||
		(std::abs(time - lastUpdateTime) >= jointAngles->getChannel(0)->getSampleDuration())){
		lastUpdateTime = time;
		requestUpdate();
	}
}

const utils::TypeInfo & SkeletonSerie::getRequestedDataType() const
{
	return requestedType;
}

const bool SkeletonSerie::ghostVisible() const
{
	if(ghostDrawer == nullptr){
		return false;
	}
	
	return !(ghostDrawer->getNode()->getNodeMask() == 0);
}

void SkeletonSerie::createGhostAndTrajectories()
{
	trajectoriesManager.reset(new TrajectoryDrawerManager);
	ghostDrawer.reset(new GhostSchemeDrawer(3, 10));

	//punkty dla ducha i trajektorii
	auto allPointsPositions = createPointsPositions(300);

	std::vector<std::vector<osg::Vec3>> pointsPositions(10);

	for(unsigned int i = 0; i < 10; ++i){
		pointsPositions[i] = allPointsPositions[i * 30];
	}

	ghostDrawer->init(pointsPositions,
		jointsMapping->generateMappedConnectionsDescription());
	ghostDrawer->pointsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->connectionsDrawer()->setColor(osg::Vec4(1.0f, 1.0f, 0.9f, 0.25f));
	ghostDrawer->pointsDrawer()->setSize(0.02);
	ghostDrawer->connectionsDrawer()->setSize(0.005);
    ghostDrawer->getNode()->setNodeMask(false);

	localRootNode->addChild(ghostDrawer->getNode());

	// teraz punkty dla ducha przerabiam na punkty dla trajektorii
	// przechodzę z klatek po czasie do klatek po stawach - generalnie transpozycja

	std::vector<std::vector<osg::Vec3>> trajectories(jointsMapping->mappedJointsNumber());

	for(auto it = allPointsPositions.begin(); it != allPointsPositions.end(); ++it){
		for(unsigned int i = 0; i < jointsMapping->mappedJointsNumber(); ++i){
			trajectories[i].push_back((*it)[i]);
		}
	}

	trajectoriesManager->initialize(trajectories);	
	trajectoriesManager->setVisible(false);
	trajectoriesManager->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	localRootNode->addChild(trajectoriesManager->getNode());
}

void SkeletonSerie::setGhostVisible(const bool visible)
{
	if(visible == true && ghostDrawer == nullptr){
		createGhostAndTrajectories();
	}

	ghostDrawer->getNode()->setNodeMask( visible == true ? 1 : 0);
}

utils::shared_ptr<TrajectoryDrawerManager> SkeletonSerie::getTrajectoriesManager() 
{
    if (!trajectoriesManager) {
        createGhostAndTrajectories();
    }
    return trajectoriesManager;
}

osg::Quat SkeletonStreamSerie::getQuatbyName(const std::string& name)
{
	UTILS_ASSERT(false);
	return osg::Quat();
}



//---------------------------------------------------------------------


class SkeletonStreamSerieUpdater : public utils::IStreamStatusObserver
{
public:
	SkeletonStreamSerieUpdater(SkeletonStreamSerie * serie)
		: serie(serie)
	{

	}

	virtual ~SkeletonStreamSerieUpdater() {}

	//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
	virtual void update()
	{
		//serie->requestUpdate();
		serie->myUpdate();
	}

private:

	SkeletonStreamSerie * serie;
};

std::string fbx2imu(const std::string& s) {
	
	/*if (s == "CMan0005-M3-CS:Bip01 L Hand") return "lHand_endSegment";
	if (s == "CMan0005-M3-CS:Bip01 L Forearm") return "lForeArm_lHand";
	if (s == "CMan0005-M3-CS:Bip01 L Thigh") return "lButtock_lShin";
	if (s == "CMan0005-M3-CS:Bip01 L UpperArm") return "lShldr_lForeArm";
	if (s == "CMan0005-M3-CS:Bip01 R Hand") return "rHand_endSegment";
	if (s == "CMan0005-M3-CS:Bip01 R Forearm") return "rForeArm_rHand";
	if (s == "CMan0005-M3-CS:Bip01 R Thigh") return "rButtock_rShin";
	if (s == "CMan0005-M3-CS:Bip01 R UpperArm") return "rShldr_rForeArm";*/

	////////////////if (s == "CMan0005-M3-CS:Bip01 R Calf") return "rShin_rFoot";
	////////////////
	//////////////////if (s == "CMan0005-M3-CS:Bip01 L UpperArm") return "lShldr_lForeArm";
	////////////////if (s == "CMan0005-M3-CS:Bip01 L ForeArm") return "lForeArm_lHand";
	////////////////if (s == "CMan0005-M3-CS:Bip01 L Hand") return "lHand_endSegment";
	//////////////////if (s == "CMan0005-M3-CS:Bip01 R UpperArm") return "rShldr_rForeArm";
	////////////////if (s == "CMan0005-M3-CS:Bip01 R ForeArm") return "rForeArm_rHand";
	////////////////if (s == "CMan0005-M3-CS:Bip01 R Hand") return "rHand_endSegment";
	////////////////
	////////////////return "";
	/*if (s == "CMan0005-M3-CS:Bip01 R Toe0") return "rToe_endSegment";
	if (s == "CMan0005-M3-CS:Bip01 R Foot") return "rFoot_rToe";
	if (s == "CMan0005-M3-CS:Bip01 R Calf") return "rShin_rFoot";
	if (s == "CMan0005-M3-CS:Bip01 R Thigh") return "rButtoc_rShin";


	if (s == "CMan0005-M3-CS:Bip01 L Foot") return "lToe_endSegment";
	if (s == "CMan0005-M3-CS:Bip01 L Calf") return "lFoot_lToe";
	if (s == "CMan0005-M3-CS:Bip01 L Thigh")return "lShin_lFoot";
	return "";*/

	////if (mb=="CMan0005-M3-CS:Bip01") return "Hips";
	////if (mb=="CMan0005-M3-CS:Bip01 Spine") return "Hips";*/
	//if (mb=="CMan0005-M3-CS:Bip01 L Calf") return "LeftUpLeg";
	//if (mb=="CMan0005-M3-CS:Bip01 L Foot") return "LeftLeg";
	//if (mb=="CMan0005-M3-CS:Bip01 L Toe") return "LeftFoot";
	//if (mb=="CMan0005-M3-CS:Bip01 L Toe0Nub") return "LeftToeBase";
	////if (mb=="CMan0005-M3-CS:Bip01 L Toe0Nub") return "L_Toe_End";
	//if (mb=="CMan0005-M3-CS:Bip01 R Calf") return "RightUpLeg";
	//if (mb=="CMan0005-M3-CS:Bip01 R Foot") return "RightLeg";
	//if (mb=="CMan0005-M3-CS:Bip01 R Toe0") return "RightFoot";
	//if (mb=="CMan0005-M3-CS:Bip01 R Toe0Nub") return "RightToeBase";
	////if (mb=="CMan0005-M3-CS:Bip01 R Toe0Nub") return "R_Toe_End";
	//if (mb=="CMan0005-M3-CS:Bip01 Spine1") return "Spine";
	//if (mb=="CMan0005-M3-CS:Bip01 Neck") return "Spine1";
	//if (mb=="CMan0005-M3-CS:Bip01 Head") return "Head";
	////if (mb=="CMan0005-M3-CS:Bip01 Head") return "Head_End";
	//if (mb=="CMan0005-M3-CS:Bip01 L Clavicle") return "LeftShoulder";
	//if (mb=="CMan0005-M3-CS:Bip01 L UpperArm") return "LeftArm";
	//if (mb=="CMan0005-M3-CS:Bip01 L Forearm") return "LeftForeArm";
	//if (mb=="CMan0005-M3-CS:Bip01 L Hand") return "LeftHand";
	//if (mb=="CMan0005-M3-CS:Bip01 L Finger0") return "LeftHandThumb";
	//if (mb=="CMan0005-M3-CS:Bip01 L Finger02") return "L_Thumb_End";
	//if (mb=="CMan0005-M3-CS:Bip01 L Finger3") return "L_Wrist_End";
	//if (mb=="CMan0005-M3-CS:Bip01 R Clavicle") return "RightShoulder";
	//if (mb=="CMan0005-M3-CS:Bip01 R UpperArm") return "RightArm";
	//if (mb=="CMan0005-M3-CS:Bip01 R Forearm") return "RightForeArm";
	//if (mb=="CMan0005-M3-CS:Bip01 R Hand") return "RightHand";
	//if (mb=="CMan0005-M3-CS:Bip01 R Finger0") return "RightHandThumb";
	//if (mb=="CMan0005-M3-CS:Bip01 R Finger02") return "R_Thumb_End";
	//if (mb=="CMan0005-M3-CS:Bip01 R Finger3") return "R_Wrist_End";

	/*if (s=="CMan0005-M3-CS:Bip01 L Calf") return "LeftUpLeg";
	if (s=="CMan0005-M3-CS:Bip01 L Foot") return "LeftLeg";
	if (s=="CMan0005-M3-CS:Bip01 L Toe") return "LeftFoot";
	if (s=="CMan0005-M3-CS:Bip01 L Toe0Nub") return "LeftToeBase";
	if (s=="CMan0005-M3-CS:Bip01 R Calf") return "RightUpLeg";
	if (s=="CMan0005-M3-CS:Bip01 R Foot") return "RightLeg";
	if (s=="CMan0005-M3-CS:Bip01 R Toe0") return "RightFoot";
	if (s=="CMan0005-M3-CS:Bip01 R Toe0Nub") return "RightToeBase";
	if (s=="CMan0005-M3-CS:Bip01 Spine1") return "Spine";
	if (s=="CMan0005-M3-CS:Bip01 Neck") return "Spine1";
	if (s=="CMan0005-M3-CS:Bip01 Head") return "Head";
	if (s=="CMan0005-M3-CS:Bip01 L Clavicle") return "LeftShoulder";
	if (s=="CMan0005-M3-CS:Bip01 L UpperArm") return "LeftArm";
	if (s=="CMan0005-M3-CS:Bip01 L Forearm") return "LeftForeArm";
	if (s=="CMan0005-M3-CS:Bip01 L Hand") return "LeftHand";
	if (s=="CMan0005-M3-CS:Bip01 L Finger0") return "LeftHandThumb";
	if (s=="CMan0005-M3-CS:Bip01 L Finger02") return "L_Thumb_End";
	if (s=="CMan0005-M3-CS:Bip01 L Finger3") return "L_Wrist_End";
	if (s=="CMan0005-M3-CS:Bip01 R Clavicle") return "RightShoulder";
	if (s=="CMan0005-M3-CS:Bip01 R UpperArm") return "RightArm";
	if (s=="CMan0005-M3-CS:Bip01 R Forearm") return "RightForeArm";
	if (s=="CMan0005-M3-CS:Bip01 R Hand") return "RightHand";
	if (s=="CMan0005-M3-CS:Bip01 R Finger0") return "RightHandThumb";
	if (s=="CMan0005-M3-CS:Bip01 R Finger02") return "R_Thumb_End";
	if (s=="CMan0005-M3-CS:Bip01 R Finger3") return "R_Wrist_End";*/

	if (s=="CMan0005-M3-CS:Bip01 Head") return "neck_head"; 
	if (s == "CMan0005-M3-CS:Bip01 Neck") return "chest_neck";
	if (s == "CMan0005-M3-CS:Bip01 L Clavicle") return "chest_lCollar";
	if (s == "CMan0005-M3-CS:Bip01 R Clavicle") return "chest_rCollar";
	if (s=="CMan0005-M3-CS:Bip01 Spine3") return "prechest_chest";
	if (s=="CMan0005-M3-CS:Bip01 Spine2") return "abdomen2_prechest";
	if (s=="CMan0005-M3-CS:Bip01 Spine1") return "abdomen_abdomen2";
	if (s=="CMan0005-M3-CS:Bip01 HeadNub") return "head_endSegment";
	
	if (s=="CMan0005-M3-CS:Bip01 R UpperArm") return "rCollar_rShldr";
	if (s=="CMan0005-M3-CS:Bip01 R ForeArm") return "rShldr_rForeArm";
	if (s=="CMan0005-M3-CS:Bip01 R Hand") return "rForeArm_rHand";
	if (s=="CMan0005-M3-CS:Bip01 R Finger0") return "rHand_endSegment";

	if (s == "CMan0005-M3-CS:Bip01 L UpperArm") return "lCollar_lShldr";
	if (s == "CMan0005-M3-CS:Bip01 L ForeArm") return "lShldr_lForeArm";
	if (s == "CMan0005-M3-CS:Bip01 L Hand") return "lForeArm_lHand";
	if (s == "CMan0005-M3-CS:Bip01 L Finger0") return "lHand_endSegment";
	if (s=="CMan0005-M3-CS:Bip01 L Thigh") return "hip_lButtock"; 
	if (s=="CMan0005-M3-CS:Bip01 R Thigh") return "hip_rButtock"; 

	if (s=="CMan0005-M3-CS:Bip01 R Calf") return "rButtock_rShin";
	if (s=="CMan0005-M3-CS:Bip01 R Foot") return "rShin_rFoot";  
	if (s=="CMan0005-M3-CS:Bip01 R Toe0") return "rFoot_rToe"; 
	if (s=="CMan0005-M3-CS:Bip01 R Toe0Nub") return "rToe_endSegment";

	if (s=="CMan0005-M3-CS:Bip01 L Calf") return "lButtock_lShin";
	if (s=="CMan0005-M3-CS:Bip01 L Foot") return "lShin_lFoot";  
	if (s=="CMan0005-M3-CS:Bip01 L Toe0") return "lFoot_lToe";  
	if (s=="CMan0005-M3-CS:Bip01 L Toe0Nub") return "lToe_endSegment";

	if (s=="CMan0005-M3-CS:Bip01 Spine") return "hip_abdomen"; 

	return "";									   
}

std::map<std::string, osgAnimation::Bone*> createBoneMap(osg::Node* n)
{
	std::map<std::string, osgAnimation::Bone*> boneMap;
	osgAnimation::Bone* b = dynamic_cast<osgAnimation::Bone*>(n);
	osg::Group* g = b ? b : dynamic_cast<osg::Group*>(n);

	/*if (!b)	{
		return;
	}*/ 

	if (b) {
		boneMap[b->getName()] = b;
	}
	if (g) {
		for (int i = 0; i < g->getNumChildren(); ++i) {
			auto bm = createBoneMap(g->getChild(i));
			boneMap.insert(bm.begin(), bm.end());
		}
	}

	return boneMap;
}


class BoneVisitor : public osg::NodeVisitor {

public:
	BoneVisitor(SkeletonStreamSerie * serie) : 
		NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN), serie(serie) {}

	virtual void apply(osg::Node& node)
	{
		osgAnimation::Bone* b = dynamic_cast<osgAnimation::Bone*>(&node);
		if (b)	{
			std::string name = fbx2imu(b->getName());
			osg::Quat quat = serie->getQuatbyName(name);
		}
		this->traverse(node);

	}

private:
	SkeletonStreamSerie * serie;

};
//class ModelCallBack : public osg::NodeCallback {
//public:
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::ref_ptr<ModelDataType> modelData =
//			dynamic_cast<ModelDataType*> (node->getUserData());
//		if (modelData)
//			modelData->updateModelSkeleton();
//		osg::NodeCallback::traverse(node, nv);
//	}
//
//};

SkeletonStreamSerie::SkeletonStreamSerie( KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data ) : 
visualizer(visualizer),
	data(data), requestedType(requestedType),	
	xyzAxis(false),
	name("SkeletonData"),
	pointsDrawer(new PointsDrawer(3)),
	connectionsDrawer(new ConnectionsDrawer(10)),
	heightCompensation(false),
	localRootNode(new osg::PositionAttitudeTransform),
	root(nullptr)
{	
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(SkeletonDataStream));
	data->getMetadata("core/name", name);	
	skeletalData = data->get();
	
	pointsDrawer->init(skeletalData->jointsCount);
	pointsDrawer->setSize(0.02);

	connectionsDrawer->init(skeletalData->connections);
	connectionsDrawer->setSize(0.005);

	pointsDrawer->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	connectionsDrawer->setColor(osg::Vec4(0.0, 1.0, 0.0, 0.5));

	localRootNode->addChild(pointsDrawer->getNode());
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	//setAxis(true);

	updater.reset(new SkeletonStreamSerieUpdater(this));

	skeletalData->jointsStream->attachObserver(updater);
	
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("C:/Users/Wojciech/Desktop/man.fbx");
	osg::ref_ptr<osg::PositionAttitudeTransform> paModel = new osg::PositionAttitudeTransform;
	boneMap = createBoneMap(model);
	root = boneMap.begin()->second;
	paModel->addChild(model);
	paModel->setScale(osg::Vec3(0.025, 0.025, 0.025));
	paModel->setPosition(osg::Vec3(1, 0, 0));
	paModel->setAttitude(osg::Quat(3.1415 / 2, osg::Vec3(1, 0, 0), 0, osg::Vec3(0, 1, 0), 0, osg::Vec3(0, 0, 1)));
	matrixTransform->addChild(paModel);
}

SkeletonStreamSerie::~SkeletonStreamSerie()
{
	if(skeletalData != nullptr){
		skeletalData->jointsStream->detachObserver(updater);
	}
}

void SkeletonStreamSerie::setAxis( bool xyz)
{
	//TODO - sprawdzic jak to dziala
	xyzAxis = xyz;	
	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}

void SkeletonStreamSerie::setName( const std::string & name )
{
	this->name = name;
}

const std::string SkeletonStreamSerie::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonStreamSerie::getData() const
{
	return data;
}

void SkeletonStreamSerie::update()
{

	std::vector<osg::Vec3> points;
	skeletalData->jointsStream->data(points);

	if(heightCompensation == false){
		heightCompensation = true;
		float minZ = std::numeric_limits<float>::max();
		for(auto it = points.begin(); it != points.end(); ++it){
			minZ = std::min(minZ, (*it).z());
		}

		minZ -= pointsDrawer->size(0);

		localRootNode->setPosition(osg::Vec3(0, 0, -minZ));
		localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
	}

	rootPosition = osg::Vec3(0,0,0);

	for(auto it = points.begin(); it != points.end(); ++it){
		rootPosition += *it;
	}

	rootPosition /= points.size();

	pointsDrawer->update(points);
	connectionsDrawer->update(points);

	std::vector<osg::Quat> quats;
	skeletalData->quatStream->data(quats);

	traverse(root, quats, points);
	
}

const utils::TypeInfo & SkeletonStreamSerie::getRequestedDataType() const
{
	return requestedType;
}

const osg::Vec3 SkeletonStreamSerie::pivotPoint() const
{
	return rootPosition * lToW;
}


void SkeletonStreamSerie::traverse(osgAnimation::Bone* b, const std::vector<osg::Quat>& quats, const std::vector<osg::Vec3>& points)
{
	if (!b) {
		return;
	}

	std::string name = fbx2imu(b->getName());
	if (!name.empty()) {
		auto findIt = skeletalData->segmentNames.find(name);
		if (findIt != skeletalData->segmentNames.end() && b->getBoneParent()) {
			int qidx = findIt->second;
			auto v = kinematic::SkeletonUtils::getEulerFromQuat(quats[qidx]);
			/*static std::vector<osg::Vec3> eu;
			eu.push_back(v);
			if (eu.size() == 200) {
				std::ofstream f("E:/katy.csv");
				for (auto& v : eu)
					f << v.x() << ", " << v.y() << ", " << v.z() << std::endl;
				f.close();
			}*/
			static int  z = 0;
			switch (z) {
			case 0: break;
			case 1: v = osg::Vec3(v.x(), v.y(), -v.z());	break;
			case 2: v = osg::Vec3(v.x(), -v.y(), v.z());	break;
			case 3: v = osg::Vec3(v.x(), -v.y(), -v.z()); break;
			case 4: v = osg::Vec3(-v.x(), v.y(), v.z());	break;
			case 5: v = osg::Vec3(-v.x(), v.y(), -v.z()); break;
			case 6: v = osg::Vec3(-v.x(), -v.y(), v.z()); break;
			case 7: v = osg::Vec3(-v.x(), -v.y(), -v.z()); break;

			case 10: v = osg::Vec3(v.x(), v.z(), v.y());	break;
			case 11: v = osg::Vec3(v.x(), v.z(), -v.y());	break;
			case 12: v = osg::Vec3(v.x(), -v.z(), v.y());	break;
			case 13: v = osg::Vec3(v.x(), -v.z(), -v.y()); break;
			case 14: v = osg::Vec3(-v.x(), v.z(), v.y());	break;
			case 15: v = osg::Vec3(-v.x(), v.z(), -v.y()); break;
			case 16: v = osg::Vec3(-v.x(), -v.z(), v.y()); break;
			case 17: v = osg::Vec3(-v.x(), -v.z(), -v.y()); break;


			case 20: v = osg::Vec3( v.x(), -v.y(),  v.z()); break;
			case 21: v = osg::Vec3( v.x(), -v.y(), -v.z()); break;
			case 22: v = osg::Vec3(-v.x(), -v.y(),  v.z()); break;
			case 23: v = osg::Vec3(-v.x(), -v.y(), -v.z()); break;

			case 24: v = osg::Vec3( v.z(), -v.y(),  v.x()); break;
			case 25: v = osg::Vec3( v.z(), -v.y(), -v.x()); break;
			case 26: v = osg::Vec3(-v.z(), -v.y(),  v.x()); break;

			}

			/*static float angle = 0.0f;
			static float delta = 3.1415 / 360;

			angle += delta;
			if (angle > 30 * 3.1415 / 180) {
				delta *= -1;
				angle += delta;
			}
			else if (angle < 0) {
				angle = 0;
				delta *= -1;
			}

			static int  a = 0;
			switch (a) {
			case 0: v = osg::Vec3(angle, 0, 0); break;
			case 1: v = osg::Vec3(0, angle, 0);	break;
			case 2: v = osg::Vec3(0, 0, angle);	break;
			case 3: v = osg::Vec3(-angle, 0, 0); break;
			case 4: v = osg::Vec3(0, -angle, 0);	break;
			case 5: v = osg::Vec3(0, 0, -angle);	break;
			}*/

			auto q = kinematic::SkeletonUtils::getQuatFromEuler(v);
			osg::Matrix r = osg::Matrix(q);
			//osg::Matrix m = r*osg::Matrix::translate(b->getMatrixInBoneSpace().getTrans())*b->getBoneParent()->getMatrixInSkeletonSpace();
			osg::Matrix m =  getGlobalTranslation(name, points);
			b->setMatrixInSkeletonSpace(m);
		}
	} else {
		if (b->getBoneParent())
		{
			osg::Matrix m = getGlobalTranslation(name, points); //b->getMatrixInBoneSpace()*b->getBoneParent()->getMatrixInSkeletonSpace();
			b->setMatrixInSkeletonSpace(m);
		}

		/*else
		{
			m_skeletonState->SkeletonRotation(GL_segment_name, q);
			m_skeletonState->SkeletonTranslation(GL_segment_name, t);
			osg::Matrix r(q);
			osg::Matrix m = r*osg::Matrix::translate(b->getMatrixInBoneSpace().getTrans());
			b->setMatrixInSkeletonSpace(m);
		}*/
	}


	for (int i = 0; i < b->getNumChildren(); ++i) {
		traverse(dynamic_cast<osgAnimation::Bone*>(b->getChild(i)), quats, points);
	}
}

osg::Matrix SkeletonStreamSerie::getGlobalTranslation(const std::string& name, const std::vector<osg::Vec3>& points)
{
	int idx = 0;
	if (name == "neck_head") idx = 5;
	if (name == "chest_neck") idx = 4;
	if (name == "chest_lCollar") idx = 4;
	if (name == "chest_rCollar") idx = 4;
	if (name == "prechest_chest") idx = 3;
	if (name == "abdomen2_prechest") idx = 2;
	if (name == "abdomen_abdomen2") idx = 1;
	if (name == "head_endSegment") idx = 6;

	if (name == "rCollar_rShldr") idx = 8;
	if (name == "rShldr_rForeArm") idx = 9;
	if (name == "rForeArm_rHand") idx = 10;
	if (name == "rHand_endSegment") idx = 11;

	if (name == "lCollar_lShldr") idx = 13;
	if (name == "lShldr_lForeArm") idx = 14;
	if (name == "lForeArm_lHand") idx = 15;
	if (name == "lHand_endSegment") idx = 16;
	if (name == "hip_lButtock") idx = 0;
	if (name == "hip_rButtock") idx = 0;

	if (name == "rButtock_rShin") idx = 18;
	if (name == "rShin_rFoot") idx = 19;
	if (name == "rFoot_rToe") idx = 20;
	if (name == "rToe_endSegment") idx = 21;

	if (name == "lButtock_lShin") idx = 23;
	if (name == "lShin_lFoot") idx = 24;
	if (name == "lFoot_lToe") idx = 25;
	if (name == "lToe_endSegment") idx = 26;

	if (name == "hip_abdomen") idx = 0;

	return osg::Matrix::translate(points[idx]);
}
