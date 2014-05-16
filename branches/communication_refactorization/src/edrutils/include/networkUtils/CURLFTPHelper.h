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
		//! \return Czy nale¿y przerwaæ operacjê
		virtual const bool aborted() const = 0;
		//! \param progress Postêp od 0.0 do 1.0
		virtual void setProgress(const double progress) = 0;
	};

	class NETWORKUTILS_EXPORT CURLFTPHelper
	{
	public:
		//! \param curl Uchwyt do po³¹czenia z serwerem
		//! \param user Nazwa uzytkownika jak¹ ³¹czymy siê z serwerem
		//! \param password Has³o uzytkownika jakim ³¹czymy siê z serwerem
		//! \param workingDirectory Katalog startowy wzglêdem homeDirecotry
		//! \param hostVerification Sposób weryfikacji serwera
		//! \param caPath Œcie¿ka certyfikatu jakim weryfikujemy serwer
		static void initializeFTPConnection(CURL * curl,
			const std::string & user = std::string(),
			const std::string & password = std::string(),
			const HostVerification hostVerification = HVNone,
			const std::string & caPath = std::string());

		//!	Metoda pobieraj¹ca plik z serwera.
		//! \param curl Uchwyt do po³¹czenia z serwerem
		//! \param remoteFileUrl Adres serwera i pliku który œci¹gamy
		//! \param localDestination Strumieñ w którym zpaisujemy œci¹gany plik
		//! \param progress Obiekt informowany o postêpie œci¹gania, steruje przerwaniem operacji
		static void initializeFTPDownload(CURL * curl,
			const std::string & remoteFileUrl,
			std::ostream * localDestination,
			CURLFTPProgress * progress = nullptr);

		//!	Metoda ³aduj¹ca plik na serwer.
		//! \param curl Uchwyt do po³¹czenia z serwerem
		//! \param remoteFileUrl Adres serwera i pliku który ³adujemy
		//! \param localSource Strumieñ z którego czytamy zawartoœæ pliku do wys³ania
		//! \param progress Obiekt informowany o postêpie wysy³ania, steruje przerwaniem operacji
		static void initializeFTPUpload(CURL * curl,
			const std::string & remoteFileUrl,
			std::istream * localSource,
			CURLFTPProgress * progress = nullptr);
	};
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__