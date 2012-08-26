#include <webserviceslib/FileStoremanWS.h>
#include <webserviceslib/Entity.h>
#include <webserviceslib/DateTimeUtils.h>
#include <time.h>

namespace webservices
{

SimpleFileStoremanWS::SimpleFileStoremanWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

SimpleFileStoremanWS::~SimpleFileStoremanWS()
{

}

void SimpleFileStoremanWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & SimpleFileStoremanWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & SimpleFileStoremanWS::connection() const
{
	return constConnection_;
}

void SimpleFileStoremanWS::downloadComplete(int id, const std::string & path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("DownloadComplete");
	connection_->setValue("fileID", id);
	connection_->setValue("path", path);
	connection_->invoke();
}

const std::string SimpleFileStoremanWS::retrieve(int id)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("RetrieveFile");
	connection_->setValue("fileID", id);
	connection_->invoke(true);

	std::string ret;
	connection_->getValue("FileLocation", ret);

	return ret;
}


PhotoStoremanWS::PhotoStoremanWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

PhotoStoremanWS::~PhotoStoremanWS()
{

}

void PhotoStoremanWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & PhotoStoremanWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & PhotoStoremanWS::connection() const
{
	return constConnection_;
}

void PhotoStoremanWS::downloadComplete(int id, const std::string & path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("DownloadComplete");
	connection_->setValue("photoID", id);
	connection_->setValue("path", path);
	connection_->invoke();
}

const std::string PhotoStoremanWS::retrieve(int id)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("RetrievePhoto");
	connection_->setValue("photoID", id);
	connection_->invoke(true);

	std::string ret;
	connection_->getValue("FileLocation", ret);

	return ret;
}


ShallowStoremanWS::ShallowStoremanWS(const WSConnectionPtr & connection) : connection_(connection),
	constConnection_(connection), fileStoremanWS(connection)
{
	mutex = this;
}

ShallowStoremanWS::~ShallowStoremanWS()
{

}

void ShallowStoremanWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
	fileStoremanWS.setConnection(connection);
}

const WSConnectionPtr & ShallowStoremanWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & ShallowStoremanWS::connection() const
{
	return constConnection_;
}

const std::string ShallowStoremanWS::getShallowCopy()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("GetShallowCopy");
	connection_->invoke(true);
	std::string ret;
	connection_->getValue("GetShallowCopyResult", ret);
	return ret;
}

const std::string ShallowStoremanWS::getMetadata()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("GetMetadata");
	connection_->invoke(true);
	std::string ret;
	connection_->getValue("GetMetadataResult", ret);
	return ret;
}

void ShallowStoremanWS::downloadComplete(const std::string & path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	fileStoremanWS.downloadComplete(0, path);
}

MotionFileStoremanWS::MotionFileStoremanWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection),
	simpleFileStoremanWS_(connection), shallowStoremanWS_(connection)
{
	mutex = this;
}

MotionFileStoremanWS::~MotionFileStoremanWS()
{

}

void MotionFileStoremanWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
	simpleFileStoremanWS_.setConnection(connection);
	shallowStoremanWS_.setConnection(connection);
}

const WSConnectionPtr & MotionFileStoremanWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & MotionFileStoremanWS::connection() const
{
	return constConnection_;
}

void MotionFileStoremanWS::downloadComplete(int fileID, const std::string& path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
    simpleFileStoremanWS_.downloadComplete(fileID, path);
}

void MotionFileStoremanWS::downloadComplete(const std::string & path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	shallowStoremanWS_.downloadComplete(path);
}

const std::string MotionFileStoremanWS::retrieve(int fileID)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
    return simpleFileStoremanWS_.retrieve(fileID);
}

const std::string MotionFileStoremanWS::getShallowCopy()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
    return shallowStoremanWS_.getShallowCopy();
}

const std::string MotionFileStoremanWS::getShallowCopyIncrement(const DateTime & dateTime)
{
	struct tm time;
	time.tm_year = dateTime.getYear();
	time.tm_mon = dateTime.getMonth() - 1;
	time.tm_mday = dateTime.getDay();
	time.tm_hour = dateTime.getHour();
	time.tm_min = dateTime.getMinutes();
	time.tm_sec = dateTime.getSeconds();

	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
    connection_->setOperation("GetShallowCopyIncrement");

    connection_->setValue("since", toString(time));
    connection_->invoke(true);
    std::string ret;
    connection_->getValue("GetShallowCopyIncrementResult", ret);
    return ret;
}
		
const std::string MotionFileStoremanWS::getMetadata()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
    return shallowStoremanWS_.getMetadata();
}

const int MotionFileStoremanWS::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("StorePerformerFile");
	connection_->setValue("performerID",performerID);
	connection_->setValue("path", path);
	connection_->setValue("description", description);
	connection_->setValue("filename", filename);
	connection_->invoke(true);
	
	int val = -1;
	connection_->getValue("StorePerformerFileResult", val);
	return val;
}

void MotionFileStoremanWS::storePerformerFiles(int performerID, const std::string& path)
{
	
}

const int MotionFileStoremanWS::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("StoreSessionFile");
	connection_->setValue("sessionId", sessionID);
	connection_->setValue("path", path);
	connection_->setValue("description", description);
	connection_->setValue("filename", filename);
	connection_->invoke(true);
		
	int val = -1;
	
	connection_->getValue("StoreSessionFileResult", val);

	return val;
}

const int MotionFileStoremanWS::storeSessionFiles(int sessionID, const std::string& path, const std::string& description)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("StoreSessionFiles");
	connection_->setValue("sessionID", sessionID);
	connection_->setValue("path", path);
	connection_->setValue("description", description);
	connection_->invoke(true);
		
	int val = -1;
	connection_->getValue("StoreSessionFilesResult", val);
	return val;
}

const int MotionFileStoremanWS::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("StoreTrialFile");
	connection_->setValue("trialID", trialID);
	connection_->setValue("path", path);
	connection_->setValue("description", description);
	connection_->setValue("filename", filename);
	connection_->invoke(true);

	int val = -1;
	connection_->getValue("StoreTrialFileResult", val);
	return val;
	
}

void MotionFileStoremanWS::storeTrialFiles(int trialID, const std::string& path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("StoreTrialFiles");
	connection_->setValue("trialID", trialID);
	connection_->setValue("path", path);
	connection_->invoke();
}



//
//void MotionFileStoremanService::storePerformerFiles(int performerID, const std::string& path)
//{
//	this->setOperation("StorePerformerFiles");
//	if(invoker->status()) {
//		if(!invoker->setValue("performerID", toString<int>(performerID)) ||
//			!invoker->setValue("path", path/*new std::string(path)*/)) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker->invoke()) {
//			throw std::runtime_error(invoker->errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker->errors().c_str());
//	}
//}
//

MedicalFileStoremanWS::MedicalFileStoremanWS(const WSConnectionPtr & connection)
	: connection_(connection), constConnection_(connection),
	photoStoremanWS_(connection), shallowStoremanWS_(connection)
{
	mutex = this;
}

MedicalFileStoremanWS::~MedicalFileStoremanWS()
{

}

void MedicalFileStoremanWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
	photoStoremanWS_.setConnection(connection);
	shallowStoremanWS_.setConnection(connection);
}

const WSConnectionPtr & MedicalFileStoremanWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & MedicalFileStoremanWS::connection() const
{
	return constConnection_;
}

void MedicalFileStoremanWS::downloadComplete(int fileID, const std::string& path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	photoStoremanWS_.downloadComplete(fileID, path);
}

void MedicalFileStoremanWS::downloadComplete(const std::string & path)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	shallowStoremanWS_.downloadComplete(path);
}

const std::string MedicalFileStoremanWS::retrieve(int fileID)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return photoStoremanWS_.retrieve(fileID);
}

const std::string MedicalFileStoremanWS::getShallowCopy()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return shallowStoremanWS_.getShallowCopy();
}

const std::string MedicalFileStoremanWS::getMetadata()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return shallowStoremanWS_.getMetadata();
}

}
