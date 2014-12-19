/********************************************************************
	created:  2014/12/11	08:51:59
	filename: RotationConverter.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__
#define __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__

#include <kinematicUtils/Types.h>
#include <osg/Vec3>
#include <osg/Quat>

namespace kinematicUtils
{
	AxisOrder axisOrder(const ChannelTriplet & channelTriplet);

	osg::Quat convertXYX(const osg::Vec3 & rotation);
	osg::Quat convertXYZ(const osg::Vec3 & rotation);
	osg::Quat convertXZX(const osg::Vec3 & rotation);
	osg::Quat convertXZY(const osg::Vec3 & rotation);
	osg::Quat convertYXY(const osg::Vec3 & rotation);
	osg::Quat convertYXZ(const osg::Vec3 & rotation);
	osg::Quat convertYZX(const osg::Vec3 & rotation);
	osg::Quat convertYZY(const osg::Vec3 & rotation);
	osg::Quat convertZXY(const osg::Vec3 & rotation);
	osg::Quat convertZXZ(const osg::Vec3 & rotation);
	osg::Quat convertZYX(const osg::Vec3 & rotation);
	osg::Quat convertZYZ(const osg::Vec3 & rotation);
	osg::Quat convert(const osg::Vec3 & rotation, const AxisOrder axisOrder);
	
	osg::Vec3 convertXYX(const osg::Quat & rotation);
	osg::Vec3 convertXYZ(const osg::Quat & rotation);
	osg::Vec3 convertXZX(const osg::Quat & rotation);
	osg::Vec3 convertXZY(const osg::Quat & rotation);
	osg::Vec3 convertYXY(const osg::Quat & rotation);
	osg::Vec3 convertYXZ(const osg::Quat & rotation);
	osg::Vec3 convertYZX(const osg::Quat & rotation);
	osg::Vec3 convertYZY(const osg::Quat & rotation);
	osg::Vec3 convertZXY(const osg::Quat & rotation);
	osg::Vec3 convertZXZ(const osg::Quat & rotation);
	osg::Vec3 convertZYX(const osg::Quat & rotation);
	osg::Vec3 convertZYZ(const osg::Quat & rotation);
	osg::Vec3 convert(const osg::Quat & rotation, const AxisOrder axisOrder);

	osg::Vec3 toRadians(const osg::Vec3 & rotation);
	osg::Vec3 toDegrees(const osg::Vec3 & rotation);
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__