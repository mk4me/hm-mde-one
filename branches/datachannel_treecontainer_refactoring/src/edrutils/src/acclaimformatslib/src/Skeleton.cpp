#include <acclaimformatslib/Skeleton.h>

using namespace acclaim;

Skeleton::Units::Units()
{
	defaultValues["length"] = 0.1;
	defaultValues["mass"] = 1.0;
	setAngleType(kinematicUtils::Deg);
}

Skeleton::Units::~Units()
{

}

//! \return Czy k¹t podawany jest w radianach
const bool Skeleton::Units::isAngleInRadians() const {
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
	axisOrder(Bone::defaultAxisOrder),
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

const kinematicUtils::AxisOrder::Type Skeleton::rotationOrder(const Skeleton & skeleton)
{
	std::string s;
	int count = skeleton.dataOrder.size();
	for (int i = 0; i < count && s.size() < 3; ++i) {
		if (skeleton.dataOrder[i] == kinematicUtils::ChannelType::RX) {
			s += "X";
		}
		else if (skeleton.dataOrder[i] == kinematicUtils::ChannelType::RY) {
			s += "Y";
		}
		else if (skeleton.dataOrder[i] == kinematicUtils::ChannelType::RZ) {
			s += "Z";
		}
	}
	return Axis::getAxisOrder(s);
}

const std::size_t Skeleton::activeBones(const Skeleton & skeleton)
{	
	return std::count_if(skeleton.bones.begin(), skeleton.bones.end(), [](const Bones::value_type & bd)
	{
		return bd.second.isActive() == true;
	});
}

Skeleton::HelperMotionData Skeleton::helperMotionData(const Skeleton & skeleton)
{
	HelperMotionData ret;	
	const bool angleInRadians = skeleton.units.isAngleInRadians();
	for (const auto & bd : skeleton.bones)
	{
		ret.insert(ret.end(), { bd.first, Bone::helperData(bd.second, angleInRadians) });
	}

	return ret;
}

Skeleton::Mapping Skeleton::createMapping(const Bones & bones)
{
	Mapping ret;	
	for (const auto & bd : bones)
	{
		ret.insert(ret.end(), { bd.first, bd.second.name });
	}

	return ret;
}