/********************************************************************
	created:  2014/12/11	08:51:59
	filename: RotationConverter.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__
#define __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__

#include <kinematicUtils/Types.h>
#include <osg/Vec3d>
#include <osg/Quat>

namespace kinematicUtils
{
	osg::Quat convertXYX(const osg::Vec3d & rotation);
	osg::Quat convertXYZ(const osg::Vec3d & rotation);
	osg::Quat convertXZX(const osg::Vec3d & rotation);
	osg::Quat convertXZY(const osg::Vec3d & rotation);
	osg::Quat convertYXY(const osg::Vec3d & rotation);
	osg::Quat convertYXZ(const osg::Vec3d & rotation);
	osg::Quat convertYZX(const osg::Vec3d & rotation);
	osg::Quat convertYZY(const osg::Vec3d & rotation);
	osg::Quat convertZXY(const osg::Vec3d & rotation);
	osg::Quat convertZXZ(const osg::Vec3d & rotation);
	osg::Quat convertZYX(const osg::Vec3d & rotation);
	osg::Quat convertZYZ(const osg::Vec3d & rotation);
	osg::Quat convert(const osg::Vec3d & rotation, const AxisOrder::Type axisOrder);
	
	osg::Vec3d convertXYX(const osg::Quat & rotation);
	osg::Vec3d convertXYZ(const osg::Quat & rotation);
	osg::Vec3d convertXZX(const osg::Quat & rotation);
	osg::Vec3d convertXZY(const osg::Quat & rotation);
	osg::Vec3d convertYXY(const osg::Quat & rotation);
	osg::Vec3d convertYXZ(const osg::Quat & rotation);
	osg::Vec3d convertYZX(const osg::Quat & rotation);
	osg::Vec3d convertYZY(const osg::Quat & rotation);
	osg::Vec3d convertZXY(const osg::Quat & rotation);
	osg::Vec3d convertZXZ(const osg::Quat & rotation);
	osg::Vec3d convertZYX(const osg::Quat & rotation);
	osg::Vec3d convertZYZ(const osg::Quat & rotation);
	osg::Vec3d convert(const osg::Quat & rotation, const AxisOrder::Type axisOrder);

	osg::Vec3d toRadians(const osg::Vec3d & rotation);
	osg::Vec3d toDegrees(const osg::Vec3d & rotation);
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__