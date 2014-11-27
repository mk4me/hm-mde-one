#include "PCH.h"
#include "DataSource.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <boost/lexical_cast.hpp>
#include <kinematiclib/BvhParser.h>
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

class ArrayExtractor
{
public:
	ArrayExtractor(const unsigned int idx) : idx(idx) {}
	ArrayExtractor(const ArrayExtractor & Other) : idx(Other.idx) {}
	~ArrayExtractor() {}

	template<typename AT, typename Ret>
	void operator()(const AT & a, Ret & ret) const
	{
		ret = a[idx];
	}

private:
	const unsigned int idx;
};

class CANopenDataExtractor
{
public:

	CANopenDataExtractor() {}

	void operator()(const IMU::IMUCostumeDataSource::CANopenFramesStream::value_type & a, IMU::IMUCostumeDataSource::CostumeStream::value_type & ret) const
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


using namespace IMU;

IMUCostumeDataSource::IMUCostumeDataSource()
: refreshData_(true), memoryDM(nullptr)
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
	refreshCostumes();
	refreshThread = plugin::getThreadPool()->get("IMUCostumeDataSource", "Costumes communication");
	refreshThread.run(&IMUCostumeDataSource::refreshData, this);
}

bool IMUCostumeDataSource::lateInit()
{
	return true;
}

void IMUCostumeDataSource::finalize()
{
	refreshData_ = false;
	if (refreshThread.joinable() == true){
		refreshThread.join();
	}

	std::map<std::string, CostumeData>().swap(costumesData);
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
		auto cIT = costumesData.find(*it);
		if (cIT != costumesData.end()){
			if (cIT->second.domainData.empty() == false){
				it = toRemove.erase(it);
			}
			else{
				++it;
			}
		}
		else{
			it = toRemove.erase(it);
		}
	}

	std::map<std::string, CostumeData> additionalCostumesData;

	for (const auto & ip : toAdd){
		try{
			CostumeData cd;
			cd.rawCostume.reset(new imuCostume::CostumeRawIO(ip, 1234));
			cd.status.reset(new std::atomic<ConnectionStatus>(ONLINE));
			configureCostume(cd);
			refreshCostumeSensorsConfiguration(cd);

			for (const auto & st : cd.sensorsConfiguration){
				for (const auto sid : st.second){
					cd.sensorsData[sid].status.reset(new std::atomic<ConnectionStatus>(UNKNOWN));
				}
			}

			additionalCostumesData.insert(std::map<std::string, CostumeData>::value_type(ip, cd));
		}
		catch (...){
			//TODO
			//info o nieudanej probie za³adowania kostiumu
		}
	}

	std::lock_guard<std::recursive_mutex> lock(updateSynch);

	for (const auto & ip : toRemove){
		costumesData.erase(ip);
	}

	costumesData.insert(additionalCostumesData.begin(), additionalCostumesData.end());

	return costumes.empty() == false;
}

void IMUCostumeDataSource::configureCostume(CostumeData & cd)
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

void IMUCostumeDataSource::refreshCostumeSensorsConfiguration(CostumeData & cd)
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
	return costumesData.size();
}

const imuCostume::Costume::SensorsConfiguration & IMUCostumeDataSource::costumeConfiguration(unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.begin();
	std::advance(it, idx);
	return it->second.sensorsConfiguration;
}

const imuCostume::CostumeRawIO::CostumeAddress IMUCostumeDataSource::costumeAddress(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.begin();
	std::advance(it, idx);
	return imuCostume::CostumeRawIO::CostumeAddress({ it->first, 1234 });
}

const IMUCostumeDataSource::ConnectionStatus IMUCostumeDataSource::costumeStatus(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.begin();
	std::advance(it, idx);
	return *(it->second.status);
}

const IMUCostumeDataSource::RawDataStreamPtr IMUCostumeDataSource::costumeRawDataStream(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.begin();
	std::advance(it, idx);
	return it->second.rawDataStream;
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

void IMUCostumeDataSource::innerLoadCostume(const unsigned int idx)
{
	auto it = costumesData.begin();
	std::advance(it, idx);
	const auto & sc = it->second.sensorsConfiguration;

	core::VariantsList ows;

	it->second.rawDataStream.reset(new RawDataStream);
	auto ow = core::Variant::create<RawDataStream>();
	ow->setMetadata("core/name", QObject::tr("Raw Data").toStdString());
	ow->set(it->second.rawDataStream);
	ows.push_back(ow);

	it->second.CANopenStream.reset(new CANopenFramesStream);
	ow = core::Variant::create<CANopenFramesStream>();
	ow->setMetadata("core/name", QObject::tr("CANopen Data").toStdString());
	ow->set(it->second.CANopenStream);
	ows.push_back(ow);

	it->second.costumeStream.reset(new threadingUtils::StreamAdapterT<CANopenFramesStream::value_type, CostumeStream::value_type, CANopenDataExtractor>(it->second.CANopenStream, CANopenDataExtractor()));
	ow = core::Variant::create<CostumeStream>();
	ow->setMetadata("core/name", QObject::tr("Costume Data").toStdString());
	ow->set(it->second.costumeStream);
	ows.push_back(ow);

	for (auto & sd : it->second.sensorsData){
		sd.second.dataStream.reset(new threadingUtils::StreamT<IMUData>);

		ow = core::Variant::create<IMUStream>();
		ow->setMetadata("core/name", QObject::tr("IMU %1 Data").arg(sd.first).toStdString());
		ow->set(sd.second.dataStream);
		ows.push_back(ow);

		for (unsigned int i = 0; i < 3; ++i){
			sd.second.vec3dStreams.push_back(utils::shared_ptr<Vec3Stream>(new threadingUtils::StreamT<osg::Vec3>));

			ow = core::Variant::create<Vec3Stream>();
			ow->setMetadata("core/name", sensorParameterName(i));
			ow->set(sd.second.vec3dStreams.back());
			ows.push_back(ow);

			for (unsigned int j = 0; j < 3; ++j){
				sd.second.scalarStreams.push_back(utils::shared_ptr<ScalarStream>(ArrayStreamAdapter::create<osg::Vec3, osg::Vec3::value_type>(sd.second.vec3dStreams.back(), j)));

				ow = core::Variant::create<ScalarStream>();
				ow->setMetadata("core/name", vectorParameterName(j));
				ow->set(sd.second.scalarStreams.back());
				ows.push_back(ow);
			}
		}

		sd.second.orientationStream.reset(new threadingUtils::StreamT<osg::Quat>);

		for (unsigned int j = 0; j < 4; ++j){
			sd.second.scalarStreams.push_back(utils::shared_ptr<ScalarStream>(ArrayStreamAdapter::create<osg::Quat, float>(sd.second.orientationStream, j)));
			ow = core::Variant::create<ScalarStream>();
			ow->setMetadata("core/name", vectorParameterName(j));
			ow->set(sd.second.scalarStreams.back());
			ows.push_back(ow);
		}
	}

	try{
		auto t = memoryDM->transaction();
		for (const auto & ow : ows){
			t->addData(ow);
		}


		it->second.domainData = ows;
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());
		it->second.rawDataStream.reset();
		it->second.CANopenStream.reset();
		it->second.costumeStream.reset();

		for (auto & sd : it->second.sensorsData){
			sd.second.dataStream.reset();
			sd.second.orientationStream.reset();
			std::vector<utils::shared_ptr<Vec3Stream>>().swap(sd.second.vec3dStreams);
			std::list<utils::shared_ptr<ScalarStream>>().swap(sd.second.scalarStreams);
		}
	}
	catch (...){
		it->second.rawDataStream.reset();
		it->second.CANopenStream.reset();
		it->second.costumeStream.reset();

		for (auto & sd : it->second.sensorsData){
			sd.second.dataStream.reset();
			sd.second.orientationStream.reset();
			std::vector<utils::shared_ptr<Vec3Stream>>().swap(sd.second.vec3dStreams);			
			std::list<utils::shared_ptr<ScalarStream>>().swap(sd.second.scalarStreams);			
		}
	}
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

void IMUCostumeDataSource::innerUnloadCostume(const unsigned int idx)
{
	auto it = costumesData.begin();
	std::advance(it, idx);
	//usuwam dane z DM
	{
		auto t = memoryDM->transaction();

		for (auto dIT = it->second.domainData.begin(); dIT != it->second.domainData.end(); ++it){
			t->removeData(*dIT);
		}
	}

	{
		std::lock_guard<std::recursive_mutex> lock(synch);
		core::VariantsList().swap(it->second.domainData);
		it->second.rawDataStream.reset();
		it->second.CANopenStream.reset();
		it->second.costumeStream.reset();
	}
}

const bool IMUCostumeDataSource::costumeLoaded(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	auto it = costumesData.begin();
	std::advance(it, idx);

	it->second.domainData.empty();

	return !(it->second.domainData.empty());
}

const unsigned int IMUCostumeDataSource::costumesLoadedCount() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	unsigned int count = 0;

	for (const auto & c : costumesData){
		if (c.second.domainData.empty() == false){
			++count;
		}
	}

	return count;
}

void IMUCostumeDataSource::loadCostume(const unsigned int idx)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (costumeLoaded(idx) == true){
		return;
	}

	innerLoadCostume(idx);
}

void IMUCostumeDataSource::unloadCostume(const unsigned int idx)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (costumeLoaded(idx) == false){
		return;
	}

	innerUnloadCostume(idx);
}

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

const core::VariantsList IMUCostumeDataSource::costumeData(const unsigned int idx) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	auto it = costumesData.begin();
	std::advance(it, idx);

	return it->second.domainData;
}

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