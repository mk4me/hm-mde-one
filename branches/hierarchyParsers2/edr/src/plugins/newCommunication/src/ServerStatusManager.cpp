#include "ServerStatusManager.h"
#include <boost/bind.hpp>
#include <ctime>
#include <iterator>

ServerStatusManager::StatusChecker::StatusChecker(ServerStatusManager * manager) : manager(manager), checkDelay_(10000), finish_(false)
{

}

ServerStatusManager::StatusChecker::~StatusChecker()
{

}

void ServerStatusManager::StatusChecker::run()
{
	while(!finish_){

		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*manager);

		if(manager->refreshingServers() == false){

			for(auto it = manager->serverStatuses.begin(); it != manager->serverStatuses.end(); ++it){
				CommunicationManager::CompleteRequest completeRequest;
				completeRequest.callbacks = manager->pingCallbacks;
				completeRequest.request = manager->manager->createRequestPing(it->first);
				manager->manager->pushRequest(completeRequest);
			}

			manager->toCheck = manager->serverStatuses.size();
		}

		microSleep(checkDelay_);
	}
}

void ServerStatusManager::StatusChecker::finish()
{
	finish_ = true;
}

void ServerStatusManager::StatusChecker::setCheckDelay(unsigned int ms)
{
	checkDelay_ = ms;
}

unsigned int ServerStatusManager::StatusChecker::checkDelay()
{
	return checkDelay_;
}

ServerStatusManager::ServerStatusManager(CommunicationManager * manager)
{
	mThis = this;
	pingCallbacks.onEndCallback = boost::bind(&ServerStatusManager::onPingEnd, this, _1);
	pingCallbacks.onErrorCallback = boost::bind(&ServerStatusManager::onPingError, this, _1, _2);
	statusChecker.reset(new StatusChecker(this));
}

ServerStatusManager::~ServerStatusManager()
{
	if(statusChecker->isRunning() == true){
		statusChecker->finish();
		statusChecker->join();
	}
}

void ServerStatusManager::addServer(const std::string & url)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	auto it = serverStatuses.find(url);

	if(it == serverStatuses.end()){
		ServerStatus status;
		status.serverUrl = url;
		status.online = false;
		status.time = webservices::DateTime::now();
		serverStatuses[url] = status;
	}

	if(statusChecker->isRunning() == false){
		statusChecker->start();
	}
}

void ServerStatusManager::removeServer(const std::string & url)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	serverStatuses.erase(url);
}

ServerStatus ServerStatusManager::serverStatus(const std::string & url) const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	auto it = serverStatuses.find(url);
	return it->second;
}

ServerStatus ServerStatusManager::serverStatus(unsigned int i) const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	auto it = serverStatuses.begin();
	std::advance(it, i);
	return it->second;
}

unsigned int ServerStatusManager::serversCount() const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	return serverStatuses.size();
}

bool ServerStatusManager::empty() const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	return serverStatuses.empty();
}

void ServerStatusManager::setCheckDelay(unsigned int ms)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	statusChecker->setCheckDelay(ms);
}

unsigned int ServerStatusManager::checkDelay() const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	return statusChecker->checkDelay();
}

bool ServerStatusManager::refreshingServers() const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	return toCheck != 0;
}

void ServerStatusManager::onPingEnd(const CommunicationManager::BasicRequestPtr & request)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	--toCheck;
	CommunicationManager::PingRequestPtr ping = utils::dynamic_pointer_cast<CommunicationManager::PingRequest>(request);
	auto it = serverStatuses.find(ping->urlToPing());
	if(it != serverStatuses.end()){
		it->second.online = ping->getProgress() == 0.0 ? false : true;
		it->second.error = std::string();
		it->second.time = webservices::DateTime::now();
	}
}

void ServerStatusManager::onPingError(const CommunicationManager::BasicRequestPtr & request, const std::string & error)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(*mThis);
	--toCheck;
	CommunicationManager::PingRequestPtr ping = utils::dynamic_pointer_cast<CommunicationManager::PingRequest>(request);
	auto it = serverStatuses.find(ping->urlToPing());
	if(it != serverStatuses.end()){
		it->second.online = ping->getProgress() == 0.0 ? false : true;
		it->second.error = error;
		it->second.time = webservices::DateTime::now();
	}
}
