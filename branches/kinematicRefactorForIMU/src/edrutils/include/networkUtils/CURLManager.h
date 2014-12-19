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
#include <future>
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
		//! Domy�lny konstruktor
		CURLManager();
		//! \param multi Uchwyt dla obs�ugiwanych po��cze�
		explicit CURLManager(CURLM * multi);
		//! Destruktor wirtualny
		virtual ~CURLManager();
		//! \param curl Skonfigurowany uchwyt do obs�u�enia
		//! \param wait Obiekt pozwalaj�cy zaczeka� na zako��enie operacji curla
		std::future<CURLcode> addRequest(CURL * curl);
		//! \param curl Uchwyt do usuni�cia
		void removeRequest(CURL * curl);
		//! Metoda wo�ana w ramach w�tku przetwarzaj�cego uchwyty po��cze�
		const bool process();
		//! Metoda konczaca przetwarzanie
		void finalize();

	private:
		//! Metoda wo�ana zaraz po zako�czeniu pracy z danymi przez po��czenia
		virtual void onProcess() {}
		//! \param curl Uchwyt kt�ry w�a�nie dodano
		virtual void onAddRequest(CURL * curl) {}
		//! \param curl Uchwyt kt�ry w�a�nie usuni�to
		virtual void onRemoveRequest(CURL * curl) {}
		//! \param curl Uchwyt kt�ry zosta� anulowany
		virtual void onCancelRequest(CURL * curl) {}
		//! \param curl Uchwyt kt�ry spowodowa� b��d
		//! \param error Opis b��du
		virtual void onErrorRequest(CURL * curl, const std::string & error) {}
		//! \param curl Uchwyt kt�ry zako�czy� przetwarzanie danych poprawnie
		virtual void onFinishRequest(CURL * curl) {}

	private:
		//! Obiekt implementuj�cy funkcjonalno�c klasy
		utils::shared_ptr<CURLManagerImpl> impl;
	};

	DEFINE_SMART_POINTERS(CURLManager);
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__