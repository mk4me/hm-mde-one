/********************************************************************
    created:  2013/11/06
    created:  6:11:2013   14:22
    filename: IUploadHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IUPLOADHELPER_H__
#define HEADER_GUARD___IUPLOADHELPER_H__

#include <webserviceslib/IFtpsConnection.h>
#include <utils/SmartPtr.h>

namespace webservices
{
	//! Interfejs do ³adowania danych do bazy wg zasad webserwises
	class IUploadHelper
	{
	public:
		//! Destruktor wirtualny
		virtual ~IUploadHelper() {}
		//! Pobiera plik
		virtual const IFtpsConnection::OperationStatus put(IFtpsConnection::IProgress * progress = nullptr) = 0;
		//! \return Postêp pobieranie w przedziale [0-100]
		virtual const int progress() = 0;
		//! Przerywa aktualne pobieranie
		virtual void abort() = 0;
		//! \return Informacja o b³êdzie
		virtual const std::string errorMessage() = 0;
	};

	typedef utils::shared_ptr<IUploadHelper> UploadHelperPtr;
}

#endif	//	HEADER_GUARD___IUPLOADHELPER_H__
