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
#include <utils/StreamData.h>
#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>
#include <kinematiclib/JointAnglesCollection.h>

namespace IMU
{
	//! Struktura opisuj¹ca surowe dane z IMU
	struct IMUData
	{
		osg::Vec3 accelerometer;
		osg::Vec3 gyroscope;
		osg::Vec3 magnetometer;
		osg::Quat orientation;
	};

	//! Strumieñ danych z IMU
	typedef utils::StreamT<std::vector<IMUData>> IMUsStream;

	DEFINE_SMART_POINTERS(IMUsStream);
}

DEFINE_WRAPPER(IMU::IMUsStream, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(kinematic::JointAnglesCollectionStream, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif	//	HEADER_GUARD_IMU__WRAPPERS_H__
