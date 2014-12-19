#include <kinematicUtils/RotationConverter.h>
#include <osg/Math>
#include <boost/lexical_cast.hpp>

using namespace kinematicUtils;

AxisOrder axisOrder(const ChannelTriplet & channelTriplet)
{
	AxisOrder ret = XYZ;

	if (channelTriplet[0] == RX && channelTriplet[1] == RY && channelTriplet[2] == RZ){
		ret = XYZ;
	}
	else if (channelTriplet[0] == RX && channelTriplet[1] == RY && channelTriplet[2] == RX){
		ret = XYX;
	}
	else if(channelTriplet[0] == RX && channelTriplet[1] == RZ && channelTriplet[2] == RX){
		ret = XZX;
	}
	else if (channelTriplet[0] == RX && channelTriplet[1] == RZ && channelTriplet[2] == RY){
		ret = XZY;
	}
	else if (channelTriplet[0] == RY && channelTriplet[1] == RX && channelTriplet[2] == RY){
		ret = YXY;
	}
	else if (channelTriplet[0] == RY && channelTriplet[1] == RX && channelTriplet[2] == RZ){
		ret = YXZ;
	}
	else if (channelTriplet[0] == RY && channelTriplet[1] == RZ && channelTriplet[2] == RX){
		ret = YZX;
	}
	else if (channelTriplet[0] == RY && channelTriplet[1] == RZ && channelTriplet[2] == RY){
		ret = YZY;
	}
	else if (channelTriplet[0] == RZ && channelTriplet[1] == RX && channelTriplet[2] == RY){
		ret = ZXY;
	}
	else if (channelTriplet[0] == RZ && channelTriplet[1] == RX && channelTriplet[2] == RZ){
		ret = ZXZ;
	}
	else if (channelTriplet[0] == RZ && channelTriplet[1] == RY && channelTriplet[2] == RX){
		ret = ZYX;
	}
	else if (channelTriplet[0] == RZ && channelTriplet[1] == RY && channelTriplet[2] == RZ){
		ret = ZYZ;
	}
	else {	
		throw std::runtime_error("Invalid channel type or axis configuration for decoding axis order: " + boost::lexical_cast<std::string>(channelTriplet[0]));
	}

	return ret;
}

template<osg::Vec3::value_type Transform(osg::Vec3::value_type)>
osg::Vec3 transform(const osg::Vec3 & rotation)
{
	return osg::Vec3(Transform(rotation.x()),
		Transform(rotation.y()),
		Transform(rotation.z()));
}

osg::Vec3::value_type mulReduce(const osg::Vec3 & rotation)
{
	return rotation.x() * rotation.y() * rotation.z();
}

osg::Quat kinematicUtils::convertXYX(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertXYZ(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(	
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.y() + sinHalfAngle.y() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) + mulReduce(sinHalfAngle)
		);
}


osg::Quat kinematicUtils::convertXZX(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertXZY(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		mulReduce(cosHalfAngle) - mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertYXY(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertYXZ(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertYZX(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.z(),
		mulReduce(cosHalfAngle) + mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertYZY(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertZXY(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) + mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertZXZ(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertZYX(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) - mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertZYZ(const osg::Vec3 & rotation)
{
	const osg::Vec3 halfAngle(rotation / 2.0);
	const osg::Vec3 cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3 sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(	
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat convert(const osg::Vec3 & rotation, const AxisOrder axisOrder)
{
	osg::Quat ret(0, 0, 0, 1);

	switch (axisOrder)
	{
	case XYX:

		ret = convertXYX(rotation);
		break;

	case XYZ:

		ret = convertXYZ(rotation);
		break;

	case XZX:

		ret = convertXZX(rotation);
		break;

	case XZY:

		ret = convertXZY(rotation);
		break;

	case YXY:

		ret = convertYXY(rotation);
		break;

	case YXZ:

		ret = convertYXZ(rotation);
		break;

	case YZX:

		ret = convertYZX(rotation);
		break;

	case YZY:

		ret = convertYZY(rotation);
		break;
		
	case ZXY:

		ret = convertZXY(rotation);
		break;

	case ZXZ:

		ret = convertZXZ(rotation);
		break;

	case ZYX:

		ret = convertZYX(rotation);
		break;

	case ZYZ:

		ret = convertZYZ(rotation);
		break;
	}

	return ret;
}

osg::Vec3 kinematicUtils::convertXYX(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z()), 2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y())),
		std::acosf(std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2)),
		std::atan2f(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), 2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()))
		);
}

osg::Vec3 kinematicUtils::convertXYZ(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2) - std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2)),
		-std::asinf(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y())),
		std::atan2f(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.y(), 2))
		);
}

osg::Vec3 kinematicUtils::convertXZX(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), 2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z())),
		std::acosf(std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.y(), 2)),
		std::atan2f(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y()), 2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()))
		);
}

osg::Vec3 kinematicUtils::convertXZY(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::powf(rotation.y(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.x(), 2)),
		std::asinf(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z())),
		std::atan2f(2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2))
		);
}

osg::Vec3 kinematicUtils::convertYXY(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), 2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x())),
		std::acosf(std::powf(rotation.y(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.x(), 2)),
		std::atan2f(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z()), 2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()))
		);
}

osg::Vec3 kinematicUtils::convertYXZ(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2) - std::powf(rotation.x(), 2) - std::powf(rotation.w(), 2)),
		std::asinf(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x())),
		std::atan2f(2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::powf(rotation.y(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.x(), 2))
		);
}

osg::Vec3 kinematicUtils::convertYZX(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2)),
		-std::asinf(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z())),
		std::atan2f(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::powf(rotation.y(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.x(), 2))
		);
}

osg::Vec3 kinematicUtils::convertYZY(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x()), 2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z())),
		std::acosf(std::powf(rotation.y(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.x(), 2)),
		std::atan2f(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), 2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()))
		);
}

osg::Vec3 kinematicUtils::convertZXY(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::powf(rotation.y(), 2) - std::powf(rotation.z(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.x(), 2)),
		-std::asinf(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x())),
		std::atan2f(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2) - std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2))
		);
}

osg::Vec3 kinematicUtils::convertZXZ(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y()), 2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x())),
		std::acosf(std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2) - std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2)),
		std::atan2f(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), 2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()))
		);
}

osg::Vec3 kinematicUtils::convertZYX(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2) - std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2)),
		std::asinf(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y())),
		std::atan2f(2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2) - std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2))
		);
}

osg::Vec3 kinematicUtils::convertZYZ(const osg::Quat & rotation)
{
	return osg::Vec3(
		std::atan2f(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), 2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y())),
		std::acosf(std::powf(rotation.z(), 2) - std::powf(rotation.y(), 2) - std::powf(rotation.x(), 2) + std::powf(rotation.w(), 2)),
		std::atan2f(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x()), 2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()))
		);
}

osg::Vec3 convert(const osg::Quat & rotation, const AxisOrder axisOrder)
{
	osg::Vec3 ret(0, 0, 0);

	switch (axisOrder)
	{
	case XYX:

		ret = convertXYX(rotation);
		break;

	case XYZ:

		ret = convertXYZ(rotation);
		break;

	case XZX:

		ret = convertXZX(rotation);
		break;

	case XZY:

		ret = convertXZY(rotation);
		break;

	case YXY:

		ret = convertYXY(rotation);
		break;

	case YXZ:

		ret = convertYXZ(rotation);
		break;

	case YZX:

		ret = convertYZX(rotation);
		break;

	case YZY:

		ret = convertYZY(rotation);
		break;

	case ZXY:

		ret = convertZXY(rotation);
		break;

	case ZXZ:

		ret = convertZXZ(rotation);
		break;

	case ZYX:

		ret = convertZYX(rotation);
		break;

	case ZYZ:

		ret = convertZYZ(rotation);
		break;
	}

	return ret;
}

osg::Vec3 toRadians(const osg::Vec3 & rotation)
{
	return transform<&osg::DegreesToRadians>(rotation);
}

osg::Vec3 toDegrees(const osg::Vec3 & rotation)
{
	return transform<&osg::RadiansToDegrees>(rotation);
}