/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/TransportWSDL_FTPS.h>

using namespace communication;

TransportWSDL_FTPS::TransportWSDL_FTPS()
{
    ftp = core::shared_ptr<FtpsConnection>(new FtpsConnection());
    wsdl = core::shared_ptr<FileStoremanService>(new FileStoremanService());
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

int TransportWSDL_FTPS::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename)
{
    int value;
    std::string local_path = path;

    if(local_path.size() > 0 && local_path[local_path.size() - 1] != '/') {
        local_path.append("/");
    }
    ftp->put(local_path + filename);
    value = wsdl->storeSessionFile(sessionID, path, description, filename);
    return value;
}

int TransportWSDL_FTPS::storeSessionFiles(int sessionID, const std::string& path, const std::string& description)
{
    throw std::runtime_error("not supported yet.");
}

int TransportWSDL_FTPS::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename)
{
    int value;
    ftp->put(filename);
    value = wsdl->storePerformerFile(performerID, path, description, filename);
    return value;
}

void TransportWSDL_FTPS::storePerformerFiles(int performerID, const std::string& path)
{
    throw std::runtime_error("not supported yet.");
}

int TransportWSDL_FTPS::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename)
{
    int value;
    ftp->put(filename);
    value = wsdl->storeTrialFile(trialID, path, description, filename);
    return value;
}

void TransportWSDL_FTPS::storeTrialFiles(int trialID, const std::string& path)
{
    throw std::runtime_error("not supported yet.");
}

const std::string TransportWSDL_FTPS::downloadFile(int fileID, const std::string& path)
{
    std::string filename;

    //sciagnij plik
    filename = wsdl->retrieveFile(fileID);
    ftp->get(filename);
    wsdl->downloadComplete(fileID, filename);
    filename = filename.substr(filename.rfind("/") + 1, filename.npos);

    //utworz foldery do odpowiedniej sciezki i przerzuc tam plik
    std::string filePath = path;
    filePath.append(filename.substr(0, filename.find(".")));
    try {
        core::Filesystem::createDirectory(filePath);
        core::Filesystem::move(filename, filePath.append("/").append(filename));
    } catch(std::exception& e) {
        if(!filePath.empty()) {
            core::Filesystem::deleteDirectory(filePath);
        }
        if(!filename.empty()) {
            core::Filesystem::deleteFile(filename);
        }
        throw std::runtime_error(e.what());
    }
    return filename;
}

int TransportWSDL_FTPS::getProgress() const
{
    return ftp->getProgress();
}

void TransportWSDL_FTPS::abort()
{
    ftp->abort();
}

const std::string TransportWSDL_FTPS::getShallowCopy()
{
    boost::filesystem::path filename, schema = "data/db/schema/shallowcopy.xml";

    //sciagnij plik
    filename = wsdl->getShallowCopy();
    ftp->get(filename.string());
    wsdl->downloadComplete(0, filename.string());
    core::Filesystem::createDirectory("data/db/schema");
    //usuwamy wczesniejsza wersje pliku
    core::Filesystem::deleteFile(schema.string());
    core::Filesystem::move(filename.filename().string(), schema.string());
    return schema.string();
}

const std::string TransportWSDL_FTPS::getMetadata()
{
    boost::filesystem::path filename, schema = "data/db/schema/metadata.xml";

    //sciagnij plik
    filename = wsdl->getMetadata();
    ftp->get(filename.string());
    wsdl->downloadComplete(0, filename.string());
    core::Filesystem::createDirectory("data/db/schema");
    //usuwamy wczesniejsza wersje pliku
    core::Filesystem::deleteFile(schema.string());
    core::Filesystem::move(filename.filename().string(), schema.string());
    return schema.string();
}
