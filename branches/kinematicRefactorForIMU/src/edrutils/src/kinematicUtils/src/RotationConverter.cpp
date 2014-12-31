#include <kinematicUtils/RotationConverter.h>
#include <osg/Math>
#include <boost/lexical_cast.hpp>

using namespace kinematicUtils;

template<osg::Vec3d::value_type Transform(osg::Vec3d::value_type)>
osg::Vec3d transform(const osg::Vec3d & rotation)
{
	return osg::Vec3d(Transform(rotation.x()),
		Transform(rotation.y()),
		Transform(rotation.z()));
}

osg::Vec3d::value_type mulReduce(const osg::Vec3d & rotation)
{
	return rotation.x() * rotation.y() * rotation.z();
}

osg::Quat kinematicUtils::convertXYX(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertXYZ(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(	
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) + mulReduce(sinHalfAngle)
		);
}


osg::Quat kinematicUtils::convertXZX(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertXZY(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		mulReduce(cosHalfAngle) - mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertYXY(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertYXZ(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		mulReduce(cosHalfAngle) - mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertYZX(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		mulReduce(cosHalfAngle) + mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertYZY(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertZXY(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) + mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertZXZ(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() - sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convertZYX(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z(),
		cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) - mulReduce(sinHalfAngle)
		);
}

osg::Quat kinematicUtils::convertZYZ(const osg::Vec3d & rotation)
{
	const osg::Vec3d halfAngle(rotation / 2.0);
	const osg::Vec3d cosHalfAngle(transform<std::cos>(halfAngle));
	const osg::Vec3d sinHalfAngle(transform<std::sin>(halfAngle));
	return osg::Quat(	
		-cosHalfAngle.x() * sinHalfAngle.y() * sinHalfAngle.z() + sinHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y(),
		cosHalfAngle.x() * cosHalfAngle.z() * sinHalfAngle.y() + mulReduce(sinHalfAngle),
		cosHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z() + cosHalfAngle.y() * cosHalfAngle.z() * sinHalfAngle.x(),
		mulReduce(cosHalfAngle) - sinHalfAngle.x() * cosHalfAngle.y() * sinHalfAngle.z()
		);
}

osg::Quat kinematicUtils::convert(const osg::Vec3d & rotation, const AxisOrder::Type axisOrder)
{
	osg::Quat ret(0, 0, 0, 1);

	switch (axisOrder)
	{
	case AxisOrder::XYX:

		ret = convertXYX(rotation);
		break;

	case AxisOrder::XYZ:

		ret = convertXYZ(rotation);
		break;

	case AxisOrder::XZX:

		ret = convertXZX(rotation);
		break;

	case AxisOrder::XZY:

		ret = convertXZY(rotation);
		break;

	case AxisOrder::YXY:

		ret = convertYXY(rotation);
		break;

	case AxisOrder::YXZ:

		ret = convertYXZ(rotation);
		break;

	case AxisOrder::YZX:

		ret = convertYZX(rotation);
		break;

	case AxisOrder::YZY:

		ret = convertYZY(rotation);
		break;
		
	case AxisOrder::ZXY:

		ret = convertZXY(rotation);
		break;

	case AxisOrder::ZXZ:

		ret = convertZXZ(rotation);
		break;

	case AxisOrder::ZYX:

		ret = convertZYX(rotation);
		break;

	case AxisOrder::ZYZ:

		ret = convertZYZ(rotation);
		break;
	}

	return ret;
}

osg::Vec3d kinematicUtils::convertXYX(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z()), 2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y())),
		std::acos(std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
		std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), 2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()))
		);
}

osg::Vec3d kinematicUtils::convertXYZ(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
		-std::asin(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y())),
		std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2))
		);
}

osg::Vec3d kinematicUtils::convertXZX(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), 2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z())),
		std::acos(std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
		std::atan2(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y()), 2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()))
		);
}

osg::Vec3d kinematicUtils::convertXZY(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
		std::asin(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z())),
		std::atan2(2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2))
		);
}

osg::Vec3d kinematicUtils::convertYXY(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), 2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x())),
		std::acos(std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
		std::atan2(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z()), 2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()))
		);
}

osg::Vec3d kinematicUtils::convertYXZ(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
		std::asin(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x())),
		std::atan2(2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2))
		);
}

osg::Vec3d kinematicUtils::convertYZX(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
		-std::asin(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z())),
		std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2))
		);
}

osg::Vec3d kinematicUtils::convertYZY(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x()), 2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z())),
		std::acos(std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
		std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), 2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()))
		);
}

osg::Vec3d kinematicUtils::convertZXY(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
		-std::asin(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x())),
		std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2))
		);
}

osg::Vec3d kinematicUtils::convertZXZ(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y()), 2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x())),
		std::acos(std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
		std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), 2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()))
		);
}

osg::Vec3d kinematicUtils::convertZYX(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
		std::asin(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y())),
		std::atan2(2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2))
		);
}

osg::Vec3d kinematicUtils::convertZYZ(const osg::Quat & rotation)
{
	return osg::Vec3d(
		std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), 2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y())),
		std::acos(std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
		std::atan2(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x()), 2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()))
		);
}

osg::Vec3d kinematicUtils::convert(const osg::Quat & rotation, const AxisOrder::Type axisOrder)
{
	osg::Vec3d ret(0, 0, 0);

	switch (axisOrder)
	{
	case AxisOrder::XYX:

		ret = convertXYX(rotation);
		break;

	case AxisOrder::XYZ:

		ret = convertXYZ(rotation);
		break;

	case AxisOrder::XZX:

		ret = convertXZX(rotation);
		break;

	case AxisOrder::XZY:

		ret = convertXZY(rotation);
		break;

	case AxisOrder::YXY:

		ret = convertYXY(rotation);
		break;

	case AxisOrder::YXZ:

		ret = convertYXZ(rotation);
		break;

	case AxisOrder::YZX:

		ret = convertYZX(rotation);
		break;

	case AxisOrder::YZY:

		ret = convertYZY(rotation);
		break;

	case AxisOrder::ZXY:

		ret = convertZXY(rotation);
		break;

	case AxisOrder::ZXZ:

		ret = convertZXZ(rotation);
		break;

	case AxisOrder::ZYX:

		ret = convertZYX(rotation);
		break;

	case AxisOrder::ZYZ:

		ret = convertZYZ(rotation);
		break;
	}

	return ret;
}

osg::Vec3d kinematicUtils::toRadians(const osg::Vec3d & rotation)
{
	return transform<&osg::DegreesToRadians>(rotation);
}

osg::Vec3d kinematicUtils::toDegrees(const osg::Vec3d & rotation)
{
	return transform<&osg::RadiansToDegrees>(rotation);
}