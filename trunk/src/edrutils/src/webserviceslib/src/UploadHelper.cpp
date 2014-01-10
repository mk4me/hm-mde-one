#include <webserviceslib/UploadHelper.h>

using namespace webservices;


BasicUploadHelper::BasicUploadHelper() : isFile(true)
{

}

BasicUploadHelper::~BasicUploadHelper()
{

}

const IFtpsConnection::OperationStatus BasicUploadHelper::put(IFtpsConnection::IProgress * progress)
{
	IFtpsConnection::OperationStatus ret = IFtpsConnection::Error;

	// Tutaj upload mo¿e siê nie powieœæ
	try {
		ret = ftpsConnection->put(sourcePath, destinationPath, progress);
		if(ret == webservices::IFtpsConnection::Error){
			errorMessage_ = ftpsConnection->errorMessage();
		}

	}catch(std::exception & e){
		errorMessage_ = e.what();
	}	
	catch(...){
		std::stringstream str;
		str << "Unknown error during ftp(s) upload. Source = " << sourcePath << " RemotePath = " << destinationPath;
		errorMessage_ = str.str();
	}

	return ret;
}

void BasicUploadHelper::abort()
{
	ftpsConnection->abort();
}

const int BasicUploadHelper::progress()
{
	return ftpsConnection->progress();
}

const std::string BasicUploadHelper::errorMessage()
{
	return errorMessage_;
}

void BasicUploadHelper::setFileUpload(const std::string & sourcePath, const std::string & destinationPath)
{
	this->sourcePath = sourcePath;
	this->destinationPath = destinationPath;
	isFile = true;
}

void BasicUploadHelper::setFolderUpload(const std::string & sourcePath, const std::string & destinationPath)
{
	this->sourcePath = sourcePath;
	this->destinationPath = destinationPath;
	isFile = false;
}

void BasicUploadHelper::configure(FileStoremanWSPtr storeman, const FtpsConnectionPtr & ftpsConnection)
{
    this->storeman = storeman;
	this->ftpsConnection = ftpsConnection;
}