/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/FileStoremanService.h>

using namespace communication;

FileStoremanService::FileStoremanService() { }

FileStoremanService::~FileStoremanService() { }

void FileStoremanService::downloadComplete(int fileID, const std::string& path)
{
	this->setOperation("DownloadComplete");
	if(invoker->status()) {
		if(!invoker->setValue("fileID", toString<int>(fileID)) ||
			!invoker->setValue("path", new std::string(path))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

std::string FileStoremanService::retrieveFile(int fileID)
{
	this->setOperation("RetrieveFile");
	if(invoker->status()) {
		if(!invoker->setValue("fileID", toString<int>(fileID))) {
			throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		Schema::Type type;
		void *val = invoker->getValue("FileLocation", type);
		if(val == NULL) {
			throw std::runtime_error("Bad response in retrieveFile operation.");
		}
		return *(std::string*)val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

int FileStoremanService::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename)
{
	this->setOperation("StoreSessionFile");
	if(invoker->status()) {
		if(!invoker->setValue("sessionId", toString<int>(sessionID)) ||
			!invoker->setValue("path", new std::string(path)) ||
			!invoker->setValue("description", new std::string(description)) ||
			!invoker->setValue("filename", new std::string(filename))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		int val = invoker->getValue<int>("StoreSessionFileResult");
		return val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

int FileStoremanService::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename)
{
	this->setOperation("StorePerformerFile");
	if(invoker->status()) {
		if(!invoker->setValue("performerID", toString<int>(performerID)) ||
			!invoker->setValue("path", new std::string(path)) ||
			!invoker->setValue("description", new std::string(description)) ||
			!invoker->setValue("filename", new std::string(filename))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		int val = invoker->getValue<int>("StorePerformerFileResult");
		return val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

int FileStoremanService::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename)
{
	this->setOperation("StoreTrialFile");
	if(invoker->status()) {
		if(!invoker->setValue("trialID", toString<int>(trialID)) ||
			!invoker->setValue("path", new std::string(path)) ||
			!invoker->setValue("description", new std::string(description)) ||
			!invoker->setValue("filename", new std::string(filename))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		int val = invoker->getValue<int>("StoreTrialFileResult");
		return val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

int FileStoremanService::storeSessionFiles(int sessionID, const std::string& path, const std::string& description)
{
	this->setOperation("StoreSessionFiles");
	if(invoker->status()) {
		if(!invoker->setValue("sessionID", toString<int>(sessionID)) ||
			!invoker->setValue("path", new std::string(path)) ||
			!invoker->setValue("description", new std::string(description))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		int val = invoker->getValue<int>("StoreSessionFilesResult");
		return val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

void FileStoremanService::storePerformerFiles(int performerID, const std::string& path)
{
	this->setOperation("StorePerformerFiles");
	if(invoker->status()) {
		if(!invoker->setValue("performerID", toString<int>(performerID)) ||
			!invoker->setValue("path", new std::string(path))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

void FileStoremanService::storeTrialFiles(int trialID, const std::string& path)
{
	this->setOperation("StoreTrialFiles");
	if(invoker->status()) {
		if(!invoker->setValue("trialID", toString<int>(trialID)) ||
			!invoker->setValue("path", new std::string(path))) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

std::string FileStoremanService::getShallowCopy()
{
	this->setOperation("GetShallowCopy");
	if(invoker->status()) {
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		Schema::Type type;
		void *val = invoker->getValue("GetShallowCopyResult", type);
		return *(std::string*)val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

std::string FileStoremanService::getMetadata()
{
	this->setOperation("GetMetadata");
	if(invoker->status()) {
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		Schema::Type type;
		void *val = invoker->getValue("GetMetadataResult", type);
		return *(std::string*)val;
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}
