#include "CommunicationPCH.h"
#include "HMDBService.h"
#include "HMDBSession.h"
#include <corelib/IThreadPool.h>
#include <threadingUtils/IThread.h>
#include <boost/bind.hpp>
#include "ShallowCopyFilterManager.h"
#include "WSDLPULLService.h"
#include <wsdlparser/WsdlInvoker.h>

using namespace hmdbCommunication;

class HMDBCURLExecutor : public XmlUtils::CURLExecutor
{
public:
	HMDBCURLExecutor(const networkUtils::CURLManagerPtr manager) : manager(manager) {}

	virtual ~HMDBCURLExecutor() {}

	virtual CURLcode execute(CURL * curl)
	{
		utils::shared_ptr<networkUtils::CURLManager::WaitCurl> wait(new networkUtils::CURLManager::WaitCurl);
		manager->addRequest(curl, wait.get());
		wait->wait();
		return wait->result();
	}

private:
	const networkUtils::CURLManagerPtr manager;
};

const utils::shared_ptr<XmlUtils::CURLExecutor> HMDBService::createCurlExecutor(const networkUtils::CURLManagerPtr manager)
{
	return utils::shared_ptr<XmlUtils::CURLExecutor>(new HMDBCURLExecutor(manager));
}

HMDBService::HMDBService() : mainWidget(nullptr), filterManager_(new ShallowCopyFilterManager),
finalizeServices_(false), finalizeData_(false)
{

}

HMDBService::~HMDBService()
{

}

IHMDBSession * HMDBService::createSession(const std::string & motionServicesUrl,
	const std::string & medicalServicesUrl,
	const std::string & user,
	const std::string & password,
	const std::string & motionDataUrl,
	const std::string & medicalDataUrl,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification)
{
	return new HMDBSession(dataManager, servicesManager, serviceCurlExecutor,
		user, password,	motionServicesUrl, medicalServicesUrl, motionDataUrl,
		medicalDataUrl, CAPath, hostVerification);
}

IHMDBSession * HMDBService::createSession(const bool motion,
	const std::string & servicesUrl,
	const std::string & user,
	const std::string & password,
	const std::string & dataUrl,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification)
{
	IHMDBSession * ret = nullptr;

	if (motion == true){
		ret = new HMDBSession(dataManager, servicesManager, serviceCurlExecutor,
			user, password,	servicesUrl, "", dataUrl,
			"", CAPath, hostVerification);
	}
	else{
		ret = new HMDBSession(dataManager, servicesManager, serviceCurlExecutor,
			user, password,	"", servicesUrl, "",
			dataUrl, CAPath, hostVerification);
	}

	return ret;
}

networkUtils::IWSDLServicePtr HMDBService::createSecureWSDL(networkUtils::CURLManagerPtr manager,
	utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
	const std::string & user,
	const std::string & password,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	auto invoker = new WsdlPull::CustomSSLWsdlInvoker(url, CAPath.string(),
		static_cast<WsdlPull::CustomSSLWsdlInvoker::HostVerification>(hostVerification),
		schemaPath.string(),
		executor.get());

	invoker->setAuth(user, password);

	return networkUtils::IWSDLServicePtr(new WSDLPULLServiceT<WsdlPull::CustomSSLWsdlInvoker>(invoker));
}

networkUtils::IWSDLServicePtr HMDBService::createUnsecureWSDL(networkUtils::CURLManagerPtr manager,
	utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
	const std::string & user,
	const std::string & password,
	const core::Filesystem::Path & schemaPath)
{
	auto invoker = new WsdlPull::WsdlInvoker(url, schemaPath.string(), executor.get());
	invoker->setAuth(user, password);

	return networkUtils::IWSDLServicePtr(new WSDLPULLServiceT<WsdlPull::WsdlInvoker>(invoker));
}

const networkUtils::IWSDLServicePtr HMDBService::createService(
	const std::string & url,
	const std::string & user,
	const std::string & password,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	if (CAPath.empty() == true){
		return createSecureWSDL(servicesManager, serviceCurlExecutor, url, user, password, CAPath, hostVerification, schemaPath);
	}
	else{
		return createUnsecureWSDL(servicesManager, serviceCurlExecutor, url, user, password, schemaPath);
	}
}

const networkUtils::IWSDLServicePtr HMDBService::createSystemService(
	const std::string & url,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	return createService(url, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification, schemaPath);
}

static size_t DummyWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	return size * nmemb;
}

const bool HMDBService::serverOnline(const std::string & url,
	const unsigned int timeout,
	const core::Filesystem::Path & caPath) const
{
	CURL * curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HEADER, false);
	curl_easy_setopt(curl, CURLOPT_AUTOREFERER, true);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
	curl_easy_setopt(curl, CURLOPT_NOBODY, true);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &DummyWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, 0);

	if (caPath.empty() == false){
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true);
		curl_easy_setopt(curl, CURLOPT_CAPATH, caPath.string().c_str());
	}
	else{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}

	bool ret = false;

	try{
		networkUtils::CURLManager::WaitCurl  wait;
		servicesManager->addRequest(curl, &wait);
		wait.wait();

		if (wait.result() == CURLE_OK){

			long retcode = -1;
			//kiedys CURLINFO_HTTP_CODE
			auto resp = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
			curl_easy_cleanup(curl);

			if (resp == CURLE_OK && retcode == 200){
				ret = true;
			}
		}
	}
	catch (...){

	}

	return ret;
}

void HMDBService::attach(IHMDBSession * session)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	//! TODO
	//! podpi¹æ pod widget
}

void HMDBService::detach(IHMDBSession * session)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	//! TODO
	//! od³¹czyæ widgeta
}

IShallowCopyFilterManager * HMDBService::filterManager()
{
	return filterManager_.get();
}

const IShallowCopyFilterManager * HMDBService::filterManager() const
{
	return filterManager_.get();
}

void HMDBService::init(core::ISourceManager * sourceManager,
	core::IVisualizerManager * visualizerManager,
	core::IMemoryDataManager * memoryDataManager,
	core::IStreamDataManager * streamDataManager,
	core::IFileDataManager * fileDataManager)
{
	core::IThreadPool::Threads threads;

	plugin::getThreadPool()->getThreads("HMDB Service", threads, 2, true);

	core::IThreadPtr st = threads.front();
	core::IThreadPtr dt = threads.back();
	networkUtils::CURLManagerPtr sM(new networkUtils::CURLManager);
	utils::shared_ptr<CURLFTPManager> dM(new CURLFTPManager);

	threadingUtils::IRunnablePtr rS(new threadingUtils::FunctorRunnable(boost::bind(&HMDBService::runServices, this)));
	threadingUtils::IRunnablePtr rD(new threadingUtils::FunctorRunnable(boost::bind(&HMDBService::runData, this)));

	st->start(rS);
	dt->start(rD);	

	dataCurlExecutor.reset(new HMDBCURLExecutor(dM));
	serviceCurlExecutor.reset(new HMDBCURLExecutor(sM));

	servicesThread.swap(st);
	dataThread.swap(dt);
	servicesManager.swap(sM);
	dataManager.swap(dM);
}

const bool HMDBService::lateInit()
{
	return true;
}

void HMDBService::finalize()
{
	finalizeServices_ = true;
	finalizeData_ = true;

	servicesManager->finalize();
	dataManager->finalize();

	servicesThread->join();
	dataThread->join();
}

void HMDBService::update(double deltaTime)
{

}

QWidget* HMDBService::getWidget()
{
	return mainWidget;
}

QWidgetList HMDBService::getPropertiesWidgets()
{
	return QWidgetList();
}

void HMDBService::runServices()
{
	while (finalizeServices_ == false){
		try{
			servicesManager->process();
		}
		catch (...){

		}
	}
}

void HMDBService::runData()
{
	while (finalizeData_ == false){
		try{
			dataManager->process();
		}
		catch (...){

		}
	}
}