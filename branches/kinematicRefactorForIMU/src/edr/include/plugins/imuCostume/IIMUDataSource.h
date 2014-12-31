/********************************************************************
	created:  2014/12/03	12:35:25
	filename: IIMUDataSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IIMUDATASOURCE_H__
#define __HEADER_GUARD_IMU__IIMUDATASOURCE_H__

#include <utils/PtrPolicyStd.h>
#include <corelib/Variant.h>
#include <threadingUtils/StreamData.h>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <plugins/imuCostume/IIMUOrientationEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>

namespace IMU
{
	class IIMUDataSource
	{
	public:

		enum ConnectionStatus
		{
			ONLINE,
			OFFLINE,
			CONNECTION_PROBLEMS,
			UNKNOWN
		};

		typedef threadingUtils::StreamT<imuCostume::ProtocolSendBufferHelper::Buffer> RawDataStream;

		DEFINE_SMART_POINTERS(RawDataStream);

		typedef threadingUtils::StreamT<imuCostume::CostumeCANopenIO::Data> CANopenFramesStream;

		DEFINE_SMART_POINTERS(CANopenFramesStream);

		typedef threadingUtils::IStreamT<imuCostume::Costume::Data> CostumeStream;

		DEFINE_SMART_POINTERS(CostumeStream);

		DEFINE_SMART_POINTERS(IIMUOrientationEstimationAlgorithm);
		DEFINE_SMART_POINTERS(IMUCostumeCalibrationAlgorithm);
		DEFINE_SMART_POINTERS(IMUCostumeMotionEstimationAlgorithm);

		struct SensorMappingDetails : public IMUCostumeCalibrationAlgorithm::SensorAdjustment
		{
			std::string jointName;			
			IMU::IIMUDataSource::IIMUOrientationEstimationAlgorithmConstPtr estimationAlgorithm;
		};

		typedef std::map<imuCostume::Costume::SensorID, SensorMappingDetails> SensorsMappingDetails;		

	public:

		virtual ~IIMUDataSource() {}

		virtual const bool refreshCostumes() = 0;

		virtual const unsigned int costumesCout() const = 0;

		virtual const imuCostume::Costume::SensorsConfiguration & costumeConfiguration(const unsigned int idx) const = 0;
		virtual const imuCostume::CostumeRawIO::CostumeAddress costumeAddress(const unsigned int idx) const = 0;
		virtual const ConnectionStatus costumeStatus(const unsigned int idx) const = 0;
		virtual const RawDataStreamPtr costumeRawDataStream(const unsigned int idx) const = 0;
		virtual const core::VariantsList costumeData(const unsigned int idx) const = 0;

		virtual void loadCostume(const unsigned int idx) = 0;
		virtual void unloadCostume(const unsigned int idx) = 0;
		virtual const bool costumeLoaded(const unsigned int idx) const = 0;
		virtual const unsigned int costumesLoadedCount() const = 0;

		virtual void loadAllCostumes() = 0;
		virtual void unloadAllCostumes() = 0;

		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithm * algorithm) = 0;
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithm * algorithm) = 0;
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithm * algorithm) = 0;

		virtual std::list<IIMUOrientationEstimationAlgorithmConstPtr> orientationEstimationAlgorithms() const = 0;
		virtual std::list<IMUCostumeCalibrationAlgorithmConstPtr> calibrationAlgorithms() const = 0;
		virtual std::list<IMUCostumeMotionEstimationAlgorithmConstPtr> motionEstimationAlgorithms() const = 0;
	};
}

DEFINE_WRAPPER(threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER_INHERITANCE(IMU::IIMUDataSource::RawDataStream, threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>);
DEFINE_WRAPPER(IMU::IIMUDataSource::CANopenFramesStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::IIMUDataSource::CostumeStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif	// __HEADER_GUARD_IMU__IIMUDATASOURCE_H__