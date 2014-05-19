#include "CommunicationPCH.h"
#include "DataSourceConnectionsManager.h"

DataSourceConnectionManager * DataSourceConnectionManager::instance_ = nullptr;

DataSourceConnectionManager * DataSourceConnectionManager::create()
{
	if (instance_ == nullptr){
		instance_ = new DataSourceConnectionManager();
	}
	else{
		throw std::runtime_error("Instance already created");
	}

	return instance_;
}

void DataSourceConnectionManager::destroy()
{
	if (instance_ != nullptr){
		delete instance_;
	}
}

DataSourceConnectionManager * DataSourceConnectionManager::instance()
{
	return instance_;
}

DataSourceConnectionManager::DataSourceConnectionManager()
: motionFilesStoremanWSConnection_(new hmdbServices::WSSecureConnection()),
motionBasicQueriesWSConnection_(new hmdbServices::WSSecureConnection()),
motionBasicUpdatesWSConnection_(new hmdbServices::WSSecureConnection()),
medicalFilesStoremanWSConnection_(new hmdbServices::WSSecureConnection()),
medicalBasicQueriesWSConnection_(new hmdbServices::WSSecureConnection()),
medicalBasicUpdatesWSConnection_(new hmdbServices::WSSecureConnection()),
accountFactoryWSConnection_(new hmdbServices::WSSecureConnection()),
userPersonalSpaceWSConnection_(new hmdbServices::WSSecureConnection()),
administrationWSConnection_(new hmdbServices::WSSecureConnection()),
authorizationWSConnection_(new hmdbServices::WSSecureConnection()),
motionFtps_(new hmdbServices::FtpsConnection()),
medicalFtps_(new hmdbServices::FtpsConnection())
{
}

DataSourceConnectionManager::~DataSourceConnectionManager()
{
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::motionFileStoremanWSConnection() const
{
	return motionFilesStoremanWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::motionBasicQueriesWSConnection() const
{
	return motionBasicQueriesWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::motionBasicUpdatesWSConnection() const
{
	return motionBasicUpdatesWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::medicalFileStoremanWSConnection() const
{
	return medicalFilesStoremanWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::medicalBasicQueriesWSConnection() const
{
	return medicalBasicQueriesWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::medicalBasicUpdatesWSConnection() const
{
	return medicalBasicUpdatesWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::accountFactoryWSConnection() const
{
	return accountFactoryWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::userPersonalSpaceWSConnection() const
{
	return userPersonalSpaceWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::administrationWSConnection() const
{
	return administrationWSConnection_;
}

const hmdbServices::WSSecureConnectionPtr & DataSourceConnectionManager::authorizationWSConnection() const
{
	return authorizationWSConnection_;
}

const hmdbServices::FtpsConnectionPtr & DataSourceConnectionManager::motionFtps() const
{
	return motionFtps_;
}

const hmdbServices::FtpsConnectionPtr & DataSourceConnectionManager::medicalFtps() const
{
	return medicalFtps_;
}