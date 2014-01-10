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
#include <threading/IThread.h>
#include <boost/bind.hpp>
#include "IMUCostumeListWidget.h"

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

	if(refreshData_ == false){
		refreshData_ = true;
		utils::FunctorRunnable::Functor f = boost::bind(&IMUCostumeDataSource::refreshData, this);
		utils::IRunnablePtr runnable(new utils::FunctorRunnable(f));
		refreshThread->start(runnable);
	}
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

	core::shared_ptr<imuCostume::Costume> costume;
	try{
		costume.reset(new imuCostume::Costume);
	}catch(...){
		return;
	}

	costumesConfigurations.resize(1);

	costumesDataStatus_.resize(1);
	std::fill(costumesDataStatus_.begin(), costumesDataStatus_.end(), NODATA);

	costumesConfigurations[0].costume = costume;

	core::Filesystem::Path p(QCoreApplication::applicationDirPath().toStdString());		

	p /= "BIC";
	p /= "UnrealStickman.bvh";

	kinematic::hAnimSkeletonPtr model;

	if(core::Filesystem::pathExists(p) == true){
		kinematic::SkeletalDataPtr tmpData(new kinematic::SkeletalData);
		kinematic::SkeletalModelPtr tmpModel(new kinematic::SkeletalModel);

		//pobieram szkielet - aktualnie na sztywno dla wszystkich kostiumów
		kinematic::BvhParser parser;
		parser.parse(tmpModel, tmpData, p.string());

		//TODO
		//model = kinematic::hAnimSkeleton::create();
		//model->doSkeletonMapping(tmpModel);
		//model->createActiveHierarchy();
	}

	costumesConfigurations[0].id = 0;
	costumesConfigurations[0].name = "Costume " + boost::lexical_cast<std::string>(0);
	// model wszêdzie taki sam
	costumesConfigurations[0].skeleton = model;	

	core::IThreadPool::Threads t;
	plugin::getThreadPool()->getThreads("IMUCostumeDataSource", t, 1);
	refreshThread = t.front();	

	connected_ = true;
}


void IMUCostumeDataSource::disconnectCostiumes()
{
	utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch);

	if(connected_ == false){
		return;
	}

	//zakoñcz w¹tek pobieraj¹cy dane jeœli jest taki
	if(refreshData_ == true){
		refreshData_ = false;
		refreshThread->join();
		refreshThread.reset();
	}	

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

				std::vector<IMU::IMUData> imuData(costumesConfigurations[it->first].costume->imusNumber());

				{
					utils::ScopedLock<utils::RecursiveSyncPolicy> lock(updateSynch);

					costumesConfigurations[it->first].costume->readPacket();

					auto packet = costumesConfigurations[it->first].costume->costumePacket();

					for(unsigned int i = 0; i < imuData.size(); ++i){						

						if(packet.status[i] == imuCostume::Costume::DATA){

							imuData[i].accelerometer = packet.data[i].accelerometer;
							imuData[i].gyroscope = packet.data[i].gyroscope;
							imuData[i].magnetometer = packet.data[i].magnetometer;
							imuData[i].orientation = packet.data[i].orientation;
						
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
						}else{
							PLUGIN_LOG_DEBUG("No data for costume " << it->first << " from IMU " << i);
						}
					}
				}

				rawData[it->first].imuDataStream->pushData(imuData);

				/*if(costumesConfigurations[it->first].jointsCount > 0){

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
				}*/
			}
		}catch(...){

		}

		OpenThreads::Thread::microSleep(1000);
	}
}