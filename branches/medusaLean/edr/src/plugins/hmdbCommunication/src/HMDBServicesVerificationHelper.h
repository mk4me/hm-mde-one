/********************************************************************
	created:  2014/08/06	10:02:30
	filename: HMDBServicesVerificationHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__HMDBSERVICESVERIFICATIONHELPER_H__
#define __HEADER_GUARD_hmdbCommunication__HMDBSERVICESVERIFICATIONHELPER_H__

namespace hmdbServices
{
	class ISingleAccountFactoryWS;
	class IMultiAccountFactoryWS;
	class IAdministrationWS;
	class IAuthorizationWS;
	class IGeneralBasicQueriesWS;
	class IMotionBasicQueriesWS;
	class IMotionBasicUpdatesWS;
	class IMedicalBasicUpdatesWS;
	class IMotionFileStoremanWS;
	class IMedicalFileStoremanWS;
	class IUserPersonalSpaceWS;
	class IFileStoremanWS;
}

namespace hmdbCommunication
{
	class IHMDBFtp;

	class HMDBServicesVerificationHelper
	{
	public:
		//! \param singleAccountFactory Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::ISingleAccountFactoryWS * singleAccountFactory);
		//! \param multiAccountFactory Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMultiAccountFactoryWS * multiAccountFactory);
		//! \param administration Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IAdministrationWS * administration);
		//! \param authorization Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IAuthorizationWS * authorization);
		//! \param generalQueries Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IGeneralBasicQueriesWS * generalQueries);
		//! \param motionQueries Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMotionBasicQueriesWS * motionQueries);
		//! \param motionUpdate Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMotionBasicUpdatesWS * motionUpdate);
		//! \param medicalUpdate Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate);
		//! \param motionFilestoreman Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMotionFileStoremanWS * motionFilestoreman);
		//! \param medicalFilestoreman Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman);
		//! \param userPersonalSpace Us�uga do weryfikacji
		//! \return Prawda je�eli us�uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IUserPersonalSpaceWS * userPersonalSpace);
		//! \param fileStoreman Us�uga plik�w
		//! \param ftp Dost�p do plik�w
		//! \return Prawda je�eli poprawnie zweryfikowano dost�p do plik�w
		static const bool verify(hmdbServices::IFileStoremanWS * fileStoreman, IHMDBFtp * ftp);
	};
}

#endif	// __HEADER_GUARD_hmdbCommunication__HMDBSERVICESVERIFICATIONHELPER_H__
