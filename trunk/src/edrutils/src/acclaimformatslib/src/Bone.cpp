#include <acclaimformatslib/Bone.h>
#include <kinematicUtils/RotationConverter.h>

using namespace acclaim;

const osg::Vec3d Bone::defaultDirection = osg::Vec3d(0, 1, 0);
const osg::Vec3d Bone::defaultAxis = osg::Vec3d(0, 0, 0);

Bone::Bone() : id(-1), axis(0.0, 0.0, 0.0), length(-1),
axisOrder(kinematicUtils::AxisOrder::XYZ), direction(0.0, 0.0, 0.0)
{

}

Bone::Bone(const Bone & Other) : id(Other.id), name(Other.name), 
direction(Other.direction), length(Other.length), axis(Other.axis),
axisOrder(Other.axisOrder), dofs(Other.dofs)
{

}

Bone::Bone(Bone && Other) : id(Other.id), name(std::move(Other.name)),
direction(Other.direction), length(Other.length), axis(Other.axis),
axisOrder(Other.axisOrder), dofs(std::move(Other.dofs))
{
	Other.id = -1;
	Other.direction = osg::Vec3(0, 0, 0);
	Other.length = -1;
	Other.axis = osg::Vec3(0, 0, 0);
}

Bone::~Bone()
{

}

const bool Bone::isActive() const
{
	return dofs.empty() == false;
}

const kinematicUtils::AxisOrder::Type Bone::rotationOrder() const
{
	std::string s;
	int count = dofs.size();
	for (int i = 0; i < count && s.size() < 3; ++i) {
		if (dofs[i].channel == kinematicUtils::ChannelType::RX) {
			s += "X";
		}
		else if (dofs[i].channel == kinematicUtils::ChannelType::RY) {
			s += "Y";
		}
		else if (dofs[i].channel == kinematicUtils::ChannelType::RZ) {
			s += "Z";
		}
	}
	return Axis::getAxisOrder(s);
}

Bone::HelperMotionData Bone::helperData(const Bone & bone, const bool angleInRadians)
{
	osg::Quat c = kinematicUtils::convert(angleInRadians == true ? bone.axis : kinematicUtils::toRadians(bone.axis), bone.axisOrder);
	osg::Quat cInv = c.inverse();

	return{ c, cInv };
}