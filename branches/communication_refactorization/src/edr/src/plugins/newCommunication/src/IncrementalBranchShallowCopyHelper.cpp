#include "CommunicationPCH.h"
#include "IncrementalBranchShallowCopyHelper.h"
#include <boost/bind.hpp>
#include "hmdbserviceslib/IncrementalBranchShallowCopy.h"
#include "hmdbserviceslib/IncrementalBranchShallowCopyParser.h"

using namespace communication;

hmdbServices::IncrementalBranchShallowCopy communication::IncrementalBranchShallowCopyHelper::getIncrementalShallowCopy(CommunicationManager* cm, const std::string& savePath, const hmdbServices::DateTime& dt)
{
	CommunicationManager::BasicRequestPtr  req = CommunicationManager::createRequestMotionShallowBranchIncrement(savePath, dt);

	IncrementalBranchShallowCopyHelper helper(cm);
	CommunicationManager::CompleteRequest cpl;
	cpl.request = req;
	cpl.callbacks = helper.singleTransferCallbacks;
	cm->pushRequest(cpl);
	State s = helper.getState();

	//TODO - to jest odpalane w wątku Qt - takie czekanie mrozi całą aplikację na czas ściągania
	// tak ma być?
	while (s != Success && s != Error) {
		s = helper.getState();
	}

	if (s == Success) {
		hmdbServices::IncrementalBranchShallowCopy shallow;
		hmdbServices::IncrementalBranchShallowCopyParser::parseFile(savePath, shallow);
		return shallow;
	}
	else {
		throw std::runtime_error(helper.error);
	}
}

communication::IncrementalBranchShallowCopyHelper::IncrementalBranchShallowCopyHelper(CommunicationManager* cm) :
communicationManager(cm),
state(Idle)
{
	//inicjalizacja callbacków pojedynczego pobierania
	singleTransferCallbacks.onBeginCallback = (CommunicationManager::RequestCallback)boost::bind(&IncrementalBranchShallowCopyHelper::onDownloadBegin, this, _1);
	singleTransferCallbacks.onEndCallback = (CommunicationManager::RequestCallback)boost::bind(&IncrementalBranchShallowCopyHelper::onDownloadComplete, this, _1);
	singleTransferCallbacks.onCancelCallback = (CommunicationManager::RequestCallback)boost::bind(&IncrementalBranchShallowCopyHelper::onDownloadCancel, this, _1);
	singleTransferCallbacks.onErrorCallback = (CommunicationManager::RequestErrorCallback)boost::bind(&IncrementalBranchShallowCopyHelper::onDownloadError, this, _1, _2);
}

communication::IncrementalBranchShallowCopyHelper::State communication::IncrementalBranchShallowCopyHelper::getState() const
{
	return state;
}

void communication::IncrementalBranchShallowCopyHelper::onDownloadBegin(const CommunicationManager::BasicRequestPtr & request)
{
	state = Started;
}

void communication::IncrementalBranchShallowCopyHelper::onDownloadCancel(const CommunicationManager::BasicRequestPtr & request)
{
	state = Error;
	error = "Downloading Cancelled";
}

void communication::IncrementalBranchShallowCopyHelper::onDownloadComplete(const CommunicationManager::BasicRequestPtr & request)
{
	state = Success;
}

void communication::IncrementalBranchShallowCopyHelper::onDownloadError(const CommunicationManager::BasicRequestPtr & request, const std::string & error)
{
	state = Error;
	this->error = error;
}