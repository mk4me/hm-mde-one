#include <webserviceslib/DownloadHelper.h>

namespace webservices
{

BasicDownloadHelper::BasicDownloadHelper()
{

}

BasicDownloadHelper::~BasicDownloadHelper()
{

}

void BasicDownloadHelper::configure(const BasicStoremanWSPtr & basicStoremanWS, const FtpsConnectionPtr & ftpsConnection)
{
	this->basicStoremanWS = basicStoremanWS;
	this->ftpsConnection = ftpsConnection;
}

IFtpsConnection::OperationStatus BasicDownloadHelper::get(IFtpsConnection::IProgress * progress)
{
	IFtpsConnection::OperationStatus ret = IFtpsConnection::Error;
	std::string remotePath;

	webservices::IWebService::ScopedLock lock(*basicStoremanWS);
	webservices::IFtpsConnection::ScopedLock lock2(*ftpsConnection);

	// Tutaj mo¿e siê nie powieœæ ca³e pobieranie jeœli na ftp po stornie bazy nie uda siê przygotowaæ pliku do pobrania
	// Lub us³uga sie wywróci
	try{
		remotePath = basicStoremanWS->retrieve(fileID);
	}catch(std::exception & e){
		errorMessage_ = e.what();
		return ret;
	}	
	catch(...){
		std::stringstream str;
		str << "Unknown error during file retrival. FileID = " << fileID;
		errorMessage_ = str.str();
		return ret;
	}	

	// Tutaj pobieranie mo¿e siê nie powieœæ
	try {
		ret = ftpsConnection->get(remotePath, destinationPath, progress);
		if(ret == webservices::IFtpsConnection::Error){
			errorMessage_ = ftpsConnection->errorMessage();
		}
	}catch(std::exception & e){
		errorMessage_ = e.what();
	}	
	catch(...){
		std::stringstream str;
		str << "Unknown error during ftps file retrival. FileID = " << fileID << " RemotePath = " << remotePath <<", LocalPath = " << destinationPath;
		errorMessage_ = str.str();
	}

	// To nie wp³ywa zasadniczo na wynik poniewaz plik jest ju¿ u nas i ewentualnie nie mozna go posprz¹taæ na serwerze
	try{
		basicStoremanWS->downloadComplete(fileID, remotePath);
	}catch(...){
		
	}	

	return ret;
}

void BasicDownloadHelper::abort()
{
	webservices::IFtpsConnection::ScopedLock lock2(*ftpsConnection);
	ftpsConnection->abort();
}

const int BasicDownloadHelper::progress()
{
	return ftpsConnection->progress();
}

void BasicDownloadHelper::setDownload(int fileID, const std::string & destinationPath)
{
	this->fileID = fileID;
	this->destinationPath = destinationPath;
}

const std::string BasicDownloadHelper::errorMessage()
{
	return errorMessage_;
}

ShallowDownloadHelper::ShallowDownloadHelper()
{

}

ShallowDownloadHelper::~ShallowDownloadHelper()
{

}

void ShallowDownloadHelper::configure(const ShallowStoremanWSPtr & shallowStoremanWS, const FtpsConnectionPtr & ftpsConnection)
{
	this->shallowStoremanWS = shallowStoremanWS;
	this->ftpsConnection = ftpsConnection;
}

IFtpsConnection::OperationStatus ShallowDownloadHelper::get(IFtpsConnection::IProgress * progress)
{
	IFtpsConnection::OperationStatus ret = IFtpsConnection::Error;
	std::string remotePath;

	webservices::IWebService::ScopedLock lock(*shallowStoremanWS);
	webservices::IFtpsConnection::ScopedLock lock2(*ftpsConnection);
	
	try{
		remotePath = (shallowFile == ShallowData ? shallowStoremanWS->getShallowCopy() : shallowStoremanWS->getMetadata());
	}catch(std::exception & e){
		errorMessage_ = e.what();
		return ret;
	}	
	catch(...){
		std::stringstream str;
		str << "Unknown error during file retrival. File = " << (shallowFile == ShallowData ? "ShallowCopy" : "Metadata");
		errorMessage_ = str.str();
		return ret;
	}

	try {
		ret = ftpsConnection->get(remotePath, destinationPath, progress);
		if(ret == webservices::IFtpsConnection::Error){
			errorMessage_ = ftpsConnection->errorMessage();
		}
	}catch(const std::exception & e){
		errorMessage_ = e.what();
	}	
	catch(...){
		std::stringstream str;
		str << "Unknown error during ftps file retrival. RemotePath = " << remotePath <<", LocalPath = " << destinationPath;
		errorMessage_ = str.str();
	}

	try{
		shallowStoremanWS->downloadComplete(remotePath);
	}catch(...){

	}

	return ret;
}

void ShallowDownloadHelper::abort()
{
	webservices::IFtpsConnection::ScopedLock lock2(*ftpsConnection);
	ftpsConnection->abort();
}

const int ShallowDownloadHelper::progress()
{
	return ftpsConnection->progress();
}

void ShallowDownloadHelper::setDownload(ShallowFile shallowFile, const std::string & destinationPath)
{
	this->shallowFile = shallowFile;
	this->destinationPath = destinationPath;
}

const std::string ShallowDownloadHelper::errorMessage()
{
	return errorMessage_;
}

}