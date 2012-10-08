/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/FileStoremanService.h>

using namespace communication;

void FileStoremanServiceBase::fileDownloadComplete(int fileID, const std::string& path)
{
    this->setOperation("DownloadComplete");
    if(invoker->status()) {
        if(!invoker->setValue("fileID", toString<int>(fileID)) ||
            !invoker->setValue("path", path)) {
                throw std::runtime_error("Bad operation arguments.");
        }
        if(!invoker->invoke()) {
            throw std::runtime_error(invoker->errors().c_str());
        }
    } else {
        throw std::runtime_error(invoker->errors().c_str());
    }
}

std::string FileStoremanServiceBase::retrieveFile(int fileID)
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

void FileStoremanServiceBase::photoDownloadComplete(int photoID, const std::string& path)
{
    this->setOperation("DownloadComplete");
    if(invoker->status()) {
        if(!invoker->setValue("photoID", toString<int>(photoID)) ||
            !invoker->setValue("path", path)) {
                throw std::runtime_error("Bad operation arguments.");
        }
        if(!invoker->invoke()) {
            throw std::runtime_error(invoker->errors().c_str());
        }
    } else {
        throw std::runtime_error(invoker->errors().c_str());
    }
}

std::string FileStoremanServiceBase::retrievePhoto(int photoID)
{
    this->setOperation("RetrievePhoto");
    if(invoker->status()) {
        if(!invoker->setValue("photoID", toString<int>(photoID))) {
            throw std::runtime_error("Bad operation arguments.");
        }
        if(!invoker->invoke()) {
            throw std::runtime_error(invoker->errors().c_str());
        }
        Schema::Type type;
        void *val = invoker->getValue("FileLocation", type);
        if(val == NULL) {
            throw std::runtime_error("Bad response in RetrievePhoto operation.");
        }
        return *(std::string*)val;
    } else {
        throw std::runtime_error(invoker->errors().c_str());
    }
}

std::string FileStoremanServiceBase::getShallowCopy()
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

std::string FileStoremanServiceBase::getMetadata()
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

int MotionFileStoremanService::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename)
{
	this->setOperation("StoreSessionFile");
	if(invoker->status()) {
		if(!invoker->setValue("sessionId", toString<int>(sessionID)) ||
			!invoker->setValue("path", path/*new std::string(path)*/) ||
			!invoker->setValue("description", description/*new std::string(description)*/) ||
			!invoker->setValue("filename", filename/*new std::string(filename)*/)) {
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

int MotionFileStoremanService::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename)
{
	this->setOperation("StorePerformerFile");
	if(invoker->status()) {
		if(!invoker->setValue("performerID", toString<int>(performerID)) ||
			!invoker->setValue("path", path/*new std::string(path)*/) ||
			!invoker->setValue("description", description/*new std::string(description)*/) ||
			!invoker->setValue("filename", filename/*new std::string(filename)*/)) {
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

int MotionFileStoremanService::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename)
{
	this->setOperation("StoreTrialFile");
	if(invoker->status()) {
		if(!invoker->setValue("trialID", toString<int>(trialID)) ||
			!invoker->setValue("path", path/*new std::string(path)*/) ||
			!invoker->setValue("description", description/*new std::string(description)*/) ||
			!invoker->setValue("filename", filename/*new std::string(filename)*/)) {
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

int MotionFileStoremanService::storeSessionFiles(int sessionID, const std::string& path, const std::string& description)
{
	this->setOperation("StoreSessionFiles");
	if(invoker->status()) {
		if(!invoker->setValue("sessionID", toString<int>(sessionID)) ||
			!invoker->setValue("path", path/*new std::string(path)*/) ||
			!invoker->setValue("description", description/*new std::string(description)*/)) {
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

void MotionFileStoremanService::storePerformerFiles(int performerID, const std::string& path)
{
	this->setOperation("StorePerformerFiles");
	if(invoker->status()) {
		if(!invoker->setValue("performerID", toString<int>(performerID)) ||
			!invoker->setValue("path", path/*new std::string(path)*/)) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}

void MotionFileStoremanService::storeTrialFiles(int trialID, const std::string& path)
{
	this->setOperation("StoreTrialFiles");
	if(invoker->status()) {
		if(!invoker->setValue("trialID", toString<int>(trialID)) ||
			!invoker->setValue("path", path/*new std::string(path)*/)) {
				throw std::runtime_error("Bad operation arguments.");
		}
		if(!invoker->invoke()) {
			throw std::runtime_error(invoker->errors().c_str());
		}
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
}
