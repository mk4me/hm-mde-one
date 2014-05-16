/********************************************************************
	created:  2014/05/14	20:30:45
	filename: FTPConnection.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__

#include <networkUtils/IFtpOperations.h>
#include <networkUtils/Types.h>
#include <curl/curl.h>

namespace networkUtils
{
	class NETWORKUTILS_EXPORT CURLFTPProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~CURLFTPProgress() {}
		//! \return Czy nale�y przerwa� operacj�
		virtual const bool aborted() const = 0;
		//! \param progress Post�p od 0.0 do 1.0
		virtual void setProgress(const double progress) = 0;
	};

	class NETWORKUTILS_EXPORT CURLFTPHelper
	{
	public:
		//! \param curl Uchwyt do po��czenia z serwerem
		//! \param user Nazwa uzytkownika jak� ��czymy si� z serwerem
		//! \param password Has�o uzytkownika jakim ��czymy si� z serwerem
		//! \param workingDirectory Katalog startowy wzgl�dem homeDirecotry
		//! \param hostVerification Spos�b weryfikacji serwera
		//! \param caPath �cie�ka certyfikatu jakim weryfikujemy serwer
		static void initializeFTPConnection(CURL * curl,
			const std::string & user = std::string(),
			const std::string & password = std::string(),
			const HostVerification hostVerification = HVNone,
			const std::string & caPath = std::string());

		//!	Metoda pobieraj�ca plik z serwera.
		//! \param curl Uchwyt do po��czenia z serwerem
		//! \param remoteFileUrl Adres serwera i pliku kt�ry �ci�gamy
		//! \param localDestination Strumie� w kt�rym zpaisujemy �ci�gany plik
		//! \param progress Obiekt informowany o post�pie �ci�gania, steruje przerwaniem operacji
		static void initializeFTPDownload(CURL * curl,
			const std::string & remoteFileUrl,
			std::ostream * localDestination,
			CURLFTPProgress * progress = nullptr);

		//!	Metoda �aduj�ca plik na serwer.
		//! \param curl Uchwyt do po��czenia z serwerem
		//! \param remoteFileUrl Adres serwera i pliku kt�ry �adujemy
		//! \param localSource Strumie� z kt�rego czytamy zawarto�� pliku do wys�ania
		//! \param progress Obiekt informowany o post�pie wysy�ania, steruje przerwaniem operacji
		static void initializeFTPUpload(CURL * curl,
			const std::string & remoteFileUrl,
			std::istream * localSource,
			CURLFTPProgress * progress = nullptr);
	};
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__