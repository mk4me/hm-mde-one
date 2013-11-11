#include "CommunicationPCH.h"
#include "DataSourceConnectionsManager.h"

DataSourceConnectionManager * DataSourceConnectionManager::instance_ = nullptr;

DataSourceConnectionManager * DataSourceConnectionManager::create()
{
	if(instance_ == nullptr){
		instance_ = new DataSourceConnectionManager();
	}else{
		throw std::runtime_error("Instance already created");
	}

    return instance_;
}

void DataSourceConnectionManager::destroy()
{
	if(instance_ != nullptr){
		delete instance_;
	}
}

DataSourceConnectionManager * DataSourceConnectionManager::instance()
{
	return instance_;
}

DataSourceConnectionManager::DataSourceConnectionManager()
	:	motionFilesStoremanWSConnection_(new webservices::WSSecureConnection()),
		motionBasicQueriesWSConnection_(new webservices::WSSecureConnection()),
		motionBasicUpdatesWSConnection_(new webservices::WSSecureConnection()),
		medicalFilesStoremanWSConnection_(new webservices::WSSecureConnection()),
		medicalBasicQueriesWSConnection_(new webservices::WSSecureConnection()),
		medicalBasicUpdatesWSConnection_(new webservices::WSSecureConnection()),
		accountFactoryWSConnection_(new webservices::WSSecureConnection()),
		userPersonalSpaceWSConnection_(new webservices::WSSecureConnection()),
		administrationWSConnection_(new webservices::WSSecureConnection()),
		authorizationWSConnection_(new webservices::WSSecureConnection()),
		motionFtps_(new webservices::FtpsConnection()),
		medicalFtps_(new webservices::FtpsConnection())
{
	
}

DataSourceConnectionManager::~DataSourceConnectionManager()
{

}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::motionFileStoremanWSConnection() const
{
	return motionFilesStoremanWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::motionBasicQueriesWSConnection() const
{
	return motionBasicQueriesWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::motionBasicUpdatesWSConnection() const
{
	return motionBasicUpdatesWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::medicalFileStoremanWSConnection() const
{
	return medicalFilesStoremanWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::medicalBasicQueriesWSConnection() const
{
	return medicalBasicQueriesWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::medicalBasicUpdatesWSConnection() const
{
	return medicalBasicUpdatesWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::accountFactoryWSConnection() const
{
	return accountFactoryWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::userPersonalSpaceWSConnection() const
{
	return userPersonalSpaceWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::administrationWSConnection() const
{
	return administrationWSConnection_;
}

const webservices::WSSecureConnectionPtr & DataSourceConnectionManager::authorizationWSConnection() const
{
	return authorizationWSConnection_;
}

const webservices::FtpsConnectionPtr & DataSourceConnectionManager::motionFtps() const
{
	return motionFtps_;
}

const webservices::FtpsConnectionPtr & DataSourceConnectionManager::medicalFtps() const
{
	return medicalFtps_;
}
