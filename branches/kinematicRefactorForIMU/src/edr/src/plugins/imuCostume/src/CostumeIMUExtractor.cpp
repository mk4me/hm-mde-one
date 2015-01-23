#include "PCH.h"
#include <plugins/imuCostume/CostumeIMUExtractor.h>
#include <corelib/IPlugin.h>

using namespace IMU;

CostumeCompleteDataFilter::CostumeCompleteDataFilter(CostumeCompleteDataFilter && Other)
	: sensorsIDs(std::move(Other.sensorsIDs))
{

}

CostumeCompleteDataFilter::CostumeCompleteDataFilter(const CostumeCompleteDataFilter & Other)
	: sensorsIDs(Other.sensorsIDs)
{

}

CostumeCompleteDataFilter::CostumeCompleteDataFilter(const imuCostume::Costume::SensorIDsSet & sensorsIDs)
	: sensorsIDs(sensorsIDs)
{

}

CostumeCompleteDataFilter::CostumeCompleteDataFilter(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration)
{
	auto it = sensorsConfiguration.find(imuCostume::Costume::IMU);
	if (it == sensorsConfiguration.end())
	{
		throw std::runtime_error("Empty IMU sensors set");
	}

	sensorsIDs = it->second;
}

CostumeCompleteDataFilter::~CostumeCompleteDataFilter()
{

}

bool CostumeCompleteDataFilter::operator()(const IMU::CostumeStream::value_type & streamData) const
{
	imuCostume::Costume::SensorIDsSet locSensorsIDs(sensorsIDs);

	IMU::SensorsData locData;

	for (const auto & d : streamData.sensorsData)
	{
		if (d->type() == imuCostume::Costume::IMU){
			auto imuData = utils::static_pointer_cast<const imuCostume::Costume::IMUSensor>(d);
			if (imuData->dataStatus() & (imuCostume::Costume::IMUSensor::ACC_DATA | imuCostume::Costume::IMUSensor::GYRO_DATA | imuCostume::Costume::IMUSensor::MAG_DATA))
			{
				locSensorsIDs.erase(d->id());
			}
		}
	}

	return (locSensorsIDs.empty() == true);
}

CostumeIMUExtractor::CostumeIMUExtractor(CostumeIMUExtractor && Other)
	: completeDataFilter(std::move(Other.completeDataFilter))
{

}

CostumeIMUExtractor::CostumeIMUExtractor(const CostumeIMUExtractor & Other)
	: completeDataFilter(Other.completeDataFilter)
{

}

CostumeIMUExtractor::CostumeIMUExtractor(const imuCostume::Costume::SensorIDsSet & sensorsIDs)
	: completeDataFilter(sensorsIDs)
{

}

CostumeIMUExtractor::CostumeIMUExtractor(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration)
	: completeDataFilter(sensorsConfiguration)
{
	
}

CostumeIMUExtractor::~CostumeIMUExtractor()
{

}

bool CostumeIMUExtractor::verify(const IMU::CostumeStream::value_type & streamData) const
{
	//return completeDataFilter(streamData);
	return true;
}

void CostumeIMUExtractor::extract(const IMU::CostumeStream::value_type & streamData, IMU::SensorsStreamData & sensorsData) const
{
	IMU::SensorsData locData;
	//PLUGIN_LOG_DEBUG("CostumeIMUExtractor");
	for (const auto & d : streamData.sensorsData)
	{
		auto imuData = utils::static_pointer_cast<const imuCostume::Costume::IMUSensor>(d);		

		IMU::SensorData sd;
		sd.accelerometer = imuData->accelerometer();
		sd.magnetometer = imuData->magnetometer();
		sd.gyroscope = imuData->gyroscope();
		sd.orientation = imuData->orientation();

		locData.insert(IMU::SensorsData::value_type(d->id(), sd));
	}		
	
	sensorsData.timestamp = streamData.timestamp;
	sensorsData.sensorsData = std::move(locData);	
}

OrientationEstimator::OrientationEstimator(const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping & orientationAlgorithms)
	: orientationAlgorithms(orientationAlgorithms)
{

}

void OrientationEstimator::operator()(IMU::SensorsStreamData & data) const
{
	//PLUGIN_LOG_DEBUG("OrientationEstimator");
	for (auto & d : data.sensorsData)
	{
		auto oIT = orientationAlgorithms.find(d.first);

		if (oIT != orientationAlgorithms.end()){

			double deltaT = 0;
			auto tIT = lastUpdateTime.find(d.first);

			if (tIT == lastUpdateTime.end()){
				lastUpdateTime.insert(std::map<imuCostume::Costume::SensorID, uint32_t>::value_type(d.first, data.timestamp));
			}
			else{
				deltaT = (data.timestamp > tIT->second) ? (data.timestamp - tIT->second) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - tIT->second + data.timestamp);
				deltaT /= 1000.0;
				tIT->second = data.timestamp;
			}

			d.second.orientation = oIT->second->estimate(d.second.accelerometer, d.second.gyroscope, d.second.magnetometer, deltaT);
		}
	}
}

OrientationEstimator::~OrientationEstimator()
{

}

ArrayExtractor::ArrayExtractor(const unsigned int idx)
	: idx(idx)
{

}

ArrayExtractor::ArrayExtractor(const ArrayExtractor & Other)
	: idx(Other.idx)
{

}

ArrayExtractor::~ArrayExtractor()
{

}

OrientationExtractor::OrientationExtractor(const unsigned int idx)
	: idx(idx)
{

}

OrientationExtractor::OrientationExtractor(const OrientationExtractor & Other)
	: idx(Other.idx)
{

}

OrientationExtractor::~OrientationExtractor()
{

}

bool OrientationExtractor::verify(const IMU::SkeletonMotionState & a) const
{
	return true;
}

void OrientationExtractor::extract(const IMU::SkeletonMotionState & a, osg::Quat & ret) const
{
	auto it = a.jointsOrientations.begin();
	std::advance(it, idx);
	ret = it->second;
}

CANopenDataExtractor::CANopenDataExtractor()
{

}

CANopenDataExtractor::~CANopenDataExtractor()
{

}

bool CANopenDataExtractor::verify(const IMU::CANopenFramesStream::value_type & a)
{
	return true;
}

void CANopenDataExtractor::extract(const IMU::CANopenFramesStream::value_type & a, IMU::CostumeStream::value_type & ret) const
{
	//PLUGIN_LOG_ERROR("CANopenDataExtractor");
	ret = imuCostume::Costume::convert(a);
}

ExtractCostumeMotion::ExtractCostumeMotion(
	kinematic::SkeletonConstPtr skeleton,
	const IMU::SensorsMapping & sensorsMapping,
	const IMU::DataIndexToJointMapping & dataMapping,
	const IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustments,
	IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm)
	: motionEstimationAlgorithm(motionEstimationAlgorithm),
	sensorsMapping(sensorsMapping), dataMapping(dataMapping), sensorsAdjustments(sensorsAdjustments),
	skeleton(skeleton), previousTime(0)
{

}

ExtractCostumeMotion::~ExtractCostumeMotion()
{

}

bool ExtractCostumeMotion::verify(const IMU::SensorsStreamData & input) const
{
	return true;
}

void ExtractCostumeMotion::extract(const IMU::SensorsStreamData & input, IMU::SkeletonMotionState & output) const
{
	double deltaTime = (input.timestamp > previousTime) ? (input.timestamp - previousTime) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previousTime + input.timestamp);
	deltaTime /= 1000.0;

	if (previousTime == 0)
	{
		deltaTime = 0;
	}

	previousTime = input.timestamp;

	IMU::IMUCostumeMotionEstimationAlgorithm::MotionState motionState;
	motionState.position = osg::Vec3(0, 0, 0);

	std::map<std::string, osg::Quat> jointsGlobalOrientations;

	for (const auto & i : input.sensorsData)
	{
		const auto & adj = sensorsAdjustments.find(i.first)->second;
		jointsGlobalOrientations.insert(std::map<std::string, osg::Quat>::value_type(sensorsMapping.left.find(i.first)->get_right(), i.second.orientation * adj.rotation.inverse()));
	}

	kinematic::SkeletonState ss(kinematic::SkeletonState::create(*skeleton));

	kinematic::SkeletonState::Joint::visitLevelOrder(ss.root(),
		[&motionState, &jointsGlobalOrientations](kinematic::SkeletonState::JointPtr joint,
		const kinematic::SkeletonState::Joint::size_type) -> void
	{
		auto it = jointsGlobalOrientations.find(joint->value.name());
		if (it != jointsGlobalOrientations.end()){
			joint->value.setGlobal(it->second);
		}

		if (joint->isLeaf() == false){
			motionState.jointsOrientations.insert(std::map<std::string, osg::Quat>::value_type(joint->value.name(), joint->value.globalOrientation()));
		}
	});

	try{
		auto ret = motionEstimationAlgorithm->estimate(motionState, input.sensorsData, deltaTime);
		output.position = ret.position;
		output.jointsOrientations = ret.jointsOrientations;
		output.timestamp = (double)previousTime / 1000.0;
	}
	catch (...){

	}
}

RawToCANopenExtractor::RawToCANopenExtractor()
{


}

RawToCANopenExtractor::~RawToCANopenExtractor()
{

}

bool RawToCANopenExtractor::verify(const IMU::RawDataStream::value_type & val) const
{
	return true;
}

void RawToCANopenExtractor::extract(const IMU::RawDataStream::value_type & in, IMU::CANopenFramesStream::value_type & out) const
{
	//PLUGIN_LOG_DEBUG("RawToCANopenExtractor");
	out = imuCostume::CostumeCANopenIO::extractData(in.buffer.get() + sizeof(imuCostume::CostumeCANopenIO::Header), in.length - sizeof(imuCostume::CostumeCANopenIO::Header));
}