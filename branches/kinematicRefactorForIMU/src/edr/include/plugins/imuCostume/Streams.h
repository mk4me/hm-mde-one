/********************************************************************
	created:  2015/01/06	00:09:20
	filename: Streams.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__STREAMS_H__
#define __HEADER_GUARD_IMU__STREAMS_H__

#include <osg/Vec3>
#include <osg/Quat>
#include <plugins/imuCostume/Types.h>
#include <utils/SmartPtr.h>
#include <utils/ObjectWrapper.h>
#include <threadingUtils/StreamData.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <kinematiclib/Skeleton.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
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
	typedef threadingUtils::IStreamT<osg::Quat> QuatStream;

	DEFINE_SMART_POINTERS(QuatStream);

	typedef threadingUtils::StreamT<imuCostume::ProtocolSendBufferHelper::Buffer> RawDataStream;

	DEFINE_SMART_POINTERS(RawDataStream);

	typedef threadingUtils::IStreamT<imuCostume::CostumeCANopenIO::Data> CANopenFramesStream;

	DEFINE_SMART_POINTERS(CANopenFramesStream);

	typedef threadingUtils::IStreamT<imuCostume::Costume::Data> CostumeStream;

	DEFINE_SMART_POINTERS(CostumeStream);

	struct SkeletonMotionState : public IMUCostumeMotionEstimationAlgorithm::MotionState
	{
		double timestamp;
	};

	typedef threadingUtils::IStreamT<SkeletonMotionState> MotionStream;

	DEFINE_SMART_POINTERS(MotionStream);

	struct CostumeSkeletonMotion
	{
		DataIndexToJointMapping dataToModelMapping;
		kinematic::SkeletonConstPtr skeleton;
		MotionStreamPtr stream;
	};

	DEFINE_SMART_POINTERS(CostumeSkeletonMotion);
}

//! -------------------- Wrappery ---------------------------------

DEFINE_WRAPPER(IMU::IMUStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::Vec3Stream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::QuatStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

DEFINE_WRAPPER(threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER_INHERITANCE(IMU::RawDataStream, threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>);
DEFINE_WRAPPER(IMU::CANopenFramesStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::CostumeStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::MotionStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::CostumeSkeletonMotion, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif	// __HEADER_GUARD_IMU__STREAMS_H__