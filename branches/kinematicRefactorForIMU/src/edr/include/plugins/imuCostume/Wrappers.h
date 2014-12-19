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
#include <plugins/newChart/Wrappers.h>

namespace IMU
{
	//! Struktura opisuj¹ca surowe dane z IMU
	struct IMUData
	{
		osg::Vec3 accelerometer;
		osg::Vec3 gyroscope;
		osg::Vec3 magnetometer;
		osg::Quat orientation;
		int status;
	};

	//! Strumieñ danych jednego czujnika IMU
	typedef threadingUtils::StreamT<IMUData> IMUStream;

	DEFINE_SMART_POINTERS(IMUStream);

	//! Strumieñ danych 3D
	typedef threadingUtils::StreamT<osg::Vec3> Vec3Stream;

	DEFINE_SMART_POINTERS(Vec3Stream);

	//! Strumieñ danych 3D
	typedef threadingUtils::StreamT<osg::Quat> QuatStream;

	DEFINE_SMART_POINTERS(QuatStream);
}

DEFINE_WRAPPER(IMU::IMUStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::Vec3Stream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif	//	HEADER_GUARD_IMU__WRAPPERS_H__
