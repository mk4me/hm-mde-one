/********************************************************************
    created:  2012/03/05
    created:  5:3:2012   21:29
    filename: IDownloadHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDOWNLOADHELPER_H__
#define HEADER_GUARD___IDOWNLOADHELPER_H__

#include <webserviceslib/IFtpsConnection.h>
#include <utils/SmartPtr.h>

namespace webservices
{
	//! Interfejs do pobierania danych z bazy wg zasad webserwises
	class IDownloadHelper
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDownloadHelper() {}
		//! Pobiera plik
		virtual const IFtpsConnection::OperationStatus get(IFtpsConnection::IProgress * progress = nullptr) = 0;
		//! \return Postęp pobieranie w przedziale [0-100]
		virtual const int progress() = 0;
		//! Przerywa aktualne pobieranie
		virtual void abort() = 0;
		//! \return Informacja o błędzie
		virtual const std::string errorMessage() = 0;
	};

	typedef utils::shared_ptr<IDownloadHelper> DownloadHelperPtr;
}

#endif	//	HEADER_GUARD___IDOWNLOADHELPER_H__
