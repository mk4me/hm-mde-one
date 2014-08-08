#include "PCH.h"
#include "DataSource.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <boost/lexical_cast.hpp>
#include <kinematiclib/BvhParser.h>
#include <corelib/Filesystem.h>
#include <QtCore/QCoreApplication>
#include <corelib/PluginCommon.h>
#include <OpenThreads/Thread>
#include <corelib/IThreadPool.h>
#include <threadingUtils/IThread.h>
#include <boost/bind.hpp>
#include "IMUCostumeListWidget.h"
#include <BIC.h>
#include <windows.h>
#include <QtCore/QCoreApplication>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include <boost/bind.hpp>
#include <plugins/newChart/Wrappers.h>
#include <iosfwd>

using namespace IMU;

IMUCostumeDataSource::IMUCostumeDataSource()
: connected_(false), refreshData_(false), memoryDM(nullptr)
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

	core::Filesystem::Path p(QCoreApplication::applicationDirPath().toStdString());

	p /= "BIC";
	p /= "BIC.cfg";

	InitializeLibrary();

	InitFromCFG(p.string().c_str());

	int i = GetModelsCount();

	if (i > 0){
		costumesConfigurations.resize(i);
		costumesDataStatus_.resize(i);

		std::fill(costumesDataStatus_.begin(), costumesDataStatus_.end(), NODATA);

		for (unsigned int j = 0; j < i; ++j){
			costumesConfigurations[j].id = j;
			costumesConfigurations[j].imusCount = 18;
			costumesConfigurations[j].jointsCount = 0;
			costumesConfigurations[j].name = "Costume " + boost::lexical_cast<std::string>(j);
		}
	}
	else{
		throw std::runtime_error("Initialization failed");
	}
}

bool IMUCostumeDataSource::lateInit()
{
	return true;
}

void IMUCostumeDataSource::finalize()
{
	disconnectCostiumes();
	DeinitializeLibrary();
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
	offeredTypes.push_back(typeid(SkeletonDataStream));
	offeredTypes.push_back(typeid(IMUStream));
	offeredTypes.push_back(typeid(Vec3Stream));
	offeredTypes.push_back(typeid(ScalarStream));
}

const unsigned int IMUCostumeDataSource::costumesCout() const
{
	return costumesConfigurations.size();
}

const IMUCostumeDataSource::CostumeConfiguration & IMUCostumeDataSource::costumeConfiguration(unsigned int idx) const
{
	return costumesConfigurations[idx];
}

const IMUCostumeDataSource::CostumeDataStatus IMUCostumeDataSource::costumeDataStatus(const unsigned int idx) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return costumesDataStatus_[idx];
}

const std::vector<IMUCostumeDataSource::CostumeDataStatus> IMUCostumeDataSource::costumesDataStatus() const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return costumesDataStatus_;
}

void IMUCostumeDataSource::addToUpdate(const unsigned int idx,
	const CostumeRawData & rd)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(updateSynch);
	rawData[idx] = rd;

	if (refreshData_ == false){
		refreshData_ = true;
		threadingUtils::FunctorRunnable::Functor f = boost::bind(&IMUCostumeDataSource::refreshData, this);
		threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(f));
		refreshThread->start(runnable);
	}
}

void IMUCostumeDataSource::removeFromUpdate(const unsigned int idx)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(updateSynch);
	coreData.erase(idx);
	rawData.erase(idx);
}

void IMUCostumeDataSource::innerLoadCostume(const unsigned int idx)
{
	CostumeRawData rd;

	rd.imuDataStreams.resize(costumesConfigurations[idx].imusCount);

	for (unsigned int i = 0; i < costumesConfigurations[idx].imusCount; ++i){
		rd.imuDataStreams[i].reset(new threadingUtils::StreamT<IMUData>);
	}

	if(costumesConfigurations[idx].jointsCount > 0){

		rd.skeletonDataStream.reset(new SkeletonDataStream);
		rd.skeletonDataStream->jointsCount = costumesConfigurations[idx].jointsCount;
		rd.skeletonDataStream->jointsStream.reset(new PointsCloudStream);
		rd.skeletonDataStream->quatStream.reset(new QuaternionStream);
		rd.skeletonDataStream->connections.resize(GetSegmentsCount(idx));

		int segCount = GetSegmentsCount(idx);
		for (int i = 0; i < segCount; ++i) {
			rd.skeletonDataStream->segmentNames[GetSegmentName(idx, i)] = i;
		}

		//konfiguruje schemat po³¹czeñ
		for (unsigned int i = 0; i < rd.skeletonDataStream->connections.size(); ++i){
			int first = -1;;
			int second = -1;

			GetSegmentRange(idx, i, first, second);

			if (first > -1 && second > -1){
				rd.skeletonDataStream->connections[i].range.first = first;
				rd.skeletonDataStream->connections[i].range.second = second;
				//normalizuje do tego co bedzie w wizualizatorze
				rd.skeletonDataStream->connections[i].length = GetSegmentLengthF(idx, i) / 100.0;
			}
		}
	}

	auto ows = generateCoreData(rd);

	{
		auto transaction = memoryDM->transaction();

		for (auto it = ows.begin(); it != ows.end(); ++it){
			transaction->addData(*it);
		}

		if (transaction->isRolledback() == false){
			tryCreateStreamItem();

			generateCostumeItem(idx, ows, streamItems);

			memoryDM->hierarchyTransaction()->updateRoot(root);

			coreData[idx] = ows;

			//teraz jeszcze w¹tek do ich odœwie¿ania
			addToUpdate(idx, rd);
		}
	}
}

void IMUCostumeDataSource::generateCostumeItem(const unsigned int idx,
	const core::VariantsList & data, core::HierarchyItemPtr parent)
{
	auto it = data.begin();

	if ((*it)->data()->getTypeInfo() == typeid(SkeletonDataStream)){
		auto sdi = utils::make_shared<core::HierarchyDataItem>(*it, QIcon(), QObject::tr("Skeleton"), QObject::tr("Skeleton data stream"));
		parent->appendChild(sdi);
		++it;
	}

	while (it != data.end()){
		++it;
	}
}

const std::string IMUCostumeDataSource::imuName(const unsigned int idx)
{
	QString ret;

	switch (idx)
	{
	case 0:
		ret = QObject::tr("Left shoulder");
		break;

	case 1:
		ret = QObject::tr("Neck");
		break;

	case 2:
		ret = QObject::tr("Head");
		break;

	case 3:
		ret = QObject::tr("Left forearm");
		break;

	case 4:
		ret = QObject::tr("Chest");
		break;

	case 5:
		ret = QObject::tr("Right shoulder");
		break;

	case 6:
		ret = QObject::tr("Left thigh");
		break;

	case 7:
		ret = QObject::tr("Right thigh");
		break;

	case 8:
		ret = QObject::tr("Sacrum");
		break;

	case 9:
		ret = QObject::tr("Right shin");
		break;

	case 10:
		ret = QObject::tr("Left shin");
		break;

	case 11:
		ret = QObject::tr("Right forearm");
		break;

	case 12:
		ret = QObject::tr("Right hand");
		break;

	case 13:
		ret = QObject::tr("Left collar");
		break;

	case 14:
		ret = QObject::tr("Left hand");
		break;

	case 15:
		ret = QObject::tr("Right foot");
		break;

	case 16:
		ret = QObject::tr("Left foot");
		break;

	case 17:
		ret = QObject::tr("Right collar");
		break;
	}

	return ret.toStdString();
}

const core::VariantsList IMUCostumeDataSource::generateCoreData(const CostumeRawData crd)
{
	core::VariantsList ret;

	if (crd.skeletonDataStream != nullptr){
		auto ow = core::Variant::create<SkeletonDataStream>();
		ow->setMetadata("core/name", QObject::tr("Skeleton Stream").toStdString());
		ow->set(crd.skeletonDataStream);
		ret.push_back(ow);
	}

	if (crd.imuDataStreams.empty() == false){
		for (unsigned int i = 0; i < crd.imuDataStreams.size(); ++i){
			auto owv = core::Variant::create<Vec3Stream>();
			Vec3StreamPtr vec3Stream(new threadingUtils::StreamAdapterT<IMUData, osg::Vec3>(crd.imuDataStreams[i],
				threadingUtils::StreamAdapterT<IMUData, osg::Vec3>::ExtractorFunction(&IMUCostumeDataSource::extractAccelerometer)));

			owv->setMetadata("core/name", QObject::tr("Accelerometer").toStdString());
			owv->set(vec3Stream);
			ret.push_back(owv);

			owv = core::Variant::create<Vec3Stream>();
			vec3Stream.reset(new threadingUtils::StreamAdapterT<IMUData, osg::Vec3>(crd.imuDataStreams[i],
				threadingUtils::StreamAdapterT<IMUData, osg::Vec3>::ExtractorFunction(&IMUCostumeDataSource::extractGyroscope)));

			owv->setMetadata("core/name", QObject::tr("Gyroscope").toStdString());
			owv->set(vec3Stream);
			ret.push_back(owv);

			owv = core::Variant::create<Vec3Stream>();
			vec3Stream.reset(new threadingUtils::StreamAdapterT<IMUData, osg::Vec3>(crd.imuDataStreams[i],
				threadingUtils::StreamAdapterT<IMUData, osg::Vec3>::ExtractorFunction(&IMUCostumeDataSource::extractMagnetometer)));

			owv->setMetadata("core/name", QObject::tr("Magnetometer").toStdString());
			owv->set(vec3Stream);
			ret.push_back(owv);
		}
	}

	return ret;
}

void IMUCostumeDataSource::extractAccelerometer(const IMUData & data, osg::Vec3 & ret)
{
	ret = data.accelerometer;
}

void IMUCostumeDataSource::extractGyroscope(const IMUData & data, osg::Vec3 & ret)
{
	ret = data.gyroscope;
}

void IMUCostumeDataSource::extractMagnetometer(const IMUData & data, osg::Vec3 & ret)
{
	ret = data.magnetometer;
}

void IMUCostumeDataSource::extractScalar(const osg::Vec3 & data, float & ret, const unsigned int idx)
{
	ret = data[idx];
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
	auto data = coreData[idx];

	//usuwam dane z DM
	{
		auto t = memoryDM->transaction();

		for (auto it = data.begin(); it != data.end(); ++it){
			t->removeData(*it);
		}
	}

	removeFromUpdate(idx);
}

const bool IMUCostumeDataSource::costumeLoaded(const unsigned int idx) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return rawData.find(idx) != rawData.end();
}

const unsigned int IMUCostumeDataSource::costumesLoadedCount() const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return rawData.size();
}

void IMUCostumeDataSource::loadCostume(const unsigned int idx)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);

	if (rawData.find(idx) != rawData.end()){
		return;
	}

	innerLoadCostume(idx);
}

void IMUCostumeDataSource::unloadCostume(const unsigned int idx)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);

	if (rawData.find(idx) == rawData.end()){
		return;
	}

	innerUnloadCostume(idx);
}

void IMUCostumeDataSource::loadAllCostumes()
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);

	if (rawData.size() == costumesConfigurations.size()){
		return;
	}

	for (unsigned int i = 0; i < costumesConfigurations.size(); ++i){
		if (rawData.find(costumesConfigurations[i].id) == rawData.end()){
			innerLoadCostume(i);
		}
	}
}

void IMUCostumeDataSource::unloadAllCostumes()
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);

	if (rawData.empty() == true){
		return;
	}

	auto locRawData = rawData;

	for (auto it = locRawData.begin(); it != locRawData.end(); ++it){
		innerUnloadCostume(it->first);
	}
}

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

void IMUCostumeDataSource::connectCostiumes()
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);

	if (connected_ == true){
		return;
	}

	unsigned int i = 1000;

	while (!GetIsReady() && --i > 0)								//Pêtla czekaj¹ca na gotowoœæ biblioteki BIC.dll
		Sleep(1);

	//! Nie uda³o sie po³¹czyæ
	if (i == 0){
		return;
	}

	core::IThreadPool::Threads t;
	plugin::getThreadPool()->getThreads("IMUCostumeDataSource", t, 1);
	refreshThread = t.front();

	connected_ = true;
}

void IMUCostumeDataSource::disconnectCostiumes()
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);

	if (connected_ == false){
		return;
	}

	//zakoñcz w¹tek pobieraj¹cy dane jeœli jest taki
	if (refreshData_ == true){
		refreshData_ = false;
		refreshThread->join();
	}

	refreshThread.reset();

	//uzupe³niam statusy
	std::fill(costumesDataStatus_.begin(), costumesDataStatus_.end(), NODATA);

	//zaznaczam ¿e roz³¹czono
	connected_ = false;
}

const bool IMUCostumeDataSource::connected() const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return connected_;
}

void IMUCostumeDataSource::refreshData()
{
	while (refreshData_ == true){
		threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(updateSynch);

		try {
			for (auto it = rawData.begin(); it != rawData.end(); ++it){
				for (unsigned int i = 0; i < costumesConfigurations[it->first].imusCount; ++i){
					auto data = GetRawData(it->first, i);

					IMUData imuData;
					imuData.accelerometer = osg::Vec3(data.acc_x, data.acc_y, data.acc_z);
					imuData.gyroscope = osg::Vec3(data.rate_x, data.rate_y, data.rate_z);
					imuData.magnetometer = osg::Vec3(data.mag_x, data.mag_y, data.mag_z);
					//imuData.orientation   = osg::Quat(data.quat_i, data.quat_j, data.quat_k, data.quat_r);

					rawData[it->first].imuDataStreams[i]->pushData(imuData);

					/*
						std::stringstream ss;

						ss << "IMU " << i <<" readings:\nacceletrometer: x = "
						<< imuData[i].accelerometer[0] << "\ty = "
						<< imuData[i].accelerometer[1] << "\tz = "
						<< imuData[i].accelerometer[2] << "\ngyroscope: x = "
						<< imuData[i].gyroscope[0] << "\ty = "
						<< imuData[i].gyroscope[1] << "\tz = "
						<< imuData[i].gyroscope[2] << "\nmagnetometer: x = "
						<< imuData[i].magnetometer[0] << "\ty = "
						<< imuData[i].magnetometer[1] << "\tz = "
						<< imuData[i].magnetometer[2];

						PLUGIN_LOG_DEBUG(ss.str());
						*/
				}

				if(costumesConfigurations[it->first].jointsCount > 0){

					std::vector<osg::Vec3> jointsPositions(costumesConfigurations[it->first].jointsCount);					

					for(unsigned int i = 0; i < costumesConfigurations[it->first].jointsCount; ++i){
						auto p = GetGlobalPositionF(it->first, i);

						//normalizuje juz do tego co jest w wizualizatorach zeby odswiezania nie meczyc tym
						jointsPositions[i] = osg::Vec3(p.x, p.y, p.z) / 100.0;
					}

					auto segCount = GetSegmentsCount(it->first);				
					std::vector<osg::Quat> globalQuats(segCount);
					for (int i = 0; i < segCount; ++i) {
						auto q = GetGlobalOrientation(it->first, i);
						globalQuats[i] = osg::Quat(q.I, q.J, q.K, q.R); 						
					}
					rawData[it->first].skeletonDataStream->jointsStream->pushData(jointsPositions);
					rawData[it->first].skeletonDataStream->quatStream->pushData(globalQuats);
				}
			}
		}
		catch (const std::exception & e){
			PLUGIN_LOG_DEBUG("Problem while getting costume data: " << e.what());
		}
		catch (...){
			PLUGIN_LOG_DEBUG("Unknown problem while getting costume data");
		}
		// 30 fps mi wystarczy
		OpenThreads::Thread::microSleep(33333);
	}
}

void IMUCostumeDataSource::callibrateFirstPass(const unsigned int idx)
{
	//Calibrate(idx);
}

void IMUCostumeDataSource::callibrateSecondPass(const unsigned int idx)
{
	//Calibrate(idx);
}

void IMUCostumeDataSource::finalizeCalibration(const unsigned int idx)
{
	if (GetJointsCount(idx) > 1 && GetSegmentsCount(idx) > 1){
		//za³adowano
		costumesConfigurations[idx].jointsCount = GetJointsCount(idx);
	}
	else{
		costumesConfigurations[idx].jointsCount = 0;
	}
}

const bool IMUCostumeDataSource::isCalibrated(const unsigned int idx) const
{
	return costumesConfigurations[idx].jointsCount > 1;
}