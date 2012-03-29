#include "CommunicationPCH.h"
#include "DataSourceWebServicesManager.h"
#include <webserviceslib/AdministrationWS.h>
#include <webserviceslib/AuthorizationWS.h>
#include <webserviceslib/BasicQueriesWS.h>
#include <webserviceslib/BasicUpdatesWS.h>
#include <webserviceslib/FileStoremanWS.h>
#include <webserviceslib/UserPersonalSpaceWS.h>

using namespace webservices;

DataSourceWebServicesManager * DataSourceWebServicesManager::instance_ = nullptr;

DataSourceWebServicesManager * DataSourceWebServicesManager::create()
{
	if(instance_ == nullptr){
		instance_ = new DataSourceWebServicesManager();
	}else{
		throw std::runtime_error("Instance already created");
	}

	return instance_;
}

void DataSourceWebServicesManager::destroy()
{
	if(instance_ != nullptr){
		delete instance_;
		instance_ = nullptr;
	}
}

DataSourceWebServicesManager * DataSourceWebServicesManager::instance()
{
	return instance_;
}

const MotionBasicQueriesWSPtr & DataSourceWebServicesManager::motionBasicQueriesService()
{
	return motionBasicQueriesService_;
}

const MedicalBasicQueriesWSPtr & DataSourceWebServicesManager::medicalBasicQueriesService()
{
	return medicalBasicQueriesService_;
}

const MotionBasicUpdatesWSPtr & DataSourceWebServicesManager::motionBasicUpdatesService()
{
	return motionBasicUpdatesService_;
}

const MedicalBasicUpdatesWSPtr & DataSourceWebServicesManager::medicalBasicUpdatesService()
{
	return medicalBasicUpdatesService_;
}

const MotionFileStoremanWSPtr & DataSourceWebServicesManager::motionFileStoremanService()
{
	return motionFileStoremanService_;
}

const MedicalFileStoremanWSPtr & DataSourceWebServicesManager::medicalFileStoremanService()
{
	return medicalFileSoremanService_;
}

const AuthorizationWSPtr & DataSourceWebServicesManager::authorizationService()
{
	return authorizationService_;
}

const AdministrationWSPtr & DataSourceWebServicesManager::administrationService()
{
	return administrationService_;
}

const UserPersonalSpaceWSPtr & DataSourceWebServicesManager::userPersonalSpaceService()
{
	return userPersonalSpaceService_;
}

DataSourceWebServicesManager::DataSourceWebServicesManager()
	:	motionBasicQueriesService_(new MotionBasicQueriesWS()),
		medicalBasicQueriesService_(new MedicalBasicQueriesWS()),
		motionBasicUpdatesService_(new MotionBasicUpdatesWS()),
		medicalBasicUpdatesService_(new MedicalBasicUpdatesWS()),
		motionFileStoremanService_(new MotionFileStoremanWS()),
		medicalFileSoremanService_(new MedicalFileStoremanWS()),
		authorizationService_(new AuthorizationWS()),
		administrationService_(new AdministrationWS()),
		userPersonalSpaceService_(new UserPersonalSpaceWS())
{

}

DataSourceWebServicesManager::~DataSourceWebServicesManager()
{

}