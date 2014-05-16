/********************************************************************
	created:  2014/05/15	10:31:26
	filename: CURLManager.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__

#include <networkUtils/Export.h>
#include <utils/SmartPtr.h>
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
		//! Domyœlny konstruktor
		CURLManager();
		//! \param multi Uchwyt dla obs³ugiwanych po³¹czeñ
		explicit CURLManager(CURLM * multi);
		//! Destruktor wirtualny
		virtual ~CURLManager();
		//! \param curl Skonfigurowany uchwyt do obs³u¿enia
		void addRequest(CURL * curl);
		//! \param curl Uchwyt do usuniêcia
		void removeRequest(CURL * curl);
		//! \param paused Czy pauzujemy przetwarzanie danych
		void pause(const bool paused);
		//! \return Czy przetwarzanie danych wstrzymane
		const bool paused() const;
		//! Definitywnie koñczy przetwarzanie danych
		void finalize();
		//! \return Czy przetwarzanie danych zosta³o definitywnie przerwane
		const bool finalized() const;
		//! Metoda uruchomiowa w ramach w¹tku przetwarzaj¹cego uchwyty po³¹czeñ
		void processThread();

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
		virtual void onErrorRequest(CURL * curl) {}
		//! \param curl Uchwyt który zakoñczy³ przetwarzanie danych poprawnie
		virtual void onFinishRequest(CURL * curl) {}

	private:
		//! Obiekt implementuj¹cy funkcjonalnoœc klasy
		utils::shared_ptr<CURLManagerImpl> impl;
	};
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__