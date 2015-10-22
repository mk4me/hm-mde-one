/********************************************************************
	created:  2014/05/29	12:56:00
	filename: HMDBSession.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSESSION_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSESSION_H__

#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <utils/Filesystem.h>
#include <networkUtils/Types.h>
#include <networkUtils/CURLManager.h>
#include <utils/SmartPtr.h>
#include "WSDLServiceTypes.h"
#define NOMINMAX
#include <xmlpull/XmlUtils.h>

namespace hmdbServices
{
	class ISingleAccountFactoryWS;
	class ISingleAccountFactoryWS;
	class IMultiAccountFactoryWS;
	class IAdministrationWS;
	class IAuthorizationWS;
	class IMotionBasicQueriesWS;
	class IGeneralBasicQueriesWS;
	class IMotionBasicUpdatesWS;
	class IMedicalBasicUpdatesWS;
	class IMotionFileStoremanWS;
	class IMedicalFileStoremanWS;
	class IUserPersonalSpaceWS;
}

namespace hmdbCommunication
{
	class HMDBFTP;
	class CURLFTPManager;

	class HMDBSession : public IHMDBSession
	{
	public:

		static const WSDLServiceCreator serviceCreator(
			const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		static const WSDLServiceCreator systemServiceCreator(
			const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

	public:
		//! \param ftpManager obs�uguj�cy po��czenia ftp CURLa
		//! \param wsdlManager obs�uguj�cy po��czenia us�ug CURLa
		//! \param wsdlExecutor CURL executor dla us�ug
		//! \param user Uzytkownik
		//! \param password Has�o
		//! \param motionUrl Adres us�ug ruchu
		//! \param medicalUrl Adres us�ug medycznych
		//! \param motionFtp Dane ruchu
		//! \param medicalFtp Dane medyczne
		//! \param CAPath �cie�ka certyfikatu SSL dla us�ug
		//! \param hostVerification Spos�b uwierzytelniania serwera us�ug
		HMDBSession(utils::shared_ptr<CURLFTPManager> ftpManager,
			networkUtils::CURLManagerPtr wsdlManager,
			utils::shared_ptr<XmlUtils::CURLExecutor> wsdlExecutor,
			const std::string & user,
			const std::string & password,
			const std::string & motionUrl,
			const std::string & medicalUrl,
			const std::string & motionFtp,
			const std::string & medicalFtp,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone);

		//! Destruktor wirtualny
		virtual ~HMDBSession();

		//! \return Adres us�ug ruchu
		virtual const std::string motionServicesUrl() const;
		//! \return Adres us�ug medycznych
		virtual const std::string medicalServicesUrl() const;
		//! \return Adres danych ruchu
		virtual const std::string motionFtpUrl() const;
		//! \return Adres danych medycznych
		virtual const std::string medicalFtpUrl() const;
		//! \return Nazwa u�ytkownika
		virtual const std::string userName() const;

		//! \return Us�uga tworz�ca konta danych ruchu
		virtual hmdbServices::ISingleAccountFactoryWS * motionAccountFactory();
		//! \return Us�uga tworz�ca konta danych medycznych
		virtual hmdbServices::ISingleAccountFactoryWS * medicalAccountFactory();
		//! \return Us�uga tworz�ca konta danych euchu i medycznych
		virtual hmdbServices::IMultiAccountFactoryWS * multiAccountFactory();
		//! \return Us�ugi administracyjne
		virtual hmdbServices::IAdministrationWS * administration();
		//! \return Us�ugi autoryzacyjne
		virtual hmdbServices::IAuthorizationWS * authorization();
		//! \return Us�ugi zapyta� o dane ruchu
		virtual hmdbServices::IMotionBasicQueriesWS * motionQueries();
		//! \return Us�ugi zapyta� o dane medyczne
		virtual hmdbServices::IGeneralBasicQueriesWS * medicalQueries();
		//! \return Us�ugi aktualizuj�ce dane ruchu
		virtual hmdbServices::IMotionBasicUpdatesWS * motionUpdate();
		//! \return Us�ugi aktualizuj�ce dane medyczne
		virtual hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate();
		//! \return Us�ugi dost�pu do danych ruchu
		virtual hmdbServices::IMotionFileStoremanWS * motionFilestoreman();
		//! \return Us�ugi dost�pu do danych medycznych
		virtual hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman();
		//! \return Us�ugi dost�pu do danych prywatnych u�ytkownika
		virtual hmdbServices::IUserPersonalSpaceWS * userPersonalSpace();
		//! \return Po��czenie do pobierania danych ruchu
		virtual IHMDBFtp * motionFtp();
		//! \return Po��czenie do pobierania danych medycznych
		virtual IHMDBFtp * medicalFtp();

	private:
		//! U�ytkownik
		const std::string user_;
		//! Adres us�ug ruchu
		const std::string motionUrl_;
		//! Adres us�ug medycznych
		const std::string medicalUrl_;
		//! Adres danych ruchu
		const std::string motionFtpUrl_;
		//! Adres danych medycznych
		const std::string medicalFtpUrl_;

		//! Manager po�acze� ftp
		utils::shared_ptr<CURLFTPManager> ftpManager_;
		//! Manager po�acze� us�ug curla
		networkUtils::CURLManagerPtr wsdlManager_;

		utils::shared_ptr<hmdbServices::ISingleAccountFactoryWS> motionAccountFactory_;
		utils::shared_ptr<hmdbServices::ISingleAccountFactoryWS> medicalAccountFactory_;
		utils::shared_ptr<hmdbServices::IMultiAccountFactoryWS> multiAccountFactory_;
		utils::shared_ptr<hmdbServices::IAdministrationWS> administration_;
		utils::shared_ptr<hmdbServices::IAuthorizationWS> authorization_;
		utils::shared_ptr<hmdbServices::IMotionBasicQueriesWS> motionBasicQueries_;
		utils::shared_ptr<hmdbServices::IGeneralBasicQueriesWS> medicalBasicQueries_;
		utils::shared_ptr<hmdbServices::IMotionBasicUpdatesWS> motionBasicUpdate_;
		utils::shared_ptr<hmdbServices::IMedicalBasicUpdatesWS> medicalBasicUpdate_;
		utils::shared_ptr<hmdbServices::IMotionFileStoremanWS> motionFileStoreman_;
		utils::shared_ptr<hmdbServices::IMedicalFileStoremanWS> medicalFileStoreman_;
		utils::shared_ptr<hmdbServices::IUserPersonalSpaceWS> userPersonalSpace_;
		utils::shared_ptr<HMDBFTP> motionFtp_;
		utils::shared_ptr<HMDBFTP> medicalFtp_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSESSION_H__