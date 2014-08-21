#include "CommunicationPCH.h"
#include "CURLFTPManager.h"
#include "CURLFTPTransferData.h"

using namespace hmdbCommunication;

utils::shared_ptr<CURLFTPTransferData> * getRawTransferData(CURL * curl)
{
	utils::shared_ptr<CURLFTPTransferData> * td = nullptr;
	auto ret = curl_easy_getinfo(curl, CURLINFO_PRIVATE, &td);
	if (ret == CURLE_OK){
		return td;
	}

	return nullptr;
}

CURLFTPTransferData * getTransferData(CURL * curl)
{
	auto rtd = getRawTransferData(curl);
	if (rtd != nullptr){
		return rtd->get();
	}

	return nullptr;
}

CURLFTPManager::CURLFTPManager() : networkUtils::CURLManager()
{

}

CURLFTPManager::CURLFTPManager(CURLM * multi) : networkUtils::CURLManager(multi)
{

}

CURLFTPManager::~CURLFTPManager()
{

}

void CURLFTPManager::onAddRequest(CURL * curl)
{
	auto t = getTransferData(curl);
	t->status->setStatus(threadingUtils::IOperation::Running);
}

void CURLFTPManager::onRemoveRequest(CURL * curl)
{
	auto t = getTransferData(curl);
	if (t->status->status() == threadingUtils::IOperation::Running){
		t->status->setStatus(threadingUtils::IOperation::Aborted);		
	}

	auto rt = getRawTransferData(curl);
	curl_easy_setopt(curl, CURLOPT_PRIVATE, nullptr);
	delete rt;
}

void CURLFTPManager::onCancelRequest(CURL * curl)
{
	auto t = getTransferData(curl);	
	t->status->setStatus(threadingUtils::IOperation::Aborted);	
}

void CURLFTPManager::onErrorRequest(CURL * curl, const std::string & error)
{
	auto t = getTransferData(curl);	
	t->status->setError(error);
	t->status->setStatus(threadingUtils::IOperation::Error);	
}

void CURLFTPManager::onFinishRequest(CURL * curl)
{
	auto t = getTransferData(curl);	
	t->status->setStatus(threadingUtils::IOperation::Finished);		
}