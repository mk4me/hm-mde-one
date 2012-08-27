/********************************************************************
    created:  2012/03/06
    created:  6:3:2012   7:51
    filename: DownloadHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DOWNLOADHELPER_H__
#define HEADER_GUARD___DOWNLOADHELPER_H__

#include <webserviceslib/IDownloadHelper.h>
#include <webserviceslib/IFileStoremanWS.h>

namespace webservices
{
	//! Klasa pomagająca pobierać pliki i zdjęcia
	class BasicDownloadHelper : public IDownloadHelper
	{
	public:
		
		BasicDownloadHelper();
		virtual ~BasicDownloadHelper();

		virtual IFtpsConnection::OperationStatus get(IFtpsConnection::IProgress * progress = nullptr);
		virtual void abort();
		virtual const int progress();
		virtual const std::string errorMessage();

		void setDownload(int fileID, const std::string & destinationPath);
		void configure(const BasicStoremanWSPtr & basicStoremanWS, const FtpsConnectionPtr & ftpsConnection);

	private:

		BasicStoremanWSPtr basicStoremanWS;
		FtpsConnectionPtr ftpsConnection;
		
		int fileID;
		std::string destinationPath;
		std::string errorMessage_;
	};

	//! Klasa pomagająca pobierać płytkie kopie danych
	class ShallowDownloadHelper : public IDownloadHelper
	{
	public:
		//! Typ pliku płytkiej kopii danych
		enum ShallowFile {
			ShallowData,	//! Plik płytkiej kopi danych
			ShallowMetadata	//! Plik płytkiej kopi metadanych
		};

	public:

		ShallowDownloadHelper();
		virtual ~ShallowDownloadHelper();

		virtual IFtpsConnection::OperationStatus get(IFtpsConnection::IProgress * progress = nullptr);
		virtual void abort();
		virtual const int progress();
		virtual const std::string errorMessage();

		void setDownload(ShallowFile shallowFile, const std::string & destinationPath);
		void configure(const ShallowStoremanWSPtr & shallowStoremanWS, const FtpsConnectionPtr & ftpsConnection);
	private:

		ShallowStoremanWSPtr shallowStoremanWS;
		FtpsConnectionPtr ftpsConnection;

		ShallowFile shallowFile;
		std::string destinationPath;
		std::string errorMessage_;
	};
}

#endif	//	HEADER_GUARD___DOWNLOADHELPER_H__
