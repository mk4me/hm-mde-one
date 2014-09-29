/********************************************************************
	created:  2014/01/03
	created:  3:1:2014   12:02
	filename: Wrappers.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_IMU__WRAPPERS_H__
#define HEADER_GUARD_IMU__WRAPPERS_H__

#include <osg/Vec3>
#include <osg/Quat>
#include <threadingUtils/StreamData.h>
#include <utils/SmartPtr.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ObjectWrapper.h>
#include <kinematiclib/JointAnglesCollection.h>

namespace IMU
{
	//! Struktura opisuj�ca surowe dane z IMU
	struct IMUData
	{
		osg::Vec3 accelerometer;
		osg::Vec3 gyroscope;
		osg::Vec3 magnetometer;
		//osg::Quat orientation;
	};

	//! Strumie� danych jednego czujnika IMU
	typedef threadingUtils::IStreamT<IMUData> IMUStream;

	DEFINE_SMART_POINTERS(IMUStream);

	//! Strumie� danych 3D
	typedef threadingUtils::IStreamT<osg::Vec3> Vec3Stream;

	DEFINE_SMART_POINTERS(Vec3Stream);
}

DEFINE_WRAPPER(IMU::IMUStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::Vec3Stream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif	//	HEADER_GUARD_IMU__WRAPPERS_H__
