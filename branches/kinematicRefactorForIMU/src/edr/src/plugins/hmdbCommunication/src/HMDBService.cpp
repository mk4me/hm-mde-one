#include "CommunicationPCH.h"
#include "HMDBService.h"
#include "HMDBSession.h"
#include "WSDLPULLService.h"
#include <wsdlparser/WsdlInvoker.h>
#include <corelib/PluginCommon.h>
#include <boost/algorithm/hex.hpp>

using namespace hmdbCommunication;

int curlLogDebugCallback(CURL *handle,
	curl_infotype type,
	char *data,
	size_t size,
	void *userptr)
{
	std::string message;

	switch (type)
	{
	case CURLINFO_TEXT:
		message = "text -> " + std::string(data, size);
		PLUGIN_LOG_NAMED_DEBUG("curl", message);
		break;

	/*case CURLINFO_HEADER_OUT:
		message = "header out => " + std::string(data, size);
		break;
	case CURLINFO_DATA_OUT:
		message = "data out => ";		
		boost::algorithm::hex(data, data + size, std::back_inserter(message));
		break;
	case CURLINFO_SSL_DATA_OUT:
		message = "SSL data out #?=> ";		
		boost::algorithm::hex(data, data + size, std::back_inserter(message));
		break;
	case CURLINFO_HEADER_IN:
		message = "header in <= " + std::string(data, size);
		break;
	case CURLINFO_DATA_IN:
		message = "data in <= ";		
		boost::algorithm::hex(data, data + size, std::back_inserter(message));
		break;
	case CURLINFO_SSL_DATA_IN:
		message = "SSL data in <=#? ";		
		boost::algorithm::hex(data, data + size, std::back_inserter(message));
		break;

	default:
		message = "Unrecognized info type -> ";		
		boost::algorithm::hex(data, data + size, std::back_inserter(message));
		break;*/

	}



	return 0;
}

class HMDBCURLExecutor : public XmlUtils::CURLExecutor
{
public:
	HMDBCURLExecutor(const networkUtils::CURLManagerPtr manager) : manager(manager) {}

	virtual ~HMDBCURLExecutor() {}

	virtual CURLcode execute(CURL * curl)
	{
		HMDBService::curlEnableLog(curl);
		auto f = manager->addRequest(curl);
		return f.get();
	}

private:
	const networkUtils::CURLManagerPtr manager;
};

const utils::shared_ptr<XmlUtils::CURLExecutor> HMDBService::createCurlExecutor(const networkUtils::CURLManagerPtr manager)
{
	return utils::shared_ptr<XmlUtils::CURLExecutor>(new HMDBCURLExecutor(manager));
}

void HMDBService::curlEnableLog(CURL * curl)
{
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curlLogDebugCallback);
	curl_easy_setopt(curl, CURLOPT_DEBUGDATA, 0L);
}

void HMDBService::curlDisableLog(CURL * curl)
{
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
}


HMDBService::HMDBService()
	: mainWidget(nullptr), finalizeServices_(false),
	finalizeData_(false)
{	
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

HMDBService::~HMDBService()
{
	curl_global_cleanup();
}

const IHMDBSessionPtr HMDBService::createSession(const std::string & motionServicesUrl,
	const std::string & medicalServicesUrl,
	const std::string & user,
	const std::string & password,
	const std::string & motionDataUrl,
	const std::string & medicalDataUrl,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification)
{
	return IHMDBSessionPtr(new HMDBSession(dataManager, servicesManager, serviceCurlExecutor,
		user, password,	motionServicesUrl, medicalServicesUrl, motionDataUrl,
		medicalDataUrl, CAPath, hostVerification));
}

const IHMDBSessionPtr HMDBService::createSession(const bool motion,
	const std::string & servicesUrl,
	const std::string & user,
	const std::string & password,
	const std::string & dataUrl,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification)
{
	IHMDBSessionPtr ret;

	if (motion == true){
		ret.reset(new HMDBSession(dataManager, servicesManager, serviceCurlExecutor,
			user, password,	servicesUrl, "", dataUrl,
			"", CAPath, hostVerification));
	}
	else{
		ret.reset(new HMDBSession(dataManager, servicesManager, serviceCurlExecutor,
			user, password,	"", servicesUrl, "",
			dataUrl, CAPath, hostVerification));
	}

	return ret;
}

const networkUtils::IWSDLServicePtr HMDBService::createSecureWSDL(
	const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
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

const networkUtils::IWSDLServicePtr HMDBService::createUnsecureWSDL(
	const utils::shared_ptr<XmlUtils::CURLExecutor> executor,
	const std::string & url,
	const std::string & user,
	const std::string & password,
	const core::Filesystem::Path & schemaPath)
{
	auto invoker = new WsdlPull::WsdlInvoker(url, schemaPath.string(), executor.get());
	invoker->setAuth(user, password);

	return networkUtils::IWSDLServicePtr(new WSDLPULLServiceT<WsdlPull::WsdlInvoker>(invoker));
}

const networkUtils::IWSDLServicePtr HMDBService::createHMDBService(
	const std::string & url,
	const std::string & user,
	const std::string & password,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	if (CAPath.empty() == true){
		return createSecureWSDL(serviceCurlExecutor, url, user, password, CAPath, hostVerification, schemaPath);
	}
	else{
		return createUnsecureWSDL(serviceCurlExecutor, url, user, password, schemaPath);
	}
}

const networkUtils::IWSDLServicePtr HMDBService::createHMDBSystemService(
	const std::string & url,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	return createHMDBService(url, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification, schemaPath);
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
		auto f = servicesManager->addRequest(curl);
		if (f.get() == CURLE_OK){

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

void HMDBService::attach(IHMDBSessionPtr session)
{
	std::lock_guard<std::recursive_mutex> lock(sync_);
	//! TODO
	//! podpi�� pod widget
}

void HMDBService::detach(IHMDBSessionPtr session)
{
	std::lock_guard<std::recursive_mutex> lock(sync_);
	//! TODO
	//! od��czy� widgeta
}

void HMDBService::init(core::ISourceManager * sourceManager,
	core::IVisualizerManager * visualizerManager,
	core::IMemoryDataManager * memoryDataManager,
	core::IStreamDataManager * streamDataManager,
	core::IFileDataManager * fileDataManager)
{
	core::ThreadPool::Threads threads;

	plugin::getThreadPool()->get(2, threads, true, "HMDB Service", "Network operations");

	servicesThread = std::move(threads.front());
	dataThread = std::move(threads.back());
	servicesManager.reset(new networkUtils::CURLManager);
	dataManager.reset(new CURLFTPManager);

	dataCurlExecutor.reset(new HMDBCURLExecutor(dataManager));
	serviceCurlExecutor.reset(new HMDBCURLExecutor(servicesManager));

	servicesThread.run(&HMDBService::runServices, this);
	dataThread.run(&HMDBService::runData, this);
	WsdlPull::SCHEMADIR = (plugin::getPaths()->getResourcesPath() / "schemas/").string();
	PLUGIN_LOG_INFO("WSDLPULL SCHEMADIR: " << WsdlPull::SCHEMADIR);

#if ((defined _WIN32) || (defined _WIN64))
	XmlUtils::TMPFILESDIR = (plugin::getPaths()->getTmpPath()).string();
	PLUGIN_LOG_INFO("XmlUtils TMPFILESDIR: " << XmlUtils::TMPFILESDIR);
#else
	UTILS_ASSERT(false); // co zrobic z tym SCHEMADIR??
#endif
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

	if (servicesThread.joinable() == true){
		servicesThread.join();
	}

	if (dataThread.joinable() == true){
		dataThread.join();
	}
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
