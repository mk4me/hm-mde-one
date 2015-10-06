#include "CommunicationPCH.h"
#include "CURLFTPTransfer.h"
#include "CURLFTPStatus.h"
#include "CURLFTPProgress.h"
#include "CURLFTPTransferData.h"
#include <curl/curl.h>
#include <loglib/Exceptions.h>

using namespace hmdbCommunication;

CURLFTPTransfer::CURLFTPTransfer(const Direction direction,
	CURLFTPManagerPtr manager, utils::shared_ptr<CURLFTPTransferData> data,
	const std::string & file, const size_t size)
	: file_(file), size_(size), data_(data), direction_(direction),
	manager_(manager)
{

}

CURLFTPTransfer::~CURLFTPTransfer()
{
	abort();
	wait();
}

const std::string CURLFTPTransfer::file() const
{
	return file_;
}

const unsigned long long CURLFTPTransfer::processed() const
{
	return data_->progress.processedData();
}

const unsigned long long CURLFTPTransfer::size() const
{
	return std::max(size_, (size_t)processed());
}

void CURLFTPTransfer::start()
{
	ScopedLock lock(sync_);

	if (data_->sharedState->status.status() != IOperation::Initialized){
		throw std::runtime_error("Transfer already started");
	}

	auto l = manager_.lock();
	if (l == nullptr){
		data_->sharedState->status.setError("Transfer manager expired");
		data_->sharedState->status.setStatus(Error);
		return;
	}	
	
	data_->sharedState->status.setStatus(Pending);
	data_->result = l->addRequest(data_->sharedState);
}

void CURLFTPTransfer::abort()
{
	data_->progress.abort();
}

void CURLFTPTransfer::wait()
{
	try{
		data_->result.wait();
		data_->resetStream();
	}
	catch (...){

	}
}

const CURLFTPTransfer::Status CURLFTPTransfer::status() const
{	
	return data_->sharedState->status.status();
}

const std::string CURLFTPTransfer::error() const
{
	return data_->sharedState->status.error();
}

const float CURLFTPTransfer::normalizedProgress() const
{
	return data_->progress.progress();
}

const float CURLFTPTransfer::elapsed() const
{
	double time = -1.0;
	auto ret = curl_easy_getinfo(data_->sharedState->curl.get(), CURLINFO_TOTAL_TIME, &time);
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
