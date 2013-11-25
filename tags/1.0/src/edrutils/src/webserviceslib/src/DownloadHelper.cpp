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

const IFtpsConnection::OperationStatus BasicDownloadHelper::get(IFtpsConnection::IProgress * progress)
{
	IFtpsConnection::OperationStatus ret = IFtpsConnection::Error;
	std::string remotePath;	

	// Tutaj może się nie powieść całe pobieranie jeśli na ftp po stornie bazy nie uda się przygotować pliku do pobrania
	// Lub usługa się wywróci
	try{
		remotePath = basicStoremanWS->retrieve(fileID).fileLocation;
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

	// Tutaj pobieranie może się nie powieść
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
		str << "Unknown error during ftp(s) file retrival. FileID = " << fileID << " RemotePath = " << remotePath <<", LocalPath = " << destinationPath;
		errorMessage_ = str.str();
	}

	// To nie wpływa zasadniczo na wynik poniewaz plik jest już u nas i ewentualnie nie mozna go posprzątać na serwerze
	try{
		basicStoremanWS->downloadComplete(fileID, remotePath);
	}catch(...){
		
	}

	return ret;
}

void BasicDownloadHelper::abort()
{
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

const IFtpsConnection::OperationStatus ShallowDownloadHelper::get(IFtpsConnection::IProgress * progress)
{
	IFtpsConnection::OperationStatus ret = IFtpsConnection::Error;
	std::string remotePath;	
	
	try{
		remotePath = (shallowFile == ShallowData ? shallowStoremanWS->getShallowCopy() : shallowStoremanWS->getMetadata());
	}catch(std::exception & e){
		errorMessage_ = e.what();
		return ret;
	}	
	catch(...){
		std::stringstream str;
		str << "Unknown error during file retrieval. File = " << (shallowFile == ShallowData ? "ShallowCopy" : "Metadata");
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
