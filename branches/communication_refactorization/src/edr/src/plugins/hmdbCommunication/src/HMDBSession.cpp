#include "CommunicationPCH.h"
#include "HMDBSession.h"
#include "HMDBFTP.h"
#include "AccountFactoryWS.h"
#include "AdministrationWS.h"
#include "AuthorizationWS.h"
#include "BasicQueriesWS.h"
#include "BasicUpdatesWS.h"
#include "FileStoremanWS.h"
#include "UserPersonalSpaceWS.h"
#include <networkUtils/CURLManager.h>
#include <boost/bind.hpp>
#include "HMDBService.h"

using namespace hmdbCommunication;

const WSDLServiceCreator HMDBSession::serviceCreator(networkUtils::CURLManagerPtr manager,
	utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
	const std::string & user,
	const std::string & password,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	if (CAPath.empty() == false){
		return boost::bind(&HMDBService::createSecureWSDL, manager, executor, url, user, password, CAPath, hostVerification, schemaPath);
	}else{
		return boost::bind(&HMDBService::createUnsecureWSDL, manager, executor, url, user, password, schemaPath);
	}
}

const WSDLServiceCreator HMDBSession::systemServiceCreator(networkUtils::CURLManagerPtr manager,
	utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const core::Filesystem::Path & schemaPath)
{
	return serviceCreator(manager, executor, url, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification);
}

HMDBSession::HMDBSession(utils::shared_ptr<CURLFTPManager> ftpManager,
	networkUtils::CURLManagerPtr wsdlManager,
	utils::shared_ptr<XmlUtils::CURLExecutor> wsdlExecutor,
	const std::string & user,
	const std::string & password,
	const std::string & motionUrl,
	const std::string & medicalUrl,
	const std::string & motionFtp,
	const std::string & medicalFtp,
	const core::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification)
	: user_(user), motionUrl_(motionUrl),
	medicalUrl_(medicalUrl), motionFtpUrl_(motionFtp),
	medicalFtpUrl_(medicalFtp),
	ftpManager_(ftpManager),
	wsdlManager_(wsdlManager),
	medicalCreator_(serviceCreator(wsdlManager, wsdlExecutor, medicalUrl_, user, password, CAPath, hostVerification)),
	motionCreator_(serviceCreator(wsdlManager, wsdlExecutor, motionUrl_, user, password, CAPath, hostVerification)),
	motionAccountFactory_(motionUrl.empty() == false ? new SingleAccountFactoryWS(serviceCreator(wsdlManager, wsdlExecutor, motionUrl_, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification)) : nullptr),
	medicalAccountFactory_(medicalUrl.empty() == false ? new SingleAccountFactoryWS(serviceCreator(wsdlManager, wsdlExecutor, medicalUrl_, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification)) : nullptr),
	multiAccountFactory_((motionUrl.empty() == false && medicalUrl.empty() == false) ? new MultiAccountFactoryWS(serviceCreator(wsdlManager, wsdlExecutor, motionUrl_, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification)) : nullptr),
	administration_(motionUrl.empty() == false ? new AdministrationWS(motionCreator_) : nullptr),
	authorization_(motionUrl.empty() == false ? new AuthorizationWS(motionCreator_) : nullptr),
	motionBasicQueries_(motionUrl.empty() == false ? new MotionBasicQueriesWS(motionCreator_) : nullptr),
	medicalBasicQueries_(medicalUrl.empty() == false ? new MedicalBasicQueriesWS(medicalCreator_) : nullptr),
	motionBasicUpdate_(motionUrl.empty() == false ? new MotionBasicUpdatesWS(motionCreator_) : nullptr),
	medicalBasicUpdate_(medicalUrl.empty() == false ? new MedicalBasicUpdatesWS(medicalCreator_) : nullptr),
	motionFileStoreman_(motionUrl.empty() == false ? new MotionFileStoremanWS(motionCreator_) : nullptr),
	medicalFileStoreman_(medicalUrl.empty() == false ? new MedicalFileStoremanWS(medicalCreator_) : nullptr),
	userPersonalSpace_(motionUrl.empty() == false ? new UserPersonalSpaceWS(motionCreator_) : nullptr),
	motionFtp_(motionFtp.empty() == false ? new HMDBFTP("testUser", "testUser", motionFtpUrl_, ftpManager) : nullptr),
	medicalFtp_(medicalFtp.empty() == false ? new HMDBFTP("testUser", "testUser", medicalFtpUrl_, ftpManager) : nullptr)
{

}

HMDBSession::~HMDBSession()
{

}

const std::string HMDBSession::motionServicesUrl() const
{
	return motionUrl_;
}

const std::string HMDBSession::medicalServicesUrl() const
{
	return medicalUrl_;
}

const std::string HMDBSession::motionFtpUrl() const
{
	return motionFtpUrl_;
}

const std::string HMDBSession::medicalFtpUrl() const
{
	return medicalFtpUrl_;
}

const std::string HMDBSession::userName() const
{
	return user_;
}

hmdbServices::ISingleAccountFactoryWS * HMDBSession::motionAccountFactory()
{
	return motionAccountFactory_.get();
}

hmdbServices::ISingleAccountFactoryWS * HMDBSession::medicalAccountFactory()
{
	return medicalAccountFactory_.get();
}

hmdbServices::IMultiAccountFactoryWS * HMDBSession::multiAccountFactory()
{
	return multiAccountFactory_.get();
}

hmdbServices::IAdministrationWS * HMDBSession::administration()
{
	return administration_.get();
}

hmdbServices::IAuthorizationWS * HMDBSession::authorization()
{
	return authorization_.get();
}

hmdbServices::IMotionBasicQueriesWS * HMDBSession::motionQueries()
{
	return motionBasicQueries_.get();
}

hmdbServices::IGeneralBasicQueriesWS * HMDBSession::medicalQueries()
{
	return medicalBasicQueries_.get();
}

hmdbServices::IMotionBasicUpdatesWS * HMDBSession::motionUpdate()
{
	return motionBasicUpdate_.get();
}

hmdbServices::IMedicalBasicUpdatesWS * HMDBSession::medicalUpdate()
{
	return medicalBasicUpdate_.get();
}

hmdbServices::IMotionFileStoremanWS * HMDBSession::motionFilestoreman()
{
	return motionFileStoreman_.get();
}

hmdbServices::IMedicalFileStoremanWS * HMDBSession::medicalFilestoreman()
{
	return medicalFileStoreman_.get();
}

hmdbServices::IUserPersonalSpaceWS * HMDBSession::userPersonalSpace()
{
	return userPersonalSpace_.get();
}

IHMDBFtp * HMDBSession::motionFtp()
{
	return motionFtp_.get();
}

IHMDBFtp * HMDBSession::medicalFtp()
{
	return medicalFtp_.get();
}