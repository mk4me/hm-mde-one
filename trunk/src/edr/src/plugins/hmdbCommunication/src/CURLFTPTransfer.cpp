#include "CommunicationPCH.h"
#include "CURLFTPTransfer.h"
#include "CURLFTPStatus.h"
#include "CURLFTPProgress.h"
#include "CURLFTPTransferData.h"
#include <curl/curl.h>
#include <loglib/Exceptions.h>

using namespace hmdbCommunication;

CURLFTPTransfer::CURLFTPTransfer(const Direction direction, CURL * curl,
	CURLManagerPtr manager,	const std::string & file, const size_t size)
	: file_(file), size_(size), direction_(direction), manager_(manager),
	curl_(curl)
{
	utils::shared_ptr<CURLFTPTransferData> * td = nullptr;
	auto ret = curl_easy_getinfo(curl, CURLINFO_PRIVATE, &td);
	if (ret != CURLE_OK || td == nullptr){
		throw loglib::runtime_error("Improperly configured connection");
	}

	data_ = *td;
}

CURLFTPTransfer::~CURLFTPTransfer()
{
	abort();
	wait();
	curl_easy_cleanup(curl_);
}

const CURL * CURLFTPTransfer::curl() const
{
	return curl_;
}

CURL * CURLFTPTransfer::curl()
{
	return curl_;
}

const std::string CURLFTPTransfer::file() const
{
	return file_;
}

const unsigned long long CURLFTPTransfer::processed() const
{
	return data_->progress->processedData();
}

const unsigned long long CURLFTPTransfer::size() const
{
	return std::max(size_, (size_t)processed());
}

void CURLFTPTransfer::start()
{
	ScopedLock lock(sync_);

	if (data_->status->status() != IOperation::Initialized){
		return;
	}

	auto l = manager_.lock();
	if (l == nullptr){
		data_->status->setError("Transfer manager expired");
		data_->status->setStatus(Error);
		return;
	}	
	
	data_->status->setStatus(Pending);
	data_->wait = l->addRequest(curl_);
}

void CURLFTPTransfer::abort()
{
	data_->progress->abort();
}

void CURLFTPTransfer::wait()
{
	if (status() != CURLFTPTransfer::Finished
		&& status() != CURLFTPTransfer::Error
		&& status() != CURLFTPTransfer::Aborted){
		data_->wait.wait();
	}
}

const CURLFTPTransfer::Status CURLFTPTransfer::status() const
{	
	return data_->status->status();
}

const std::string CURLFTPTransfer::error() const
{
	return data_->status->error();
}

const float CURLFTPTransfer::normalizedProgress() const
{
	return data_->progress->progress();
}

const float CURLFTPTransfer::elapsed() const
{
	double time = -1.0;
	auto ret = curl_easy_getinfo(curl_, CURLINFO_TOTAL_TIME, &time);
	return ret;
}

const float CURLFTPTransfer::remaining() const
{
	auto avs = averageSpeed();
	if (avs == 0.0){
		return -1.0;
	}

	return (size() - processed()) * avs;
}

const float CURLFTPTransfer::averageSpeed() const
{
	auto e = elapsed();
	if (e <= 0.0){
		return 0.0;
	}

	return processed() / e;
}

const CURLFTPTransfer::Direction CURLFTPTransfer::direction() const
{
	return direction_;
}
