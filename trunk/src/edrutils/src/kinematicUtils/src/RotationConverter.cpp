#include <kinematicUtils/RotationConverter.h>
#include <osg/Math>
#include <boost/lexical_cast.hpp>

//namespace musi byæ bo to wolne funkcje w przestrzeni nazw
namespace kinematicUtils
{

	double convertAngleUnit(const double value, const AngleUnitType destType)
	{
		switch (destType)
		{
		case Deg: return osg::RadiansToDegrees(value);
		case Rad: return osg::DegreesToRadians(value);
		default:
			return 0.0;
		}
	}

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

	osg::Vec3d orderedAngles(const osg::Vec3d & angles, const AxisOrder::Type axisOrder)
	{
		osg::Vec3d ret = angles;
		switch (axisOrder)
		{
		case AxisOrder::XYX:
			break;

		case AxisOrder::XYZ:
			break;

		case AxisOrder::XZX:
		case AxisOrder::XZY:
			std::swap(ret[1], ret[2]);
			break;

		case AxisOrder::YXY:
		case AxisOrder::YXZ:
			std::swap(ret[0], ret[1]);
			break;

		case AxisOrder::YZX:
			ret = osg::Vec3d(angles.y(), angles.z(), angles.x());
			break;

		case AxisOrder::YZY:
			//ret = convertYZY(rotation);
			break;

		case AxisOrder::ZXY:
			ret = osg::Vec3d(angles.z(), angles.x(), angles.y());
			break;

		case AxisOrder::ZXZ:
			//ret = convertZXZ(rotation);
			break;

		case AxisOrder::ZYX:
			std::swap(ret[0], ret[2]);
			break;

		case AxisOrder::ZYZ:
			//ret = convertZYZ(rotation);
			break;
		}

		return ret;
	}

	osg::Quat convertXYX(const osg::Vec3d & rotation)
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

	osg::Quat convertXYZ(const osg::Vec3d & rotation)
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


	osg::Quat convertXZX(const osg::Vec3d & rotation)
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

	osg::Quat convertXZY(const osg::Vec3d & rotation)
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

	osg::Quat convertYXY(const osg::Vec3d & rotation)
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

	osg::Quat convertYXZ(const osg::Vec3d & rotation)
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

	osg::Quat convertYZX(const osg::Vec3d & rotation)
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

	osg::Quat convertYZY(const osg::Vec3d & rotation)
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

	osg::Quat convertZXY(const osg::Vec3d & rotation)
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

	osg::Quat convertZXZ(const osg::Vec3d & rotation)
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

	osg::Quat convertZYX(const osg::Vec3d & rotation)
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

	osg::Quat convertZYZ(const osg::Vec3d & rotation)
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

	osg::Quat convert(const osg::Vec3d & rotation, const AxisOrder::Type axisOrder)
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

	osg::Vec3d convertXYX(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z()), 2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y())),
			std::acos(std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
			std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), 2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()))
			);
	}

	osg::Vec3d convertXYZ(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
			-std::asin(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y())),
			std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2))
			);
	}

	osg::Vec3d convertXZX(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), 2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z())),
			std::acos(std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
			std::atan2(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y()), 2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()))
			);
	}

	osg::Vec3d convertXZY(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
			std::asin(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z())),
			std::atan2(2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2))
			);
	}

	osg::Vec3d convertYXY(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), 2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x())),
			std::acos(std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
			std::atan2(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z()), 2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()))
			);
	}

	osg::Vec3d convertYXZ(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
			std::asin(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x())),
			std::atan2(2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2))
			);
	}

	osg::Vec3d convertYZX(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
			-std::asin(2 * (rotation.x() * rotation.y() - rotation.w() * rotation.z())),
			std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2))
			);
	}

	osg::Vec3d convertYZY(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x()), 2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z())),
			std::acos(std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
			std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), 2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()))
			);
	}

	osg::Vec3d convertZXY(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.y(), 2) - std::pow(rotation.z(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.x(), 2)),
			-std::asin(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x())),
			std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2))
			);
	}

	osg::Vec3d convertZXZ(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.x() * rotation.z() - rotation.w() * rotation.y()), 2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x())),
			std::acos(std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
			std::atan2(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()), 2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()))
			);
	}

	osg::Vec3d convertZYX(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (-rotation.x() * rotation.y() + rotation.w() * rotation.z()), std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2) - std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2)),
			std::asin(2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y())),
			std::atan2(2 * (-rotation.y() * rotation.z() + rotation.w() * rotation.x()), std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2))
			);
	}

	osg::Vec3d convertZYZ(const osg::Quat & rotation)
	{
		return osg::Vec3d(
			std::atan2(2 * (rotation.y() * rotation.z() + rotation.w() * rotation.x()), 2 * (-rotation.x() * rotation.z() + rotation.w() * rotation.y())),
			std::acos(std::pow(rotation.z(), 2) - std::pow(rotation.y(), 2) - std::pow(rotation.x(), 2) + std::pow(rotation.w(), 2)),
			std::atan2(2 * (rotation.y() * rotation.z() - rotation.w() * rotation.x()), 2 * (rotation.x() * rotation.z() + rotation.w() * rotation.y()))
			);
	}

	osg::Vec3d convert(const osg::Quat & rotation, const AxisOrder::Type axisOrder)
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

	osg::Vec3d toRadians(const osg::Vec3d & rotation)
	{
		return transform<&osg::DegreesToRadians>(rotation);
	}

	osg::Vec3d toDegrees(const osg::Vec3d & rotation)
	{
		return transform<&osg::RadiansToDegrees>(rotation);
	}

}