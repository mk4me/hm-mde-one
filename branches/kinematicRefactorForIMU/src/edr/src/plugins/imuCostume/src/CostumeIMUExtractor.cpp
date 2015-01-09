#include "PCH.h"
#include <plugins/imuCostume/CostumeIMUExtractor.h>

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
	return completeDataFilter(streamData);
}

void CostumeIMUExtractor::extract(const IMU::CostumeStream::value_type & streamData, IMU::SensorsStreamData & sensorsData) const
{
	IMU::SensorsData locData;

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