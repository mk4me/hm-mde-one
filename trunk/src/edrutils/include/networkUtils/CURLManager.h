/********************************************************************
	created:  2014/05/15	10:31:26
	filename: CURLManager.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__

#include <networkUtils/Export.h>
#include <string>
#include <utils/SmartPtr.h>
#define NOMINMAX
#include <curl/curl.h>

namespace networkUtils
{
	class NETWORKUTILS_EXPORT CURLManager
	{
	private:
		//! Forward declaration
		class CURLManagerImpl;
		friend class CURLManagerImpl;

	public:

		//! Obiekt pozwalaj¹cy zaczekaæ na zakoñczenie zadania
		class NETWORKUTILS_EXPORT WaitCurl
		{
			friend class CURLManagerImpl;

		private:
			//! Forward declaration
			class WaitCurlImpl;

		public:
			//! Konstruktor domyslny
			WaitCurl();
			//! Destruktor
			~WaitCurl();
			//! Metoda czeka na zakoñczenie operacji curla
			void wait();
			//! \return Rezultat obs³ugi po³¹czenia
			const CURLcode result() const;
			//! \return Opis b³êdu jesli wyst¹pi³
			const std::string error() const;

		private:
			//! Implementacja
			utils::shared_ptr<WaitCurlImpl> impl;
		};

	public:
		//! Domyœlny konstruktor
		CURLManager();
		//! \param multi Uchwyt dla obs³ugiwanych po³¹czeñ
		explicit CURLManager(CURLM * multi);
		//! Destruktor wirtualny
		virtual ~CURLManager();
		//! \param curl Skonfigurowany uchwyt do obs³u¿enia
		//! \param wait Obiekt pozwalaj¹cy zaczekaæ na zakoñæenie operacji curla
		void addRequest(CURL * curl, WaitCurl * wait = nullptr);
		//! \param curl Uchwyt do usuniêcia
		void removeRequest(CURL * curl);
		//! Metoda wo³ana w ramach w¹tku przetwarzaj¹cego uchwyty po³¹czeñ
		const bool process();
		//! Metoda konczaca przetwarzanie
		void finalize();

	private:
		//! Metoda wo³ana zaraz po zakoñczeniu pracy z danymi przez po³¹czenia
		virtual void onProcess() {}
		//! \param curl Uchwyt który w³aœnie dodano
		virtual void onAddRequest(CURL * curl) {}
		//! \param curl Uchwyt który w³aœnie usuniêto
		virtual void onRemoveRequest(CURL * curl) {}
		//! \param curl Uchwyt który zosta³ anulowany
		virtual void onCancelRequest(CURL * curl) {}
		//! \param curl Uchwyt który spowodowa³ b³¹d
		//! \param error Opis b³êdu
		virtual void onErrorRequest(CURL * curl, const std::string & error) {}
		//! \param curl Uchwyt który zakoñczy³ przetwarzanie danych poprawnie
		virtual void onFinishRequest(CURL * curl) {}

	private:
		//! Obiekt implementuj¹cy funkcjonalnoœc klasy
		utils::shared_ptr<CURLManagerImpl> impl;
	};

	DEFINE_SMART_POINTERS(CURLManager);
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__