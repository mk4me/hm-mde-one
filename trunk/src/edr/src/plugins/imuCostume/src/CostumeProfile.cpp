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

CostumeProfile * CostumeProfile::clone() const
{
	std::unique_ptr<CostumeProfile> ret(new CostumeProfile);

	ret->name = name;

	if (skeleton != nullptr){
		ret->skeleton.reset(new Skeleton(*skeleton));
	}

	if (calibrationAlgorithm != nullptr){
		ret->calibrationAlgorithm.reset(calibrationAlgorithm->create());
	}

	if (motionEstimationAlgorithm != nullptr){
		ret->motionEstimationAlgorithm.reset(motionEstimationAlgorithm->create());
	}

	for (const auto & sd : sensorsDescriptions)
	{
		SensorDescription locsd;
		locsd.jointName = sd.second.jointName;
		locsd.offset = sd.second.offset;
		locsd.rotation = sd.second.rotation;
		if (sd.second.orientationEstimationAlgorithm != nullptr){
			locsd.orientationEstimationAlgorithm.reset(sd.second.orientationEstimationAlgorithm->create());
		}

		ret->sensorsDescriptions.insert(SensorsDescriptions::value_type(sd.first, locsd));
	}

	return ret.release();
}

SerializableCostumeProfile SerializableCostumeProfile::pack(const CostumeProfile & profile)
{
	SerializableCostumeProfile ret;
	ret.name = profile.name;
	ret.skeletonID = profile.skeleton == nullptr ? boost::uuids::nil_uuid() : profile.skeleton->ID();
	ret.calibrationAlgorithmID = profile.calibrationAlgorithm == nullptr ? boost::uuids::nil_uuid() : profile.calibrationAlgorithm->ID();
	ret.motionEstimationAlgorithmID = profile.motionEstimationAlgorithm == nullptr ? boost::uuids::nil_uuid() : profile.motionEstimationAlgorithm->ID();

	for (const auto & sd : profile.sensorsDescriptions)
	{
		SensorDescription local;
		local.jointName = sd.second.jointName;
		local.offset = sd.second.offset;
		local.rotation = sd.second.rotation;
		local.orientationEstimationAlgorithmID = sd.second.orientationEstimationAlgorithm == nullptr ? boost::uuids::nil_uuid() : sd.second.orientationEstimationAlgorithm->ID();
		ret.sensorsDescriptions.insert(SensorsDescriptions::value_type(sd.first, local));
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

	//szkielet
	{
		auto sIT = skeletons.find(profile.skeletonID);

		if (sIT != skeletons.end()){
			ret.skeleton = utils::make_shared < IMU::Skeleton >(*sIT->second);
		}
	}

	//motion estimation algorithm
	{
		auto mIT = motionEstimationAlgos.find(profile.motionEstimationAlgorithmID);

		if (mIT != motionEstimationAlgos.end()){
			ret.motionEstimationAlgorithm.reset(mIT->second->create());
		}
	}

	//kalibracja
	{
		auto cIT = calibrationAlgos.find(profile.calibrationAlgorithmID);

		if (cIT != calibrationAlgos.end()){
			ret.calibrationAlgorithm.reset(cIT->second->create());
		}
	}

	//konfiguracja sensor�w
	for (const auto & ea : profile.sensorsDescriptions)
	{
		CostumeProfile::SensorDescription local;
		local.jointName = ea.second.jointName;
		local.offset = ea.second.offset;
		local.rotation = ea.second.rotation;

		auto eIT = orientationEstimationAlgos.find(ea.second.orientationEstimationAlgorithmID);
		if (eIT != orientationEstimationAlgos.end()){
			local.orientationEstimationAlgorithm.reset(eIT->second->create());
		}

		ret.sensorsDescriptions.insert(CostumeProfile::SensorsDescriptions::value_type(ea.first, local));
	}

	return ret;
}

SerializableCostumeProfile::VerificationStatus SerializableCostumeProfile::verify(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds)
{
	if (profile.name.empty() == true){
		return Failed;
	}

	if (profile.sensorsDescriptions.empty() == true
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

	for (const auto & ea : profile.sensorsDescriptions)
	{
		auto eIT = orientationEstimationAlgos.find(ea.second.orientationEstimationAlgorithmID);

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
	stream << "SensorsConfiguration: " << sProfile.sensorsDescriptions.size() << std::endl;

	for (const auto & m : sProfile.sensorsDescriptions)
	{		
		std::string sID = std::to_string(m.first);

		stream << sID << ": " << m.second.jointName << " " << m.second.offset[0] << " " << m.second.offset[1] << " " << m.second.offset[2]
			<< " " << m.second.rotation[0] << " " << m.second.rotation[1] << " " << m.second.rotation[2] << " " << m.second.rotation[3] << " " << m.second.orientationEstimationAlgorithmID << std::endl;
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
	profileTokens.insert("SensorsConfiguration");
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
					else if (token == "SensorsConfiguration"){
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
								SensorDescription sd;
								
								ss >> sd.jointName;
								ss >> sd.offset[0];
								ss >> sd.offset[1];
								ss >> sd.offset[2];
								ss >> sd.rotation[0];
								ss >> sd.rotation[1];
								ss >> sd.rotation[2];
								ss >> sd.rotation[3];
								ss >> sd.orientationEstimationAlgorithmID;

								ret.sensorsDescriptions.insert(SensorsDescriptions::value_type(sID, sd));
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