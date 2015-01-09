#include "PCH.h"
#include "DataSource.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <boost/lexical_cast.hpp>
#include <corelib/Filesystem.h>
#include <QtCore/QCoreApplication>
#include <corelib/PluginCommon.h>
#include "IMUCostumeListWidget.h"
#include <QtCore/QCoreApplication>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/hmdbCommunication/TreeItemHelper.h>
#include <iosfwd>
#include "IMUPerspective.h"
#include "CostumeSkeletonMotionHelper.h"

class OrientationEstimator
{
public:

	OrientationEstimator(const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping & orientationAlgorithms)
		: orientationAlgorithms(orientationAlgorithms), previousTime(0)
	{

	}

	void operator()(IMU::SensorsStreamData & data) const
	{
		double deltaTime = (data.timestamp > previousTime) ? (data.timestamp - previousTime) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previousTime + data.timestamp);
		previousTime = data.timestamp;

		for (auto & d : data.sensorsData)
		{
			auto it = orientationAlgorithms.find(d.first);

			d.second.orientation = it->second->estimate(d.second.accelerometer, d.second.gyroscope, d.second.magnetometer, deltaTime);
		}
	}

	~OrientationEstimator()
	{

	}

private:
	mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;
	const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping orientationAlgorithms;
};


enum
{
	AccIdx = 0,
	GyroIdx = 1,
	MagIdx = 2
};

class ArrayExtractor
{
public:
	ArrayExtractor(const unsigned int idx) : idx(idx) {}
	ArrayExtractor(const ArrayExtractor & Other) : idx(Other.idx) {}
	~ArrayExtractor() {}

	template<typename AT>
	bool verify(const AT & a) const
	{
		return true;
	}

	template<typename AT, typename Ret>
	void extract(const AT & a, Ret & ret) const
	{
		ret = a[idx];
	}

private:
	const unsigned int idx;
};

class OrientationExtractor
{
public:
	OrientationExtractor(const unsigned int idx) : idx(idx) {}
	OrientationExtractor(const OrientationExtractor & Other) : idx(Other.idx) {}
	~OrientationExtractor() {}
	
	bool verify(const IMU::SkeletonMotionState & a) const
	{
		return true;
	}

	void extract(const IMU::SkeletonMotionState & a, osg::Quat & ret) const
	{
		auto it = a.jointsOrientations.begin();
		std::advance(it, idx);
		ret = it->second;
	}

private:
	const unsigned int idx;
};

class CANopenDataExtractor
{
public:

	CANopenDataExtractor() {}

	bool verify(const IMU::CANopenFramesStream::value_type & a)
	{
		return true;
	}

	void extract(const IMU::CANopenFramesStream::value_type & a, IMU::CostumeStream::value_type & ret) const
	{
		ret = imuCostume::Costume::convert(a);		
	}
};

class ArrayStreamAdapter
{
public:

	template<typename Base, typename Dest>
	static threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor> * create(typename threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>::BaseStreamTypePtr baseStream, const unsigned int idx)
	{
		return new threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>(baseStream, ArrayExtractor(idx));
	}	
};

class ExtractCostumeMotion
{
public:
	ExtractCostumeMotion(
		kinematic::SkeletonConstPtr skeleton,
		const IMU::SensorsMapping & sensorsMapping,
		const IMU::DataIndexToJointMapping & dataMapping,
		const IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustments,
		IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm)
		: motionEstimationAlgorithm(motionEstimationAlgorithm),
		sensorsMapping(sensorsMapping), dataMapping(dataMapping), sensorsAdjustments(sensorsAdjustments),
		skeleton(skeleton)
	{

	}

	virtual ~ExtractCostumeMotion()
	{

	}

	bool verify(const IMU::SensorsStreamData & input) const
	{
		return true;
	}

	void extract(const IMU::SensorsStreamData & input, IMU::SkeletonMotionState & output) const
	{
		double deltaTime = deltaTime = (input.timestamp > previousTime) ? (input.timestamp - previousTime) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previousTime + input.timestamp);
		previousTime = input.timestamp;

		IMU::IMUCostumeMotionEstimationAlgorithm::MotionState motionState;
		motionState.position = osg::Vec3(0, 0, 0);		

		for (const auto & i : input.sensorsData)
		{
			const auto & adjustment = sensorsAdjustments.find(i.first)->second;			
			motionState.jointsOrientations.insert(std::map<std::string, osg::Quat>::value_type(sensorsMapping.left.find(i.first)->get_right(), i.second.orientation * adjustment.rotation.inverse()));
		}

		kinematic::SkeletonState ss(kinematic::SkeletonState::create(*skeleton));

		kinematic::SkeletonState::Joint::visit(ss.root(), [&motionState](kinematic::SkeletonState::JointPtr joint) -> void
		{
			joint->setGlobal(motionState.jointsOrientations.find(joint->name())->second);
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

private:
	kinematic::SkeletonConstPtr skeleton;	
	IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
	const IMU::SensorsMapping sensorsMapping;
	const IMU::DataIndexToJointMapping dataMapping;
	mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;
	const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping orientationAlgorithms;
	const IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;
};

typedef threadingUtils::StreamAdapterT<IMU::SensorsStreamData, IMU::SkeletonMotionState, ExtractCostumeMotion> RealMotionStream;

using namespace IMU;

IMUCostumeDataSource::IMUCostumeDataSource()
	: memoryDM(nullptr), finish(false)
{

}

IMUCostumeDataSource::~IMUCostumeDataSource()
{
}

void IMUCostumeDataSource::init(core::IMemoryDataManager * memoryDM,
	core::IStreamDataManager * streamDM,
	core::IFileDataManager * fileDM)
{
	this->memoryDM = memoryDM;
	refreshThread = plugin::getThreadPool()->get(name(), "Costumes data reader");
}

void IMUCostumeDataSource::resfreshCostumesData()
{
	while (finish == false){
		std::map<std::string, CostumeDescription> locCostumesDescription;
		{
			std::lock_guard<std::recursive_mutex > lock(synch);
			locCostumesDescription = costumesDescription;
		}

		if (locCostumesDescription.empty() == true){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else{

			imuCostume::CostumeRawIO::Buffer buffer;
			uint16_t length;

			for (const auto & cd : locCostumesDescription)
			{
				try{
					bool success = cd.second.rawCostume->receive(buffer, length, 100);
					if (success == true){
						imuCostume::ProtocolSendBufferHelper::Buffer streamBuffer;
						streamBuffer.buffer.reset(new uint8_t[length], utils::array_deleter<uint8_t>());
						streamBuffer.length = length;
						std::memcpy(streamBuffer.buffer.get(), buffer.data(), length);
						//raw data
						cd.second.rawDataStream->pushData(streamBuffer);						
					}
					else{
						PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " timeout on data receive");
					}
				}
				catch (...){
					PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " critically failed to receive data");
				}
			}
		}
	}
}

bool IMUCostumeDataSource::lateInit()
{
	refreshThread.run(&IMUCostumeDataSource::resfreshCostumesData, this);
	return true;
}

void IMUCostumeDataSource::finalize()
{
	finish = true;

	if (refreshThread.joinable() == true){
		refreshThread.join();
	}

	std::list<IIMUOrientationEstimationAlgorithmConstPtr>().swap(orientationEstimationAlgorithms_);
	std::list<IMUCostumeCalibrationAlgorithmConstPtr>().swap(calibrationAlgorithms_);
	std::list<IMUCostumeMotionEstimationAlgorithmConstPtr>().swap(motionEstimationAlgorithms_);
	std::list<kinematic::SkeletonConstPtr>().swap(skeletonModels_);
}

void IMUCostumeDataSource::update(double deltaTime)
{
}

QWidget* IMUCostumeDataSource::getWidget()
{
	return new IMUCostumeWidget(this);
}

QWidget* IMUCostumeDataSource::getControlWidget()
{
	return nullptr;
}

QWidget* IMUCostumeDataSource::getSettingsWidget()
{
	return nullptr;
}

void IMUCostumeDataSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{	
	offeredTypes.push_back(typeid(RawDataStream));
	offeredTypes.push_back(typeid(CANopenFramesStream));
	offeredTypes.push_back(typeid(CostumeStream));
	offeredTypes.push_back(typeid(IMUStream));
	offeredTypes.push_back(typeid(Vec3Stream));
	offeredTypes.push_back(typeid(ScalarStream));
}

const bool IMUCostumeDataSource::refreshCostumes()
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	auto costumes = imuCostume::CostumeRawIO::listAvailableCostumes();
	if (costumes.empty() == true){
		costumes = imuCostume::CostumeRawIO::listAvailableCostumes();
	}

	std::set<std::string> refreshedIPs;
	std::set<std::string> currentIPs;

	for (const auto & c : costumes){
		refreshedIPs.insert(c.ip);
	}

	for (const auto & c : costumesData){
		currentIPs.insert(c.first);
	}

	std::vector<std::string> toAdd(costumes.size());
	std::vector<std::string> toRemove(costumesData.size());

	auto removeIT = std::set_difference(currentIPs.begin(), currentIPs.end(), refreshedIPs.begin(), refreshedIPs.end(), toRemove.begin());
	auto addIT = std::set_difference(refreshedIPs.begin(), refreshedIPs.end(), currentIPs.begin(), currentIPs.end(), toAdd.begin());

	toAdd.resize(addIT - toAdd.begin());
	toRemove.resize(removeIT - toRemove.begin());

	auto it = toRemove.begin();

	while (it != toRemove.end()){
		auto cIT = costumesDescription.find(*it);
		if (cIT != costumesDescription.end()){
			//TODO - unload jak by³ za³adowany
			/*
			if (cIT->second.domainData.empty() == false){
				it = toRemove.erase(it);
			}
			else{
				++it;
			}
			*/
		}
		else{
			it = toRemove.erase(it);
		}
	}

	std::map<std::string, CostumeDescription> additionalCostumesData;

	for (const auto & ip : toAdd){
		try{
			CostumeDescription cd;
			cd.rawCostume.reset(new imuCostume::CostumeRawIO(ip, 1234));
			cd.status.reset(new std::atomic<ConnectionStatus>(ONLINE));
			configureCostume(cd);
			refreshCostumeSensorsConfiguration(cd);

			for (const auto & st : cd.sensorsConfiguration){
				for (const auto sid : st.second){
					cd.sensorsStatus[sid].status.reset(new std::atomic<ConnectionStatus>(UNKNOWN));
				}
			}

			additionalCostumesData.insert(std::map<std::string, CostumeDescription>::value_type(ip, cd));
		}
		catch (...){
			//TODO
			//info o nieudanej probie za³adowania kostiumu
		}
	}

	for (const auto & ip : toRemove){
		costumesDescription.erase(ip);
	}

	costumesDescription.insert(additionalCostumesData.begin(), additionalCostumesData.end());

	return costumes.empty() == false;
}

void IMUCostumeDataSource::configureCostume(CostumeDescription & cd)
{
	imuCostume::ProtocolSendBufferHelper psbh;
	imuCostume::CostumeCANopenIO::prepareCostumeConfiguration(psbh);
	imuCostume::Costume::prepareCostumeConfiguration(psbh);

	while (psbh.empty() == false)
	{
		auto buff = psbh.createBuffer();
		cd.rawCostume->send(buff.buffer.get(), buff.length);
	}
}

void IMUCostumeDataSource::refreshCostumeSensorsConfiguration(CostumeDescription & cd)
{
	static const uint8_t MaxSamplesCount = 25;
	imuCostume::CostumeCANopenIO::Frame frame = { 0 };
	uint16_t length = 0;	

	for (unsigned int i = 0; i < MaxSamplesCount; ++i){		
		if (cd.rawCostume->receive(frame.buffer, length, 100) == true){
			if (length > 1){
				auto sc = imuCostume::Costume::sensorsConfiguration(frame.structure.data.data(), length - sizeof(imuCostume::CostumeCANopenIO::Header));				
				for (const auto & st : sc){
					auto it = cd.sensorsConfiguration.find(st.first);
					if (it == cd.sensorsConfiguration.end()){
						cd.sensorsConfiguration.insert(st);
					}
					else{
						it->second.insert(st.second.begin(), st.second.end());
					}
				}
			}
		}
	}
}

const unsigned int IMUCostumeDataSource::costumesCout() const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	return costumesDescription.size();
}

bool IMUCostumeDataSource::costumeLoaded(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	const auto & cd = costumeDescription(idx);
	auto ip = cd.rawCostume->ip();
	auto it = costumesData.find(ip);
	return it != costumesData.end();
}

std::string IMUCostumeDataSource::sensorParameterName(const unsigned int idx)
{
	switch (idx)
	{
	case AccIdx:	return QObject::tr("Accelerometer").toStdString();
	case GyroIdx:	return QObject::tr("Gyroscope").toStdString();
	case MagIdx:	return QObject::tr("Magnetometer").toStdString();
	default:		return QObject::tr("Unknown").toStdString();
	}
}

std::string IMUCostumeDataSource::vectorParameterName(const unsigned int idx)
{
	switch (idx)
	{
	case 0:	return "x";
	case 1:	return "y";
	case 2:	return "z";
	case 3:	return "w";
	default: return QObject::tr("Unknown").toStdString();
	}
}

class RawToCANopenExtractor
{
public:

	RawToCANopenExtractor() {}
	~RawToCANopenExtractor() {}

	bool verify(const RawDataStream::value_type & val) const
	{
		return true;
	}

	void extract(const RawDataStream::value_type & in, CANopenFramesStream::value_type & out) const
	{
		out = imuCostume::CostumeCANopenIO::extractData(in.buffer.get(), in.length);
	}
};

void IMUCostumeDataSource::fillRawCostumeData(CostumeData & cData, const CostumeDescription & cd)
{
	const auto & sc = cd.sensorsConfiguration;
	auto ow = core::Variant::create<RawDataStream>();
	ow->setMetadata("core/name", QObject::tr("Raw Data").toStdString());
	ow->set(cd.rawDataStream);
	cData.domainData.push_back(ow);

	//new CANopenFramesStream
	cData.CANopenStream.reset(new threadingUtils::StreamAdapterT<RawDataStream::value_type, CANopenFramesStream::value_type, RawToCANopenExtractor>(cd.rawDataStream, RawToCANopenExtractor()));
	ow = core::Variant::create<CANopenFramesStream>();
	ow->setMetadata("core/name", QObject::tr("CANopen Data").toStdString());
	ow->set(cData.CANopenStream);
	cData.domainData.push_back(ow);

	cData.costumeStream.reset(new threadingUtils::StreamAdapterT<CANopenFramesStream::value_type, CostumeStream::value_type, CANopenDataExtractor>(cData.CANopenStream, CANopenDataExtractor()));
	ow = core::Variant::create<CostumeStream>();
	ow->setMetadata("core/name", QObject::tr("Costume Data").toStdString());
	ow->set(cData.costumeStream);
	cData.domainData.push_back(ow);

	auto sIT = sc.find(imuCostume::Costume::IMU);
	if (sIT != sc.end()){

		std::map<imuCostume::Costume::SensorID, SensorData> sData;

		for (auto & sID : sIT->second){
			SensorData sd;
			sd.dataStream.reset(new threadingUtils::StreamT<IMUData>);

			ow = core::Variant::create<IMUStream>();
			ow->setMetadata("core/name", QObject::tr("IMU %1 Data").arg(sID).toStdString());
			ow->set(sd.dataStream);
			cData.domainData.push_back(ow);

			for (unsigned int i = 0; i < 3; ++i){
				sd.vec3dStreams.push_back(utils::shared_ptr<Vec3Stream>(new threadingUtils::StreamT<osg::Vec3>));

				ow = core::Variant::create<Vec3Stream>();
				ow->setMetadata("core/name", sensorParameterName(i));
				ow->set(sd.vec3dStreams.back());
				cData.domainData.push_back(ow);

				for (unsigned int j = 0; j < 3; ++j){
					sd.scalarStreams.push_back(utils::shared_ptr<ScalarStream>(ArrayStreamAdapter::create<osg::Vec3, osg::Vec3::value_type>(sd.vec3dStreams.back(), j)));

					ow = core::Variant::create<ScalarStream>();
					ow->setMetadata("core/name", vectorParameterName(j));
					ow->set(sd.scalarStreams.back());
					cData.domainData.push_back(ow);
				}
			}

			sd.orientationStream.reset(new threadingUtils::StreamT<osg::Quat>);

			for (unsigned int j = 0; j < 4; ++j){
				sd.scalarStreams.push_back(utils::shared_ptr<ScalarStream>(ArrayStreamAdapter::create<osg::Quat, float>(sd.orientationStream, j)));
				ow = core::Variant::create<ScalarStream>();
				ow->setMetadata("core/name", "Orientation");
				ow->set(sd.scalarStreams.back());
				cData.domainData.push_back(ow);
			}

			sData.insert(std::map<imuCostume::Costume::SensorID, SensorData>::value_type(sID, sd));
		}

		cData.sensorsData = std::move(sData);
	}
}

void IMUCostumeDataSource::loadRawCostume(const unsigned int idx)
{
	std::lock_guard<std::recursive_mutex > lock(synch);	
	const auto & cd = costumeDescription(idx);
	auto ip = cd.rawCostume->ip();
	auto it = costumesData.find(ip);
	if (it != costumesData.end()){
		throw std::runtime_error("Costume already loaded");
	}

	CostumeData cData;
	fillRawCostumeData(cData, cd);

	try{
		auto t = memoryDM->transaction();
		for (const auto & ow : cData.domainData){
			t->addData(ow);
		}

		costumesData.insert(std::map<std::string, CostumeData>::value_type(ip, cData));
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());		
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}

void IMUCostumeDataSource::loadCalibratedCostume(const unsigned int idx,
	kinematic::SkeletonConstPtr skeleton,
	const SensorsMapping & sensorsMapping,
	const IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustments,
	const OrientationEstimationAlgorithmsMapping & orientationAlgorithms,
	IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm)
{
	std::lock_guard<std::recursive_mutex > lock(synch);	
	
	const auto & cd = costumeDescription(idx);

	if (orientationAlgorithms.size() != cd.sensorsStatus.size())
	{
		throw std::runtime_error("Mismatch in number of loaded sensors orientation estimation algorithms and costume configuration");
	}

	auto ip = cd.rawCostume->ip();
	auto it = costumesData.find(ip);
	if (it != costumesData.end()){
		throw std::runtime_error("Costume already loaded");
	}

	CostumeData cData;
	fillRawCostumeData(cData, cd);

	//adapter na pe³ne dane z kostiumu napêdzaj¹ce strumieñ z estymacj¹
	cData.completeImuStream.reset(new ExtractedCostumeStreamAdapter(cData.costumeStream, CostumeIMUExtractor(cd.sensorsConfiguration)));

	auto ow = core::Variant::create<CostumeSkeletonMotionHelper::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Complete unpacked IMU data").toStdString());
	ow->set(cData.completeImuStream);
	cData.domainData.push_back(ow);

	auto estimatedData = utils::make_shared<threadingUtils::StreamProcessorT<IMU::SensorsStreamData, OrientationEstimator>>(cData.completeImuStream, OrientationEstimator(orientationAlgorithms));

	ow = core::Variant::create<CostumeSkeletonMotionHelper::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Complete filtered IMU data").toStdString());
	ow->set(estimatedData);
	cData.domainData.push_back(ow);

	//TODO dodaæ kwaterniony po estymacji + skalary

	cData.skeletonMotion = utils::make_shared<IMU::CostumeSkeletonMotion>();
	ow = core::Variant::create<IMU::CostumeSkeletonMotion>();
	ow->setMetadata("core/name", QObject::tr("Complete skeleton motion stream").toStdString());
	ow->set(cData.skeletonMotion);
	cData.domainData.push_back(ow);

	//mapowanie pozycji wektora do nazwy jointa w szkielecie i stanie

	kinematic::Joint::visit(skeleton->root, [cData](kinematic::JointPtr joint) -> void
	{
		cData.skeletonMotion->dataToModelMapping.insert(DataIndexToJointMapping::value_type(cData.skeletonMotion->dataToModelMapping.size(), joint->name));
	});

	cData.skeletonMotion->skeleton = skeleton;
	cData.skeletonMotion->stream.reset(new RealMotionStream(estimatedData, ExtractCostumeMotion(skeleton, sensorsMapping, cData.skeletonMotion->dataToModelMapping, sensorsAdjustments, motionEstimationAlgorithm)));
	ow = core::Variant::create<MotionStream>();
	ow->setMetadata("core/name", QObject::tr("Calibrated skeleton motion stream").toStdString());
	ow->set(cData.skeletonMotion);
	cData.domainData.push_back(ow);

	//adapter na orientacje po estymacji

	auto sIT = cd.sensorsConfiguration.find(imuCostume::Costume::IMU);
	if (sIT != cd.sensorsConfiguration.end()){

		for (auto & sID : sIT->second){

			auto it = sensorsMapping.left.find(sID);

			//znaleŸæ mapowanie i indeks iteratora
			auto idx = cData.skeletonMotion->dataToModelMapping.right.find(it->second)->get_left();

			auto processedOrientation = utils::make_shared<threadingUtils::StreamAdapterT<IMU::SkeletonMotionState, osg::Quat, OrientationExtractor>>(cData.skeletonMotion->stream, OrientationExtractor(idx));
			ow = core::Variant::create<threadingUtils::IStreamT<osg::Quat>>();
			ow->setMetadata("core/name", "Orientation");
			ow->set(processedOrientation);
			cData.domainData.push_back(ow);
			//TODO - podzia³ kwaternionu na skalary
			/*
			for (unsigned int j = 0; j < 4; ++j){
				sd.scalarStreams.push_back(utils::shared_ptr<ScalarStream>(ArrayStreamAdapter::create<osg::Quat, float>(sd.orientationStream, j)));
				ow = core::Variant::create<ScalarStream>();
				ow->setMetadata("core/name", "Orientation");
				ow->set(sd.scalarStreams.back());
				cData.domainData.push_back(ow);
			}

			sData.insert(std::map<imuCostume::Costume::SensorID, SensorData>::value_type(sID, sd));
			*/

			cData.domainData.push_back(ow);
		}

		//cData.sensorsData = std::move(sData);
	}





	try{
		auto t = memoryDM->transaction();
		for (const auto & ow : cData.domainData){
			t->addData(ow);
		}

		costumesData.insert(std::map<std::string, CostumeData>::value_type(ip, cData));
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}

void IMUCostumeDataSource::unloadCostume(const unsigned int idx)
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	const auto & cd = costumeDescription(idx);
	auto ip = cd.rawCostume->ip();
	auto it = costumesData.find(ip);
	if (it != costumesData.end()){
		auto transaction = memoryDM->transaction();

		for (const auto & d : it->second.domainData){
			transaction->tryRemoveData(d);
		}

		//TODO - update hierarchy in analysis
		costumesData.erase(it);
	}
	else{
		throw std::runtime_error("Costume not loaded");
	}
}

unsigned int IMUCostumeDataSource::loadedCostumesCount() const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	return costumesData.size();
}

core::ConstVariantsList IMUCostumeDataSource::costumeData(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex > lock(synch);

	core::ConstVariantsList ret;

	const auto & cd = costumeDescription(idx);
	auto ip = cd.rawCostume->ip();
	auto it = costumesData.find(ip);
	if (it != costumesData.end()){
		for (const auto & d : it->second.domainData){
			ret.push_back(d);
		}
	}

	return ret;
}

core::HierarchyItemPtr IMUCostumeDataSource::createRootItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("Costumes data"), QObject::tr("Costumes data"), QIcon());
}

core::HierarchyItemPtr IMUCostumeDataSource::createStreamItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("Streams"), QObject::tr("Costumes data streams"), QIcon());
}

core::HierarchyItemPtr IMUCostumeDataSource::createRecordItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("Recordings"), QObject::tr("Costumes data recordings"), QIcon());
}

core::HierarchyItemPtr IMUCostumeDataSource::createIMUsItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("IMUs"), QObject::tr("IMUs data"), QIcon());
}

void IMUCostumeDataSource::tryCreateRootItem()
{
	if (root == nullptr){
		root = createRootItem();
		auto hierarchyTransaction = memoryDM->hierarchyTransaction();
		hierarchyTransaction->addRoot(root);
	}
}

void IMUCostumeDataSource::tryCreateStreamItem()
{
	if (streamItems == nullptr){
		streamItems = createStreamItem();
		tryCreateRootItem();
		root->appendChild(streamItems);
	}
}

void IMUCostumeDataSource::tryCreateRecordedItem()
{
	if (recordedItems == nullptr){
		recordedItems = createRecordItem();
		tryCreateRootItem();
		root->appendChild(recordedItems);
	}
}
/*
void IMUCostumeDataSource::loadAllCostumes()
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	
	for (unsigned int i = 0; i < costumesData.size(); ++i){
		if (costumeLoaded(i) == false){
			innerLoadCostume(i);
		}
	}
}

void IMUCostumeDataSource::unloadAllCostumes()
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	
	for (unsigned int i = 0; i < costumesData.size(); ++i){
		if (costumeLoaded(i) == true){
			innerUnloadCostume(i);
		}
	}
}
*/

void IMUCostumeDataSource::registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithm * algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = std::find_if(orientationEstimationAlgorithms_.begin(), orientationEstimationAlgorithms_.end(), [algorithm](std::list<IIMUOrientationEstimationAlgorithmConstPtr>::value_type val)
	{
		return val.get() == algorithm;
	});
	if (it != orientationEstimationAlgorithms_.end()){
		throw std::runtime_error("Orientation estimation algorithm already registered");
	}

	orientationEstimationAlgorithms_.push_back(IIMUOrientationEstimationAlgorithmConstPtr(algorithm));
}

void IMUCostumeDataSource::registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithm * algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = std::find_if(calibrationAlgorithms_.begin(), calibrationAlgorithms_.end(), [algorithm](std::list<IMUCostumeCalibrationAlgorithmConstPtr>::value_type val)
	{
		return val.get() == algorithm;
	});
	if (it != calibrationAlgorithms_.end()){
		throw std::runtime_error("Costume calibration algorithm already registered");
	}

	calibrationAlgorithms_.push_back(IMUCostumeCalibrationAlgorithmConstPtr(algorithm));
}

void IMUCostumeDataSource::registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithm * algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = std::find_if(motionEstimationAlgorithms_.begin(), motionEstimationAlgorithms_.end(), [algorithm](std::list<IMUCostumeMotionEstimationAlgorithmConstPtr>::value_type val)
	{
		return val.get() == algorithm;
	});
	if (it != motionEstimationAlgorithms_.end()){
		throw std::runtime_error("Motion estimation algorithm already registered");
	}

	motionEstimationAlgorithms_.push_back(IMUCostumeMotionEstimationAlgorithmConstPtr(algorithm));
}

void IMUCostumeDataSource::registerSkeletonModel(kinematic::SkeletonConstPtr skeleton)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	//TODO - weryfikacja czy model ju¿ istnieje
	skeletonModels_.push_back(skeleton);
}

std::list<IIMUOrientationEstimationAlgorithmConstPtr> IMUCostumeDataSource::orientationEstimationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return orientationEstimationAlgorithms_;
}

std::list<IMUCostumeCalibrationAlgorithmConstPtr> IMUCostumeDataSource::calibrationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return calibrationAlgorithms_;
}

std::list<IMUCostumeMotionEstimationAlgorithmConstPtr> IMUCostumeDataSource::motionEstimationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return motionEstimationAlgorithms_;
}

std::list<kinematic::SkeletonConstPtr> IMUCostumeDataSource::skeletonModels() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return skeletonModels_;
}

/*
kinematic::SkeletonMappingSchemePtr IMUCostumeDataSource::createMappingScheme()
{
	kinematic::SkeletonMappingScheme::segmentsMappingDict dict;

	dict["hip"] = kinematic::SkeletonMappingScheme::segmentsRange("pelvis", "pelvis");
	dict["chest"] = kinematic::SkeletonMappingScheme::segmentsRange("t1", "t6");
	dict["neck"] = kinematic::SkeletonMappingScheme::segmentsRange("c1", "c7");
	dict["head"] = kinematic::SkeletonMappingScheme::segmentsRange("skull", "skull");
	dict["rCollar"] = kinematic::SkeletonMappingScheme::segmentsRange("r_clavicle", "r_scapula");
	dict["rShldr"] = kinematic::SkeletonMappingScheme::segmentsRange("r_upperarm", "r_upperarm");
	dict["rForeArm"] = kinematic::SkeletonMappingScheme::segmentsRange("r_forearm", "r_forearm");
	dict["rHand"] = kinematic::SkeletonMappingScheme::segmentsRange("r_hand", "r_hand");
	dict["lCollar"] = kinematic::SkeletonMappingScheme::segmentsRange("l_clavicle", "l_scapula");
	dict["lShldr"] = kinematic::SkeletonMappingScheme::segmentsRange("l_upperarm", "l_upperarm");
	dict["lForeArm"] = kinematic::SkeletonMappingScheme::segmentsRange("l_forearm", "l_forearm");
	dict["lHand"] = kinematic::SkeletonMappingScheme::segmentsRange("l_hand", "l_hand");
	dict["rButtock"] = kinematic::SkeletonMappingScheme::segmentsRange("r_thigh", "r_thigh");
	dict["rShin"] = kinematic::SkeletonMappingScheme::segmentsRange("r_calf", "r_calf");
	dict["rFoot"] = kinematic::SkeletonMappingScheme::segmentsRange("r_middistal", "r_middistal");
	dict["lButtock"] = kinematic::SkeletonMappingScheme::segmentsRange("l_thigh", "l_thigh");
	dict["lShin"] = kinematic::SkeletonMappingScheme::segmentsRange("l_calf", "l_calf");
	dict["lFoot"] = kinematic::SkeletonMappingScheme::segmentsRange("l_middistal", "l_middistal");

	return kinematic::SkeletonMappingScheme::create(dict);
}
*/

const IMUCostumeDataSource::CostumeDescription & IMUCostumeDataSource::costumeDescription(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	auto it = costumesDescription.begin();
	std::advance(it, idx);

	return it->second;
}

/*
void IMUCostumeDataSource::refreshData()
{
	imuCostume::CostumeCANopenIO::Frame frame = { 0 };
	uint16_t length = 0;

	while (refreshData_ == true){		
		std::map<std::string, CostumeData> locCostumesData;
		{
			std::lock_guard<std::recursive_mutex> lock(synch);
			locCostumesData = costumesData;
		}

		if (locCostumesData.empty() == true){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else{

			for (auto & c : costumesData){
				if (c.second.rawCostume->receive(frame.buffer, length, 100) == true){
					if (length > 1 && c.second.domainData.empty() == false){						
						imuCostume::ProtocolSendBufferHelper::Buffer buffer;
						buffer.buffer.reset(new uint8_t[length], utils::array_deleter<uint8_t>());
						buffer.length = length;
						std::memcpy(buffer.buffer.get(), frame.buffer.data(), length);
						//raw data
						c.second.rawDataStream->pushData(buffer);
						//CANopen data + auto unpack to sensor data
						auto cd = imuCostume::CostumeCANopenIO::extractData(frame.structure.data.data(), length - sizeof(imuCostume::CostumeCANopenIO::Header));
						c.second.CANopenStream->pushData(cd);

						imuCostume::Costume::Data data;
						c.second.costumeStream->data(data);

						for (const auto & s : data.sensorsData){

							auto id = s->id();

							auto it = c.second.sensorsData.find(id);

							if (it != c.second.sensorsData.end()){

								if (s->type() == imuCostume::Costume::IMU){
									auto imu = utils::dynamic_pointer_cast<const imuCostume::Costume::IMUSensor>(s);
									if (imu != nullptr){
										IMUData imuData;
										imuData.status = imu->dataStatus();
										if (imuData.status & imuCostume::Costume::IMUSensor::ACC_DATA){
											imuData.accelerometer = imu->accelerometer();
											it->second.vec3dStreams[AccIdx]->pushData(imuData.accelerometer);
										}

										if (imuData.status & imuCostume::Costume::IMUSensor::GYRO_DATA){
											imuData.gyroscope = imu->gyroscope();
											it->second.vec3dStreams[GyroIdx]->pushData(imuData.gyroscope);
										}

										if (imuData.status & imuCostume::Costume::IMUSensor::MAG_DATA){
											imuData.magnetometer = imu->magnetometer();
											it->second.vec3dStreams[MagIdx]->pushData(imuData.magnetometer);
										}

										if (imuData.status & imuCostume::Costume::IMUSensor::ORIENT_DATA){
											imuData.orientation = imu->orientation();
											it->second.orientationStream->pushData(imuData.orientation);
										}

										it->second.dataStream->pushData(imuData);
									}
								}
							}
						}


						//TODO
						//aktualizacja strumieni sensorów i ich statusów					
					}
				}
				else{
					//status update!!
				}
			}
		}
	}

	PLUGIN_LOG_DEBUG("Finished thread loop");
}
*/

/*
void IMU::IMUCostumeDataSource::loadDatFile(const core::Filesystem::Path& path)
{
    auto transaction = fileDM->transaction();
    transaction->addFile(path);
    core::ConstVariantsList oList;
    transaction->getObjects(path, oList);

    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
    core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(path.filename().string().c_str(), path.string().c_str(), QIcon());
	auto config = utils::make_shared<IMU::IMUConfig>();
	IMUPerspective::createIMUBranch(oList, config, path.filename().string(), root);


    hierarchyTransaction->addRoot(root);
}
*/