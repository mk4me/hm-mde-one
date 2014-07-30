#include "CommunicationPCH.h"
#include "HMDBSource.h"
#include "HMDBSourceWidget.h"
#include "HMDBSourceContext.h"
#include "HMDBDataContext.h"
#include "HMDBLocalContext.h"
#include "HMDBRemoteContext.h"
#include "HMDBSession.h"
#include <corelib/IServiceManager.h>
#include <corelib/PluginCommon.h>
#include <plugins/hmdbCommunication/IHMDBService.h>
#include "HMDBService.h"
#include <hmdbserviceslib/AccountFactoryWS.h>

using namespace hmdbCommunication;

HMDBSource::HMDBSource() : memoryDM(nullptr),
streamDM(nullptr), fileDM(nullptr), mainWidget(nullptr)
{

}

HMDBSource::~HMDBSource()
{

}

void HMDBSource::init(core::IMemoryDataManager * memoryDM,
	core::IStreamDataManager * streamDM,
	core::IFileDataManager * fileDM)
{
	this->memoryDM = memoryDM;
	this->streamDM = streamDM;
	this->fileDM = fileDM;
}

bool HMDBSource::lateInit()
{
	return true;
}

void HMDBSource::finalize()
{
	//TODO
	//zamkn¹æ wszystkie widgety z kontekstami,
	//posprz¹taæ po nich - usuwamy
	ContextViews().swap(contextViews);
}

void HMDBSource::update(double deltaTime)
{

}

QWidget * HMDBSource::getWidget()
{
	if (mainWidget == nullptr){
		mainWidget = new HMDBSourceWidget(this);
	}

	return mainWidget;
}

QWidget * HMDBSource::getControlWidget()
{
	return nullptr;
}

QWidget * HMDBSource::getSettingsWidget()
{
	return nullptr;
}

void HMDBSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{

}

IHMDBSourceContext * HMDBSource::createSourceContext(IHMDBSession * session,
	IHMDBStorage * storage, const std::string & userName,
	const std::string & userHash)
{
	std::auto_ptr<HMDBDataContext> dCtx(new HMDBDataContext(storage, userName, userHash));
	std::auto_ptr<HMDBLocalContext> lCtx(new HMDBLocalContext(dCtx.get(), memoryDM, streamDM));
	std::auto_ptr<HMDBRemoteContext> rCtx(session != nullptr && userHash.empty() == false ? new HMDBRemoteContext(session, userHash) : nullptr);
		
	return new HMDBSourceContext(
		dCtx.release(),
		lCtx.release(),
		rCtx.release());
}

void HMDBSource::registerSourceContextViewPrototype(IHMDBSourceContextView * prototype)
{
	for (auto it = contextViews.begin(); it != contextViews.end(); ++it){
		if (it->first.get() == prototype){
			throw std::runtime_error("HMDBSourceContextView prototype already registered");
		}
	}

	contextViews[SourceContextViewPtr(prototype)];
}

const unsigned int HMDBSource::sourceContextViewPrototypesCount() const
{
	return contextViews.size();
}

IHMDBSource::IHMDBSourceContextView * HMDBSource::sourceContextViewPrototype(const unsigned int idx)
{
	auto it = contextViews.begin();
	std::advance(it, idx);
	return it->first.get();
}

const IHMDBSource::IHMDBSourceContextView * HMDBSource::sourceContextViewPrototype(const unsigned int idx) const
{
	auto it = contextViews.begin();
	std::advance(it, idx);
	return it->first.get();
}

const bool HMDBSource::registerSourceContextConfiguration(IHMDBSourceContextView * prototype,
	const ContextConfiguration & config)
{

	if (config.name.isEmpty() == true){
		return false;
	}

	bool ret = false;

	auto it = contextViews.begin();
	for (; it != contextViews.end(); ++it){
		if (it->first.get() == prototype){
			break;
		}
	}

	if (it != contextViews.end() && it->second.find(config.name) == it->second.end()){
		it->second[config.name] = config;
		ret = true;
	}

	return ret;
}

const unsigned int HMDBSource::sourceContextConfigurationsCount(IHMDBSourceContextView * prototype) const
{
	unsigned int ret = 0;

	auto it = contextViews.begin();
	for (; it != contextViews.end(); ++it){
		if (it->first.get() == prototype){
			break;
		}
	}

	if (it != contextViews.end()){
		ret = it->second.size();
	}

	return ret;
}

const HMDBSource::ContextConfiguration HMDBSource::sourceContextConfiguration(IHMDBSourceContextView * prototype,
	const unsigned int idx) const
{

	ContextConfiguration ret;

	auto it = contextViews.begin();
	for (; it != contextViews.end(); ++it){
		if (it->first.get() == prototype){
			break;
		}
	}

	if (it != contextViews.end()){
		auto IT = it->second.begin();
		std::advance(IT, idx);
		ret = IT->second;
	}

	return ret;
}

const networkUtils::IWSDLServicePtr createSystemService(const std::string & url,
	const std::string & caPath)
{
	networkUtils::IWSDLServicePtr ret;

	auto sm = plugin::getServiceManager();
	if (sm != nullptr){
		auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService != nullptr){

			ret = hmdbService->createSystemService(url, caPath, networkUtils::HVMatch);

			if (ret == nullptr){
				throw std::runtime_error("Could not create system service");
			}
		}
		else{
			throw std::runtime_error("Could not find HMDBService");
		}
	}
	else{
		throw std::runtime_error("Uninitialized service manager");
	}

	return ret;
}

void HMDBSource::registerAccount(const std::string & url,
	const std::string & login, const std::string & password,
	const std::string & name, const std::string & surname,
	const std::string & email, const bool multi,
	const std::string & caPath)
{

	auto s = createSystemService(url, caPath);

	if (multi == true){

		hmdbServices::MultiAccountFactoryWS(s).createUserAccount(login, email, password, name, surname, true);

	}
	else{

		hmdbServices::SingleAccountFactoryWS(s).createUserAccount(login, email, password, name, surname);

	}
}

void HMDBSource::activateAccount(const std::string & url,
	const std::string & login,
	const std::string & activationCode, const bool multi,
	const std::string & caPath)
{
	auto s = createSystemService(url, caPath);

	if (multi == true){

		hmdbServices::MultiAccountFactoryWS(s).activateUserAccount(login, activationCode, true);

	}
	else{

		hmdbServices::SingleAccountFactoryWS(s).activateUserAccount(login, activationCode);

	}
}

void HMDBSource::resetPassword(const std::string & url,
	const std::string & email, const bool multi,
	const std::string & caPath)
{
	auto s = createSystemService(url, caPath);

	if (multi == true){

		hmdbServices::MultiAccountFactoryWS(s).resetPassword(email, true);

	}
	else{

		hmdbServices::SingleAccountFactoryWS(s).resetPassword(email);

	}
}

const unsigned int HMDBSource::size() const
{
	return sourceContexts_.size();
}

IHMDBSourceContext * HMDBSource::sourceContext(const unsigned int idx)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	auto it = sourceContexts_.begin();
	std::advance(it, idx);
	return *it;
}

const IHMDBSourceContext * HMDBSource::sourceContext(const unsigned int idx) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	auto it = sourceContexts_.begin();
	std::advance(it, idx);
	return *it;
}

/*
IHMDBSourceContext * HMDBSource::sourceContextForData(core::VariantConstPtr data)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	IHMDBSourceContext * ret = nullptr;
	for (auto it = sourceContexts_.begin(); it != sourceContexts_.end(); ++it){
		if ((*it)->isMyData(data) == true){
			ret = *it;
			break;
		}
	}

	return ret;
}

const IHMDBSourceContext * HMDBSource::sourceContextForData(core::VariantConstPtr data) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	IHMDBSourceContext * ret = nullptr;
	for (auto it = sourceContexts_.begin(); it != sourceContexts_.end(); ++it){
		if ((*it)->isMyData(data) == true){
			ret = *it;
			break;
		}
	}

	return ret;
}*/