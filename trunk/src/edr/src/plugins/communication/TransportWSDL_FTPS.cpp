/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/TransportWSDL_FTPS.h>
#include <boost/bind.hpp>

using namespace communication;

TransportWSDL_FTPSBase::TransportWSDL_FTPSBase() : ftp(new FtpsConnection())
{

}

void TransportWSDL_FTPSBase::setWSCredentials(const std::string& uri, const std::string& usr, const std::string& pswd)
{
    wsdl->setCredentials(uri, usr, pswd);
}

void TransportWSDL_FTPSBase::setFTPCredentials(const std::string& addr, const std::string& usr, const std::string& pswd)
{
    ftp->setCredentials(addr, usr, pswd);
}

int TransportWSDL_FTPSBase::getProgress() const
{
    return ftp->getProgress();
}

void TransportWSDL_FTPSBase::abort()
{
    ftp->abort();
}

const std::string & TransportWSDL_FTPSBase::getLastError() const
{
    return errorMessage;
}

FtpsConnection::OperationStatus TransportWSDL_FTPSBase::getShallowCopy(const std::string & path, communication::FtpsConnection::IProgress * progress)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;
    try{
        std::string remoteFile(wsdl->getShallowCopy());
        SafeWSDL_FTPManagerBase safeEnder(boost::bind(&FileStoremanServiceBase::fileDownloadComplete, wsdl, 0, remoteFile)); 

        //sciagnij plik
        ret = ftp->get(remoteFile, path, progress);
        
        if(ret == FtpsConnection::Error){
            errorMessage = ftp->getLastError();
        }

    }catch(const std::exception & e){
        ret = FtpsConnection::Error;
        errorMessage = e.what();
    }catch(...){
        ret = FtpsConnection::Error;
        errorMessage = "Unknown error while getting shallowcopy";
    }

    return ret;
}

FtpsConnection::OperationStatus TransportWSDL_FTPSBase::getMetadata(const std::string & path, communication::FtpsConnection::IProgress * progress)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;
    try{
        std::string remoteFile(wsdl->getMetadata());
        SafeWSDL_FTPManagerBase safeEnder(boost::bind(&FileStoremanServiceBase::fileDownloadComplete, wsdl, 0, remoteFile)); 

        //sciagnij plik
        ret = ftp->get(remoteFile, path, progress);
        
        if(ret == FtpsConnection::Error){
            errorMessage = ftp->getLastError();
        }

    }catch(const std::exception & e){
        ret = FtpsConnection::Error;
        errorMessage = e.what();
    }catch(...){
        ret = FtpsConnection::Error;
        errorMessage = "Unknown error while getting metadata";
    }

    return ret;
}

FtpsConnection::OperationStatus TransportWSDL_FTPSBase::downloadFile(int fileID, const std::string& path, communication::FtpsConnection::IProgress * progress)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;
    try{
        std::string remoteFile(wsdl->retrieveFile(fileID));
        SafeWSDL_FTPManagerBase safeEnder(boost::bind(&FileStoremanServiceBase::fileDownloadComplete, wsdl, 0, remoteFile)); 

        //sciagnij plik
        ret = ftp->get(remoteFile, path, progress);
        
        if(ret == FtpsConnection::Error){
            errorMessage = ftp->getLastError();
        }

    }catch(const std::exception & e){
        ret = FtpsConnection::Error;
        errorMessage = e.what();
    }catch(...){
        ret = FtpsConnection::Error;
        errorMessage = "Unknown error while getting file";
    }

    return ret;
}

FtpsConnection::OperationStatus TransportWSDL_FTPSBase::downloadPhoto(int photoID, const std::string& path, communication::FtpsConnection::IProgress * progress)
{
    FtpsConnection::OperationStatus ret = FtpsConnection::Complete;
    try{
        std::string remoteFile(wsdl->retrievePhoto(photoID));
        SafeWSDL_FTPManagerBase safeEnder(boost::bind(&FileStoremanServiceBase::photoDownloadComplete, wsdl, 0, remoteFile)); 

        //sciagnij plik
        ret = ftp->get(remoteFile, path, progress);
        
        if(ret == FtpsConnection::Error){
            errorMessage = ftp->getLastError();
        }

    }catch(const std::exception & e){
        ret = FtpsConnection::Error;
        errorMessage = e.what();
    }catch(...){
        ret = FtpsConnection::Error;
        errorMessage = "Unknown error while getting photo";
    }

    return ret;
}

MotionTransportWSDL_FTPS::MotionTransportWSDL_FTPS()
    : motionWsdl(new MotionFileStoremanService())
{
    init(motionWsdl);
}

MotionTransportWSDL_FTPS::~MotionTransportWSDL_FTPS()
{

}

int MotionTransportWSDL_FTPS::storeSessionFile(int sessionID, const std::string& remoteDestination, const std::string& description, const std::string& localSource)
{
    int value;

    ftp->put(localSource, remoteDestination);

    core::Filesystem::Path path(remoteDestination);

    value = motionWsdl->storeSessionFile(sessionID, path.parent_path().string(), description, path.filename().string());
    return value;
}

int MotionTransportWSDL_FTPS::storeSessionFiles(int sessionID, const std::string& path, const std::string& description)
{
    throw std::runtime_error("not supported yet.");
}

int MotionTransportWSDL_FTPS::storePerformerFile(int performerID, const std::string& remoteDestination, const std::string& description, const std::string& localSource)
{
    int value;

    ftp->put(localSource, remoteDestination);

    core::Filesystem::Path path(remoteDestination);

    value = motionWsdl->storePerformerFile(performerID, path.parent_path().string(), description, path.filename().string());
    return value;
}

int MotionTransportWSDL_FTPS::storePerformerFiles(int performerID, const std::string& path)
{
    throw std::runtime_error("not supported yet.");
    return -1;
}

int MotionTransportWSDL_FTPS::storeTrialFile(int trialID, const std::string& remoteDestination, const std::string& description, const std::string& localSource)
{
    int value;
    
    ftp->put(localSource, remoteDestination);

    core::Filesystem::Path path(remoteDestination);

    value = motionWsdl->storeTrialFile(trialID, path.parent_path().string(), description, path.filename().string());
    return value;
}

int MotionTransportWSDL_FTPS::storeTrialFiles(int trialID, const std::string& path)
{
    throw std::runtime_error("not supported yet.");
    return -1;
}


MedicalTransportWSDL_FTPS::MedicalTransportWSDL_FTPS() : medicalWsdl(new MedicalFileStoremanService())
{
    init(medicalWsdl);
};
