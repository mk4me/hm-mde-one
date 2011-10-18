/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/TransportWSDL_FTPS.h>

using namespace communication;

TransportWSDL_FTPS::TransportWSDL_FTPS()
    : ftp(new FtpsConnection()), wsdl(new FileStoremanService()), aborted(false),
    filesToDownload(0), downloadedFiles(0)
{
    
}

TransportWSDL_FTPS::~TransportWSDL_FTPS()
{

}

void TransportWSDL_FTPS::setWSCredentials(const std::string& uri, const std::string& usr, const std::string& pswd)
{
    wsdl->setCredentials(uri, usr, pswd);
}

void TransportWSDL_FTPS::setFTPCredentials(const std::string& addr, const std::string& usr, const std::string& pswd)
{
    ftp->setCredentials(addr, usr, pswd);
}

int TransportWSDL_FTPS::storeSessionFile(int sessionID, const std::string& remoteDestination, const std::string& description, const std::string& localSource)
{
    int value;

    ftp->put(localSource, remoteDestination);

    core::Filesystem::Path path(remoteDestination);

    value = wsdl->storeSessionFile(sessionID, path.parent_path().string(), description, path.filename().string());
    return value;
}

int TransportWSDL_FTPS::storeSessionFiles(int sessionID, const std::string& path, const std::string& description)
{
    throw std::runtime_error("not supported yet.");
}

int TransportWSDL_FTPS::storePerformerFile(int performerID, const std::string& remoteDestination, const std::string& description, const std::string& localSource)
{
    int value;

    ftp->put(localSource, remoteDestination);

    core::Filesystem::Path path(remoteDestination);

    value = wsdl->storePerformerFile(performerID, path.parent_path().string(), description, path.filename().string());
    return value;
}

void TransportWSDL_FTPS::storePerformerFiles(int performerID, const std::string& path)
{
    throw std::runtime_error("not supported yet.");
}

int TransportWSDL_FTPS::storeTrialFile(int trialID, const std::string& remoteDestination, const std::string& description, const std::string& localSource)
{
    int value;
    
    ftp->put(localSource, remoteDestination);

    core::Filesystem::Path path(remoteDestination);

    value = wsdl->storeTrialFile(trialID, path.parent_path().string(), description, path.filename().string());
    return value;
}

void TransportWSDL_FTPS::storeTrialFiles(int trialID, const std::string& path)
{
    throw std::runtime_error("not supported yet.");
}

void TransportWSDL_FTPS::downloadFile(int fileID, const std::string& path)
{
    SafeWSDL_FTPManager safeEnder(fileID, wsdl, wsdl->retrieveFile(fileID)); 

    //sciagnij plik -> sciezka FTP, lokalna sciezka
    //std::string localPath((core::Filesystem::Path(path) / core::Filesystem::Path(safeEnder.getFtpFilePath()).filename()).string());
    ftp->get(safeEnder.getFtpFilePath(), path);  
}

int TransportWSDL_FTPS::getProgress() const
{
    return ftp->getProgress();
}

void TransportWSDL_FTPS::abort()
{
    aborted = true;
    ftp->abort();
}

void TransportWSDL_FTPS::getShallowCopy(const std::string & path)
{
    SafeWSDL_FTPManager safeEnder(0, wsdl, wsdl->getShallowCopy()); 

    //sciagnij plik
    ftp->get(safeEnder.getFtpFilePath(), path);
}

void TransportWSDL_FTPS::getMetadata(const std::string & path)
{
    SafeWSDL_FTPManager safeEnder(0, wsdl, wsdl->getMetadata()); 

    //sciagnij plik
    ftp->get(safeEnder.getFtpFilePath(), path);
}
