#include "CommunicationPCH.h"
#include "DataSourceWebServicesManager.h"
#include <webserviceslib/AdministrationWS.h>
#include <webserviceslib/AuthorizationWS.h>
#include <webserviceslib/BasicQueriesWS.h>
#include <webserviceslib/BasicUpdatesWS.h>
#include <webserviceslib/FileStoremanWS.h>
#include <webserviceslib/UserPersonalSpaceWS.h>
#include <webserviceslib/AccountFactoryWS.h>

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

const MotionBasicQueriesWSPtr & DataSourceWebServicesManager::motionBasicQueriesService() const
{
	return motionBasicQueriesService_;
}

const MedicalBasicQueriesWSPtr & DataSourceWebServicesManager::medicalBasicQueriesService() const
{
	return medicalBasicQueriesService_;
}

const MotionBasicUpdatesWSPtr & DataSourceWebServicesManager::motionBasicUpdatesService() const
{
	return motionBasicUpdatesService_;
}

const MedicalBasicUpdatesWSPtr & DataSourceWebServicesManager::medicalBasicUpdatesService() const
{
	return medicalBasicUpdatesService_;
}

const MotionFileStoremanWSPtr & DataSourceWebServicesManager::motionFileStoremanService() const
{
	return motionFileStoremanService_;
}

const MedicalFileStoremanWSPtr & DataSourceWebServicesManager::medicalFileStoremanService() const
{
	return medicalFileSoremanService_;
}

const AuthorizationWSPtr & DataSourceWebServicesManager::authorizationService() const
{
	return authorizationService_;
}

const AdministrationWSPtr & DataSourceWebServicesManager::administrationService() const
{
	return administrationService_;
}

const UserPersonalSpaceWSPtr & DataSourceWebServicesManager::userPersonalSpaceService() const
{
	return userPersonalSpaceService_;
}

const MultiAccountFactoryWSPtr & DataSourceWebServicesManager::accountFactoryService() const
{
	return accountFactoryService_;
}

DataSourceWebServicesManager::DataSourceWebServicesManager()
	:	motionBasicQueriesService_(new MotionBasicQueriesWS()),
		medicalBasicQueriesService_(new MedicalBasicQueriesWS()),
		motionBasicUpdatesService_(new MotionBasicUpdatesWS()),
		medicalBasicUpdatesService_(new MedicalBasicUpdatesWS()),
		motionFileStoremanService_(new MotionFileStoremanWS()),
		medicalFileSoremanService_(new MedicalFileStoremanWS()),
		accountFactoryService_(new MultiAccountFactoryWS()),
		authorizationService_(new AuthorizationWS()),
		administrationService_(new AdministrationWS()),
		userPersonalSpaceService_(new UserPersonalSpaceWS())
{

}

DataSourceWebServicesManager::~DataSourceWebServicesManager()
{

}