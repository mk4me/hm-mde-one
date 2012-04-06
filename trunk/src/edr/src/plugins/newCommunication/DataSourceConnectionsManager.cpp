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
	:	motionFilesStoremanWSConnection_(new WSConnection()),
		motionBasicQueriesWSConnection_(new WSConnection()),
		motionBasicUpdatesWSConnection_(new WSConnection()),
		medicalFilesStoremanWSConnection_(new WSConnection()),
		medicalBasicQueriesWSConnection_(new WSConnection()),
		medicalBasicUpdatesWSConnection_(new WSConnection()),
		userPersonalSpaceWSConnection_(new WSConnection()),
		administrationWSConnection_(new WSConnection()),
		authorizationWSConnection_(new WSConnection()),
		motionFtps_(new webservices::FtpsConnection()),
		medicalFtps_(new webservices::FtpsConnection()),
		accountFactoryWSConnection_(new WSConnection())
{
	
}

DataSourceConnectionManager::~DataSourceConnectionManager()
{

}

const WSConnectionPtr & DataSourceConnectionManager::motionFileStoremanWSConnection() const
{
	return motionFilesStoremanWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::motionBasicQueriesWSConnection() const
{
	return motionBasicQueriesWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::motionBasicUpdatesWSConnection() const
{
	return motionBasicUpdatesWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::medicalFileStoremanWSConnection() const
{
	return medicalFilesStoremanWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::medicalBasicQueriesWSConnection() const
{
	return medicalBasicQueriesWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::medicalBasicUpdatesWSConnection() const
{
	return medicalBasicUpdatesWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::accountFactoryWSConnection() const
{
	return accountFactoryWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::userPersonalSpaceWSConnection() const
{
	return userPersonalSpaceWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::administrationWSConnection() const
{
	return administrationWSConnection_;
}

const WSConnectionPtr & DataSourceConnectionManager::authorizationWSConnection() const
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