#include "CommunicationPCH.h"
#include "CURLFTPProgress.h"
#include <utils/Debug.h>

using namespace hmdbCommunication;

CURLFTPProgress::CURLFTPProgress()
	: aborted_(false), progress_(0.0)
{

}

CURLFTPProgress::~CURLFTPProgress()
{

}

void CURLFTPProgress::abort()
{
	aborted_ = true;
}

const bool CURLFTPProgress::aborted() const
{
	return aborted_;
}

void CURLFTPProgress::setProgress(const float progress)
{
	UTILS_ASSERT(progress >= progress_);
	progress_ = progress;
}

const float CURLFTPProgress::progress() const
{
	return progress_;
}

void CURLFTPProgress::setProcessedData(const unsigned long long processedData)
{
	processedData_ = processedData;
}

const unsigned long long CURLFTPProgress::processedData() const
{
	return processedData_;
}
