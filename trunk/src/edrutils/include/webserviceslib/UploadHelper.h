/********************************************************************
    created:  2013/11/06
    created:  6:11:2013   14:23
    filename: UploadHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___UPLOADHELPER_H__
#define HEADER_GUARD___UPLOADHELPER_H__

#include <webserviceslib/IUploadHelper.h>
#include <webserviceslib/IFileStoremanWS.h>

namespace webservices
{
	//! Klasa pomagaj¹ca pobieraæ pliki i zdjêcia
	class BasicUploadHelper : public IUploadHelper
	{
	public:

		BasicUploadHelper();
		virtual ~BasicUploadHelper();

		virtual const IFtpsConnection::OperationStatus put(IFtpsConnection::IProgress * progress = nullptr);
		virtual void abort();
		virtual const int progress();
		virtual const std::string errorMessage();

		void setFileUpload(const std::string & sourcePath, const std::string & destinationPath);
		void setFolderUpload(const std::string & sourcePath, const std::string & destinationPath);
        void configure(FileStoremanWSPtr storeman, const FtpsConnectionPtr & ftpsConnection);

	private:
		FtpsConnectionPtr ftpsConnection;
		bool isFile;
		std::string sourcePath;
		std::string destinationPath;
		std::string errorMessage_;
        FileStoremanWSPtr storeman;
	};
}

#endif	//	HEADER_GUARD___UPLOADHELPER_H__
