/**
@author Marek Daniluk
*/

#include "TransportWSDL_FTPS.h"

using namespace EDRCM;

TransportWSDL_FTPS::TransportWSDL_FTPS() {
	ftp = new FTPS_cURL();
	wsdl = new FileStoremanService();
}

TransportWSDL_FTPS::~TransportWSDL_FTPS() {
	if(ftp) {
		delete ftp;
		ftp = NULL;
	}
	if(wsdl) {
		delete wsdl;
		wsdl = NULL;
	}
}

void TransportWSDL_FTPS::setWSCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) {
	wsdl->setCredentials(uri, usr, pswd);
}

void TransportWSDL_FTPS::setFTPCredentials(const std::string& addr, const std::string& usr, const std::string& pswd) {
	ftp->setCredentials(addr, usr, pswd);
}

int TransportWSDL_FTPS::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename) {
	int value;
	std::string local_path = path;

	if(local_path.size() > 0 && local_path[local_path.size() - 1] != '/') {
		local_path.append("/");
	}
	ftp->put(local_path + filename);
	value = wsdl->storeSessionFile(sessionID, path, description, filename);
	return value;
}

int TransportWSDL_FTPS::storeSessionFiles(int sessionID, const std::string& path, const std::string& description) {
	throw EDRException("not supported yet.");
}

int TransportWSDL_FTPS::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename) {
	int value;
	ftp->put(filename);
	value = wsdl->storePerformerFile(performerID, path, description, filename);
	return value;
}

void TransportWSDL_FTPS::storePerformerFiles(int performerID, const std::string& path) {
	throw EDRException("not supported yet.");
}

int TransportWSDL_FTPS::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename) {
	int value;
	ftp->put(filename);
	value = wsdl->storeTrialFile(trialID, path, description, filename);
	return value;
}

void TransportWSDL_FTPS::storeTrialFiles(int trialID, const std::string& path) {
	throw EDRException("not supported yet.");
}

const std::string TransportWSDL_FTPS::downloadFile(int fileID, const std::string& path) {
	std::string filename;

	filename = wsdl->retrieveFile(fileID);
	ftp->get(filename);
	wsdl->downloadComplete(fileID, filename);
	filename = filename.substr(filename.rfind("/") + 1, filename.npos);
	return filename;
}
