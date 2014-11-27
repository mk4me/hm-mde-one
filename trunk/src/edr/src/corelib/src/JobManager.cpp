#include "CoreLibPCH.h"
#include <corelib/JobManager.h>

using namespace core;

LogPtr JobManager::log_;
JobManager::JobBase::TLS JobManager::JobBase::tlsJD;

void JobManager::logError(const std::string & message)
{
	LOG_ERROR(log_, "Job " << JobBase::currentName() << " owned by " << JobBase::currentOwner() << message << ". Job processing finished.");
}

void JobManager::setLog(LogPtr log)
{
	log_ = log;
}

JobManager::JobManager(InnerJobManager * jm) : jm(jm)
{

}