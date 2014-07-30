#include "CommunicationPCH.h"
#include "CURLFTPProgress.h"

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
	progress_ = progress;
}

const float CURLFTPProgress::progress() const
{
	return progress_;
}