#include "PCH.h"
#include <plugins/imuCostume/CostumeProfile.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <boost/lexical_cast.hpp>

using namespace IMU;

Skeleton::Skeleton(const Skeleton & skeleton)
	: kinematic::Skeleton(skeleton), id(skeleton.id)
{

}

Skeleton::Skeleton(Skeleton && skeleton)
	: kinematic::Skeleton(skeleton), id(skeleton.id)
{
	
}

Skeleton::Skeleton(const core::UniqueID id, const kinematic::Skeleton & skeleton)
	: kinematic::Skeleton(skeleton), id(id)
{

}

Skeleton::Skeleton(const core::UniqueID id, kinematic::Skeleton && skeleton)
	: kinematic::Skeleton(std::move(skeleton)), id(id)
{

}

Skeleton::~Skeleton()
{

}

const core::UniqueID Skeleton::ID() const
{
	return id;
}

CostumeProfileInstance CostumeProfileInstance::create(const CostumeProfile & profile)
{
	CostumeProfileInstance ret;

	ret.name = profile.name;
	ret.calibrationAlgorithm.reset(profile.calibrationAlgorithm->create());
	ret.motionEstimationAlgorithm.reset(profile.motionEstimationAlgorithm->create());
	ret.sensorsAdjustments = profile.sensorsAdjustments;
	ret.sensorsMapping = profile.sensorsMapping;

	if (profile.skeleton){
		ret.skeleton.reset(new Skeleton(*profile.skeleton));
	}

	for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
	{
		ret.sensorsOrientationEstimationAlgorithms.insert(OrientationEstimationAlgorithmsMapping::value_type(ea.first, IIMUOrientationEstimationAlgorithmPtr(ea.second->create())));
	}

	return ret;
}	

CostumeProfileInstance CostumeProfileInstance::create(const CostumeProfile & profile,
	const imuCostume::Costume::SensorIDsSet & filter)
{
	CostumeProfileInstance ret;

	ret.name = profile.name;
	ret.calibrationAlgorithm.reset(profile.calibrationAlgorithm->create());
	ret.motionEstimationAlgorithm.reset(profile.motionEstimationAlgorithm->create());
	
	for (const auto & val : profile.sensorsAdjustments)
	{
		if (filter.find(val.first) != filter.end())
		{
			ret.sensorsAdjustments.insert(val);
		}
	}

	for (const auto & val : profile.sensorsMapping)
	{
		if (filter.find(val.get_left()) != filter.end())
		{
			ret.sensorsMapping.insert(val);
		}
	}	

	if (profile.skeleton){
		ret.skeleton.reset(new Skeleton(*profile.skeleton));
	}

	for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
	{
		if (filter.find(ea.first) != filter.end()){
			ret.sensorsOrientationEstimationAlgorithms.insert(OrientationEstimationAlgorithmsMapping::value_type(ea.first, IIMUOrientationEstimationAlgorithmPtr(ea.second->create())));
		}
	}

	return ret;
}

SerializableCostumeProfile SerializableCostumeProfile::pack(const CostumeProfile & profile)
{
	SerializableCostumeProfile ret;
	ret.name = profile.name;
	ret.skeletonID = profile.skeleton == nullptr ? boost::uuids::nil_uuid() : profile.skeleton->ID();
	ret.calibrationAlgorithmID = profile.calibrationAlgorithm == nullptr ? boost::uuids::nil_uuid() : profile.calibrationAlgorithm->ID();
	ret.motionEstimationAlgorithmID = profile.motionEstimationAlgorithm == nullptr ? boost::uuids::nil_uuid() : profile.motionEstimationAlgorithm->ID();
	ret.sensorsAdjustments = profile.sensorsAdjustments;
	ret.sensorsMapping = profile.sensorsMapping;
	for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
	{
		if (ea.second != nullptr){
			ret.sensorsOrientationEstimationAlgorithms.insert(OrientationEstimationAlgorithmsMapping::value_type(ea.first, ea.second->ID()));
		}
	}

	return ret;
}

CostumeProfile SerializableCostumeProfile::unpack(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds)
{
	auto skeletons = ds->skeletonModels();
	auto motionEstimationAlgos = ds->motionEstimationAlgorithms();
	auto calibrationAlgos = ds->calibrationAlgorithms();
	auto orientationEstimationAlgos = ds->orientationEstimationAlgorithms();


	CostumeProfile ret;
	ret.name = profile.name;
	ret.sensorsAdjustments = profile.sensorsAdjustments;
	ret.sensorsMapping = profile.sensorsMapping;

	{
		auto sIT = skeletons.find(profile.skeletonID);

		if (sIT != skeletons.end()){
			ret.skeleton = sIT->second;
		}
	}

	{
		auto mIT = motionEstimationAlgos.find(profile.motionEstimationAlgorithmID);

		if (mIT != motionEstimationAlgos.end()){
			ret.motionEstimationAlgorithm = mIT->second;
		}
	}

	{
		auto cIT = calibrationAlgos.find(profile.calibrationAlgorithmID);

		if (cIT != calibrationAlgos.end()){
			ret.calibrationAlgorithm = cIT->second;
		}
	}

	for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
	{
		auto eIT = orientationEstimationAlgos.find(ea.second);

		if (eIT != orientationEstimationAlgos.end()){
			ret.sensorsOrientationEstimationAlgorithms.insert(CostumeProfile::OrientationEstimationAlgorithmsMapping::value_type(ea.first, eIT->second));
		}
	}

	return ret;
}

SerializableCostumeProfile::VerificationStatus SerializableCostumeProfile::verify(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds)
{
	if (profile.name.empty() == true){
		return Failed;
	}

	if (profile.sensorsAdjustments.size() != profile.sensorsMapping.size()
		|| profile.sensorsAdjustments.size() != profile.sensorsOrientationEstimationAlgorithms.size()
		|| profile.skeletonID.is_nil() == true
		|| profile.calibrationAlgorithmID.is_nil() == true
		|| profile.motionEstimationAlgorithmID.is_nil() == true){

		return Incomplete;
	}	

	{
		auto skeletons = ds->skeletonModels();
		auto sIT = skeletons.find(profile.skeletonID);

		if (sIT == skeletons.end()){
			return Incomplete;
		}
	}

	{
		auto motionEstimationAlgos = ds->motionEstimationAlgorithms();
		auto mIT = motionEstimationAlgos.find(profile.motionEstimationAlgorithmID);

		if (mIT == motionEstimationAlgos.end()){
			return Incomplete;
		}
	}

	{
		auto calibrationAlgos = ds->calibrationAlgorithms();
		auto cIT = calibrationAlgos.find(profile.calibrationAlgorithmID);

		if (cIT == calibrationAlgos.end()){
			return Incomplete;
		}
	}

	auto orientationEstimationAlgos = ds->orientationEstimationAlgorithms();

	for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
	{
		auto eIT = orientationEstimationAlgos.find(ea.second);

		if (eIT == orientationEstimationAlgos.end()){
			return Incomplete;
		}
	}

	return Ok;
}

void SerializableCostumeProfile::serialize(std::ostream & stream, const SerializableCostumeProfile & sProfile)
{
	unsigned int nodesCount = 0;	

	stream << "ProfileName: " << sProfile.name << std::endl;
	stream << "SkeletonID: " << sProfile.skeletonID << std::endl;
	stream << "MappingDetails: " << sProfile.sensorsMapping.size() << std::endl;

	for (const auto & m : sProfile.sensorsMapping)
	{
		const auto id = m.get_left();

		auto algoID = boost::uuids::nil_uuid();

		auto algoIT = sProfile.sensorsOrientationEstimationAlgorithms.find(id);

		if (algoIT != sProfile.sensorsOrientationEstimationAlgorithms.end()){
			algoID = algoIT->second;
		}

		auto adjIT = sProfile.sensorsAdjustments.find(id);

		IMU::IMUCostumeCalibrationAlgorithm::SensorAdjustment sa;
		sa.offset = osg::Vec3(0, 0, 0);
		sa.rotation = osg::Quat(0, 0, 0, 1);

		if (adjIT != sProfile.sensorsAdjustments.end()){
			sa = adjIT->second;
		}

		//std::string sID = boost::lexical_cast<std::string>(id);
		std::string sID = std::to_string(id);

		stream << sID << ": " << m.get_right() << " " << sa.offset[0] << " " << sa.offset[1] << " " << sa.offset[2]
			<< " " << sa.rotation[0] << " " << sa.rotation[1] << " " << sa.rotation[2] << " " << sa.rotation[3] << " " << algoID << std::endl;
	}

	stream << "CalibrationAlgoID: " << sProfile.calibrationAlgorithmID << std::endl;
	stream << "EstimationAlgoID: " << sProfile.motionEstimationAlgorithmID;
}

SerializableCostumeProfile SerializableCostumeProfile::deserialize(std::istream & stream)
{
	SerializableCostumeProfile ret;

	std::set<std::string> profileTokens;

	profileTokens.insert("ProfileName");
	profileTokens.insert("SkeletonID");	
	profileTokens.insert("MappingDetails");
	profileTokens.insert("CalibrationAlgoID");
	profileTokens.insert("EstimationAlgoID");

	std::string line;
	std::getline(stream, line);

	bool finish = false;

	while (finish == false){

		auto pos = line.find(':');

		if (pos != std::string::npos){
			auto token = line.substr(0, pos);

			if (profileTokens.find(token) == profileTokens.end()){
				finish = true;
			}
			else{

				std::string valueToExtract = line.substr(pos + 2);

				if (valueToExtract.empty() == true){
					finish = true;
				}
				else{

					profileTokens.erase(token);

					finish = profileTokens.empty();

					if (token == "ProfileName"){
						ret.name = valueToExtract;
					}
					else if (token == "SkeletonID"){
						std::stringstream ss;
						ss << valueToExtract;
						ss >> ret.skeletonID;
					}
					else if (token == "MappingDetails"){
						unsigned int count = 0;
						std::stringstream ss;
						ss << valueToExtract;
						ss >> count;

						for (unsigned int i = 0; i < count; ++i){
							std::getline(stream, line);
							pos = line.find(':');

							if (pos != std::string::npos){
								token = line.substr(0, pos);
								imuCostume::Costume::SensorID sID = std::stoul(token);

								valueToExtract = line.substr(pos + 2);
								std::stringstream ss;
								ss << valueToExtract;
								IMU::IMUCostumeCalibrationAlgorithm::SensorAdjustment sa;
								core::UniqueID algoID;
								std::string jointName;
								ss >> jointName;
								ss >> sa.offset[0];
								ss >> sa.offset[1];
								ss >> sa.offset[2];
								ss >> sa.rotation[0];
								ss >> sa.rotation[1];
								ss >> sa.rotation[2];
								ss >> sa.rotation[3];
								ss >> algoID;

								ret.sensorsAdjustments.insert(IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts::value_type(sID, sa));
								ret.sensorsMapping.insert(IMU::SensorsMapping::value_type(sID, jointName));
								ret.sensorsOrientationEstimationAlgorithms.insert(SerializableCostumeProfile::OrientationEstimationAlgorithmsMapping::value_type(sID, algoID));

							}
							else{
								finish = true;
								break;
							}
						}
					}
					else if (token == "CalibrationAlgoID"){
						std::stringstream ss;
						ss << valueToExtract;
						ss >> ret.calibrationAlgorithmID;
					}
					else if (token == "EstimationAlgoID"){
						std::stringstream ss;
						ss << valueToExtract;
						ss >> ret.motionEstimationAlgorithmID;						
					}

					std::getline(stream, line);
				}
			}
		}
		else{
			finish = true;
		}
	}

	return ret;
}