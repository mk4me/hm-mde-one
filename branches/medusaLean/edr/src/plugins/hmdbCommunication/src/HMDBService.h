/********************************************************************
	created:  2014/05/30	15:27:20
	filename: HMDBService.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__HMDBSERVICE_H__
#define __HEADER_GUARD_hmdbCommunication__HMDBSERVICE_H__

#include <plugins/hmdbCommunication/IHMDBService.h>
#include <corelib/IService.h>
#include <corelib/IThreadPool.h>
#include "CURLFTPManager.h"
#include <networkUtils/CURLManager.h>
#define NOMINMAX
#include <xmlpull/XmlUtils.h>

namespace hmdbCommunication
{
	class HMDBService : public plugin::IService, public IHMDBService
	{

		UNIQUE_ID("{E8801231-BACA-42C6-9A8E-706F561A563F}")
		CLASS_DESCRIPTION("HMDB Service", "HMDB Service")

	public:

		static void curlEnableLog(CURL * curl);
		static void curlDisableLog(CURL * curl);		

		static const networkUtils::IWSDLServicePtr createSecureWSDL(
			const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		static const networkUtils::IWSDLServicePtr createUnsecureWSDL(
			const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		//! Konstruktor domy�lny
		HMDBService();
		//! Destruktor wirtualny
		virtual ~HMDBService();

		//! \param motionServicesUrl Adres us�ug danych ruchu
		//! \param medicalServicesUrl Adres us�ug danych medycznych
		//! \param user Nazwa uzytkownika
		//! \param password Has�o uzytkownika
		//! \param motionDataUrl Adres danych ruchu
		//! \param motionDataUrl Adres danych medycznych
		//! \return Sesja serwis�w danych ruchu, nullptr je�eli nie uda�o si� uwierzytelni�
		//! u�ytkownika w us�ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const std::string & motionServicesUrl,
			const std::string & medicalServicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & motionDataUrl,
			const std::string & medicalDataUrl,
			const utils::Filesystem::Path & CAPath,
			const networkUtils::SSLHostVerification hostVerification);

		//! \param motion Czy sesja tylko dla motion? Je�li false to tylko medical
		//! \param servicesUrl Adres us�ug danych ruchu		
		//! \param user Nazwa uzytkownika
		//! \param password Has�o uzytkownika
		//! \param dataUrl Adres danych ruchu		
		//! \return Sesja serwis�w danych ruchu, nullptr je�eli nie uda�o si� uwierzytelni�
		//! u�ytkownika w us�ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const bool motion,
			const std::string & servicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & dataUrl,
			const utils::Filesystem::Path & CAPath,
			const networkUtils::SSLHostVerification hostVerification);

		virtual const networkUtils::IWSDLServicePtr createHMDBService(
			const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		virtual const networkUtils::IWSDLServicePtr createHMDBSystemService(
			const std::string & url,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		//! \param url Adres serwera do pingowania
		//! \param caPath �cie�ka ewentualnego certyfikatu SSL je�eli wymagany
		//! \return Czy serwer odpowiedzia�
		virtual const bool serverOnline(const std::string & url,
			const unsigned int timeout = 250,
			const utils::Filesystem::Path & caPath = utils::Filesystem::Path()) const;

		//! \param session Sesja us�ug bazy danych ruchu kt�r� chcemy obs�ugiwa�
		virtual void attach(IHMDBSessionPtr session);

		//! \param session Sesja us�ug bazy danych ruchu kt�r� chcemy obs�ugiwa�
		virtual void detach(IHMDBSessionPtr session);

		//-----------------------------------------------------------------

		//! Inicjalizacja us�ugi. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich us�ug.
		virtual void init(core::ISourceManager * sourceManager,
			core::IVisualizerManager * visualizerManager,
			core::IDataManager * memoryDataManager,
			core::IStreamDataManager * streamDataManager,
			core::IFileDataManager * fileDataManager,
			core::IDataHierarchyManager * hierarchyManager);

		//! P�na inicjalizacja us�ug, nast�puje po wczytaniu i inicjalizacji wszystkich us�ug
		//! \return Czy us�uga poprawnie zainicjalizowana czy nie - mo�e brakuje innych us�ug zale�nych i sama nie ma jak dzia�a�
		//! Dla false us�uga zostanie wy�adowana
		virtual const bool lateInit();

		//! Metoda powinna w bezpieczny spos�b zwalniac zasoby, maj�c na uwadze �e niekoniecznie wszystkie us�ugi i zasoby pobrane z zewn�trz s� jeszcze dost�pne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, kt�re uruchomi� serwis, mo�e tez zwalniac pamie� przydzielon� dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezale�niaj�cy dana us�ug� od pozosta�ych us�ug i przygotowuj�cy ja na usuni�cie
		virtual void finalize();

		//! Metoda aktualizuj�ca pochodzi z w�tku UI! Powinny tu by� realizowane lekkie operacje od�wie�ania widget�w!!
		//! Jako parametr dostajemy przyrost czasu jaki mina� od poprzedniego wywo�ania
		virtual void update(double deltaTime);

		//! Us�uga nie musi mie� wizualnej reprezentacji.
		//! \return Widget tworzony przez us�ug� b�d� NULL.
		virtual QWidget* getWidget();

		//! Metoda dostarcza pomocniczych widget�w serwisu
		//! \return Lista pomocniczych widget�w serwisu
		virtual QWidgetList getPropertiesWidgets();

	private:
		//! Widget
		QWidget * mainWidget;
		//! W�tek us�ug
		core::Thread servicesThread;
		//! W�tek danych
		core::Thread dataThread;
		//! Manager us�ug
		networkUtils::CURLManagerPtr servicesManager;
		//! Manager danych
		utils::shared_ptr<CURLFTPManager> dataManager;

		utils::shared_ptr<XmlUtils::CURLExecutor> serviceCurlExecutor;

		//! Obiekt synchronizuj�cy
		std::recursive_mutex sync_;
	};
}

#endif	// __HEADER_GUARD_hmdbCommunication__HMDBSERVICE_H__
