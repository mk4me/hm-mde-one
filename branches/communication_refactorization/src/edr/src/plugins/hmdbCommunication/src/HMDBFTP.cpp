#include "CommunicationPCH.h"
#include "HMDBFTP.h"
#include <networkUtils/CURLFTPHelper.h>
#include "CURLFTPManager.h"
#include "CURLFTPTransfer.h"
#include "CURLFTPProgress.h"
#include "CURLFTPTransferData.h"

using namespace hmdbCommunication;
using namespace networkUtils;

HMDBFTP::HMDBFTP(const std::string user, const std::string password,
	const std::string & url,
	utils::shared_ptr<CURLFTPManager> manager)
	: user(user), password(password), url(url), manager(manager)
{

}

HMDBFTP::~HMDBFTP()
{

}

const HMDBFTP::TransferPtr HMDBFTP::preparePut(const std::string & destinationFileName,
	std::istream * stream, unsigned long long size)
{
	HMDBFTP::TransferPtr ret;

	auto m = manager.lock();

	if (m == nullptr){
		return ret;
	}

	auto curl = curl_easy_init();
	if (curl != nullptr){

		utils::shared_ptr<CURLFTPTransferData> * td(new utils::shared_ptr<CURLFTPTransferData>(new CURLFTPTransferData));
		CURLFTPHelper::initializeFTPConnection(curl, user, password);
		CURLFTPHelper::initializeFTPUpload(curl, url + "/" + destinationFileName, stream, (*td)->progress.get());
		curl_easy_setopt(curl, CURLOPT_PRIVATE, td);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE, size);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, size);
		ret.reset(new CURLFTPTransfer(CURLFTPTransfer::Upload, curl, m, destinationFileName, size));
	}

	return ret;
}

const HMDBFTP::TransferPtr HMDBFTP::prepareGet(const std::string & destinationFileName,
	std::ostream * stream, unsigned long long size)
{
	HMDBFTP::TransferPtr ret;

	auto m = manager.lock();

	if (m == nullptr){
		return ret;
	}

	auto curl = curl_easy_init();
	if (curl != nullptr){

		utils::shared_ptr<CURLFTPTransferData> * td(new utils::shared_ptr<CURLFTPTransferData>(new CURLFTPTransferData));
		CURLFTPHelper::initializeFTPConnection(curl, user, password);
		CURLFTPHelper::initializeFTPDownload(curl, url + "/" + destinationFileName, stream, (*td)->progress.get());
		curl_easy_setopt(curl, CURLOPT_PRIVATE, td);

		ret.reset(new CURLFTPTransfer(CURLFTPTransfer::Download, curl, m, destinationFileName, size));
	}

	return ret;
}