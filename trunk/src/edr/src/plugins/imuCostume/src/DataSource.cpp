#include "PCH.h"
#include "DataSource.h"
#include <kinematiclib/JointAnglesCollection.h>
#include <KdAR.h>
#include <BIC.h>
#include <boost/lexical_cast.hpp>
#include <kinematiclib/BvhParser.h>
#include <corelib/Filesystem.h>
#include <QtCore/QCoreApplication>
#include <corelib/PluginCommon.h>
#include <OpenThreads/Thread>
#include <corelib/IThreadPool.h>
#include <threading/IThread.h>
#include <boost/bind.hpp>
#include "IMUCostumeListWidget.h"

using namespace IMU;

IMUCostumeDataSource::IMUCostumeDataSource() : connected_(false)
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
}

bool IMUCostumeDataSource::lateInit()
{
	return true;
}

void IMUCostumeDataSource::finalize()
{

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

void IMUCostumeDataSource::getOfferedTypes(core::TypeInfoList & offeredTypes) const
{
	offeredTypes.push_back(typeid(kinematic::JointAnglesCollectionStream));
	offeredTypes.push_back(typeid(IMUsStream));
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
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	return costumesDataStatus_[idx];
}

const std::vector<IMUCostumeDataSource::CostumeDataStatus> IMUCostumeDataSource::costumesDataStatus() const
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	return costumesDataStatus_;
}

void IMUCostumeDataSource::addToUpdate(const unsigned int idx,
	const CostumeRawData & rd,
	const core::ObjectsList & ol)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(updateSynch);
	coreData[idx] = ol;
	rawData[idx] = rd;
}

void IMUCostumeDataSource::removeFromUpdate(const unsigned int idx)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(updateSynch);
	coreData.erase(idx);
	rawData.erase(idx);
}

void IMUCostumeDataSource::innerLoadCostume(const unsigned int idx)
{
	CostumeRawData rd;

	rd.skeletalDataStream.reset(new kinematic::SkeletalDataStream);

	if(costumesConfigurations[idx].skeleton != nullptr){
		rd.skeletonStream.reset(new kinematic::JointAnglesCollectionStream);
		rd.skeletonStream->setSkeletal(costumesConfigurations[idx].skeleton, rd.skeletalDataStream);
	}
	
	rd.imuDataStream.reset(new IMUsStream);

	core::ObjectsList ol;

	if(rd.skeletonStream != nullptr){
		auto ssOW = core::ObjectWrapper::create<kinematic::JointAnglesCollectionStream>();
		ssOW->set(rd.skeletonStream);
		ol.push_back(ssOW);
	}

	auto imuSOW = core::ObjectWrapper::create<IMUsStream>();
	imuSOW->set(rd.imuDataStream);
	ol.push_back(imuSOW);

	//dane do DM
	{	
		auto t = memoryDM->transaction();
	
		for(auto it = ol.begin(); it != ol.end(); ++it){
			t->addData(*it);
		}
	}

	//teraz jeszcze w¹tek do ich odœwie¿ania
	addToUpdate(idx, rd, ol);
}

void IMUCostumeDataSource::innerUnloadCostume(const unsigned int idx)
{
	auto data = coreData[idx];

	//usuwam dane z DM
	{	
		auto t = memoryDM->transaction();

		for(auto it = data.begin(); it != data.end(); ++it){
			t->removeData(*it);
		}
	}

	removeFromUpdate(idx);
}

const bool IMUCostumeDataSource::costumeLoaded(const unsigned int idx) const
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	return rawData.find(idx) != rawData.end();
}

const unsigned int IMUCostumeDataSource::costumesLoadedCount() const
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	return rawData.size();
}

void IMUCostumeDataSource::loadCostume(const unsigned int idx)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	
	if(rawData.find(idx) != rawData.end()){
		return;
	}

	innerLoadCostume(idx);
}

void IMUCostumeDataSource::unloadCostume(const unsigned int idx)
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);

	if(rawData.find(idx) == rawData.end()){
		return;
	}

	innerUnloadCostume(idx);
}

void IMUCostumeDataSource::loadAllCostumes()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);

	if(rawData.size() == costumesConfigurations.size()){
		return;
	}

	for(unsigned int i = 0; i < costumesConfigurations.size(); ++i){
		if(rawData.find(costumesConfigurations[i].id) == rawData.end()){
			innerLoadCostume(i);
		}
	}
}

void IMUCostumeDataSource::unloadAllCostumes()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);

	if(rawData.empty() == true){
		return;
	}

	auto locRawData = rawData;

	for(auto it = locRawData.begin(); it != locRawData.end(); ++it){
		innerUnloadCostume(it->first);
	}
}

void IMUCostumeDataSource::connectCostiumes()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	
	if(connected_ == true){
		return;
	}

	StartThread();

	unsigned int i = 500;

	while((!GetIsReady() || !IsReady()) && --i > 0){
		OpenThreads::Thread::microSleep(1000);
	}

	const auto modelsCount = GetModelsCount();

	if(GetIsReady() == false || IsReady() == false || modelsCount == 0){
		//EndThread();
		return;
	}

	costumesConfigurations.resize(modelsCount);

	costumesDataStatus_.resize(modelsCount);
	std::fill(costumesDataStatus_.begin(), costumesDataStatus_.end(), NODATA);

	core::Filesystem::Path p(QCoreApplication::applicationDirPath().toStdString());		

	p /= "BIC";
	p /= "UnrealStickman.bvh";

	kinematic::hAnimSkeletonPtr model;

	if(core::Filesystem::pathExists(p) == false){
		//TODO
		//message ¿e nie ma pliku z konfiguracj¹ szkieletu - nie mozna utworzyæ strumieni dla szkieletów
	}else{
		kinematic::SkeletalDataPtr tmpData(new kinematic::SkeletalData);
		kinematic::SkeletalModelPtr tmpModel(new kinematic::SkeletalModel);

		//pobieram szkielet - aktualnie na sztywno dla wszystkich kostiumów
		kinematic::BvhParser parser;
		parser.parse(tmpModel, tmpData, p.string());

		model = kinematic::hAnimSkeleton::create();
		model->doSkeletonMapping(tmpModel);
		model->createActiveHierarchy();
	}

	for(unsigned int i = 0; i < modelsCount; ++i){
		costumesConfigurations[i].id = i;
		//iloœæ IMU na sztywno 12
		costumesConfigurations[i].IMUcount = 12;
		//ile mam jointów w modelu
		costumesConfigurations[i].jointsCount = 0;
		if(model != nullptr){
			costumesConfigurations[i].jointsCount = model->getJoints().size();
		}

		costumesConfigurations[i].name = "Costume " + boost::lexical_cast<std::string>(i);
		// model wszêdzie taki sam
		costumesConfigurations[i].skeleton = model;
	}

	core::IThreadPool::Threads t;
	plugin::getThreadPool()->getThreads("IMUCostumeDataSource", t, 1);

	if(t.empty() == false){
		utils::FunctorRunnable::Functor f = boost::bind(&IMUCostumeDataSource::refreshData, this);
		refreshThread = t.front();
		utils::IRunnablePtr runnable(new utils::FunctorRunnable(f));
		refreshData_ = true;
		refreshThread->start(runnable);
	}

	connected_ = true;
}

void IMUCostumeDataSource::disconnectCostiumes()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);

	if(connected_ == false){
		return;
	}

	//zakoñcz w¹tek pobieraj¹cy dane jeœli jest taki
	if(refreshThread != nullptr){
		refreshData_ = false;
		refreshThread->join();
		refreshThread.reset();
	}
	//koñczê w¹tek odbieraj¹cy dane
	//EndThread();

	//uzupe³niam statusy
	std::fill(costumesDataStatus_.begin(), costumesDataStatus_.end(), NODATA);

	//zaznaczam ¿e roz³¹czono
	connected_ = false;
}

const bool IMUCostumeDataSource::connected() const
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);
	return connected_;
}

void IMUCostumeDataSource::refreshData()
{
	while(refreshData_ == true){
		try {
			for(auto it = rawData.begin(); it != rawData.end(); ++it){

				std::vector<IMUData> imuData(12);

				{
					utils::ScopedLock<utils::RecursiveSyncPolicy> lock(updateSynch);

					for(unsigned int i = 0; i < 12; ++i){

						auto rid = GetImuData(i+1);

						imuData[i].accelerometer = osg::Vec3(rid.acc_x, rid.acc_y, rid.acc_z);
						imuData[i].gyroscope = osg::Vec3(rid.rate_x, rid.rate_y, rid.rate_z);
						imuData[i].magnetometer = osg::Vec3(rid.mag_x, rid.mag_y, rid.mag_z);

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
				}

				rawData[it->first].imuDataStream->pushData(imuData);

				if(costumesConfigurations[it->first].jointsCount > 0){

					kinematic::StreamSkeletonDataFrame sf;

					{
						utils::ScopedLock<utils::RecursiveSyncPolicy> lock(updateSynch);
					
						sf.rootPosition = osg::Vec3(0,0,0);
						sf.frameNo = -1;
						sf.rotations.reserve(costumesConfigurations[it->first].jointsCount);

						for(unsigned int i = 0; i < costumesConfigurations[it->first].jointsCount; ++i){
							auto q = GetLocalOrientation(it->first, i);

							sf.rotations.push_back(osg::Quat(q.I, q.J, q.K, q.R));
						}
					}

					rawData[it->first].skeletalDataStream->pushData(sf);
				}
			}
		}catch(...){

		}

		OpenThreads::Thread::microSleep(1000);
	}
}