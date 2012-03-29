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
#include <boost/shared_ptr.hpp>

namespace webservices
{
	//! Interfejs do pobierania danych z bazy wg zasad webserwisec
	class IDownloadHelper
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDownloadHelper() {}
		//! Pobiera plik
		virtual IFtpsConnection::OperationStatus get(IFtpsConnection::IProgress * progress = nullptr) = 0;
		//! \return Postêp pobieranie w przedziale [0-100]
		virtual const int progress() = 0;
		//! Przerywa aktualne pobieranie
		virtual void abort() = 0;
		//! \return Informacja o b³êdzie
		virtual const std::string errorMessage() = 0;
	};

	typedef boost::shared_ptr<IDownloadHelper> DownloadHelperPtr;
}

#endif	//	HEADER_GUARD___IDOWNLOADHELPER_H__
