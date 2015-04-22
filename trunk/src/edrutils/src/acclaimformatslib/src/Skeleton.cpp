#include <acclaimformatslib/Skeleton.h>

using namespace acclaim;

Skeleton::Units::Units()
{
	defaultValues["length"] = 1.0;
	defaultValues["mass"] = 1.0;
	setAngleType(kinematicUtils::Deg);
}

Skeleton::Units::~Units()
{

}

//! \return Czy k¹t podawany jest w radianach
bool Skeleton::Units::isAngleInRadians() const {
	bool ret = false;
	auto angleIt = typeValueUnits.find("angle");
	if (angleIt != typeValueUnits.end()){
		ret = (angleIt->second == "rad");
	}

	return ret;
}

void Skeleton::Units::setAngleType(const kinematicUtils::AngleUnitType angleType)
{
	typeValueUnits["angle"] = (angleType == kinematicUtils::Deg ? "deg" : "rad");
}

Skeleton::Skeleton():
    position(0.0, 0.0, 0.0),
    orientation(0.0, 0.0, 0.0),
	axisOrder(kinematicUtils::AxisOrder::XYZ),
	dataOrder({ kinematicUtils::ChannelType::TX, kinematicUtils::ChannelType::TY, kinematicUtils::ChannelType::TZ,
	kinematicUtils::ChannelType::RZ, kinematicUtils::ChannelType::RY, kinematicUtils::ChannelType::RX }),
	version("1.10"),
	name("unnamed"),
	root(-1)
{
	
}

Skeleton::~Skeleton()
{

}

kinematicUtils::AxisOrder::Type Skeleton::rotationOrder() const
{
	std::string s;
	int count = dataOrder.size();
	for (int i = 0; i < count && s.size() < 3; ++i) {
		if (dataOrder[i] == kinematicUtils::ChannelType::RX) {
			s += "X";
		}
		else if (dataOrder[i] == kinematicUtils::ChannelType::RY) {
			s += "Y";
		}
		else if (dataOrder[i] == kinematicUtils::ChannelType::RZ) {
			s += "Z";
		}
	}
	return Axis::getAxisOrder(s);
}

unsigned int Skeleton::activeBones() const
{	
	return std::count_if(bones.begin(), bones.end(), [](const Bones::value_type & bd)
	{
		return bd.second.isActive() == true;
	});
}

//double SkeletonUtils::getMaxBoneLength(const Skeleton& skeleton) 
//{
//	JointConstPtr root = skeleton.getRoot();
//	return getMaxLength(root, -1.0);
//}
//
//double SkeletonUtils::getMaxLength(const JointConstPtr & joint, double maxLength)
//{
//	maxLength = std::max(maxLength, joint->length);
//	for (int i = joint->children.size() - 1; i >= 0; --i) {
//		maxLength = std::max(maxLength, getMaxLength(joint->children[i], maxLength));
//	}
//	return maxLength;
//}
//
//osg::Quat SkeletonUtils::createRotation( const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order )
//{
//	switch (order) 
//	{
//	case Axis::UnknownAxisOrder:
//	case Axis::XYZ: return rX * rY * rZ;
//	case Axis::XZY: return rX * rZ * rY;
//	case Axis::YXZ: return rY * rX * rZ;
//	case Axis::YZX: return rY * rZ * rX;
//	case Axis::ZXY: return rZ * rX * rY;
//	case Axis::ZYX: return rZ * rY * rX;
//	default:
//		UTILS_ASSERT(false);
//		osg::Quat q;
//		return q;
//	}
//}