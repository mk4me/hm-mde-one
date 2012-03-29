/********************************************************************
    created:  2012/03/05
    created:  5:3:2012   21:16
    filename: IFtpsConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFTPSCONNECTION_H__
#define HEADER_GUARD___IFTPSCONNECTION_H__

#include <webserviceslib/IWSCredentials.h>
#include <boost/shared_ptr.hpp>

namespace webservices
{
	class IFtpsConnection : public IWSCredentials
	{
	public:

		//! Status operacji
		enum OperationStatus {
			Complete,   //! Zakoñczona sukcesem
			Error,      //! Zakoñczona b³êdem
			Cancelled   //! Anulowana
		};

		//! Interfejs do customowej obs³ugi progresu - metoda setProgress powinna byæ bardzo wydajna
		class IProgress
		{
		public:
			//! Wirtualny destruktor
			virtual ~IProgress() {}
			//! \param progress Postêp w zakresie od 0 do 100 w %
			virtual void setProgress(double progress) = 0;
			//! Pozwala wp³ywaæ na anulowanie zadania
			virtual const bool aborted() const
			{
				return false;
			}
		};

	public:

		virtual ~IFtpsConnection() {}

		virtual void setUrl(const std::string & url) = 0;

		virtual const std::string url() const = 0;

		/**
        Metoda wysy³aj¹ca na serwer plik. Nazwa wys³anego pliku przechowywana jest w zmiennej filename.
        @param localSource nazwa pliku który wysy³amy na serwer ftp
        @param remoteDestination miejsce zapisu naszego pliku na ftp
        @param customProgress customowy obiekt obs³uguj¹cy postêp operacji
        */
        virtual const OperationStatus put(const std::string & localSource, const std::string & remoteDestination, IProgress * customProgress = nullptr) = 0;
        /**
        Metoda pobieraj¹ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param remoteSource œcia¿ka pliku na serwerze ftp który chcemy œci¹gn¹æ
        @param localDestination œcia¿ka w której chcemy zpaisaæ œci¹gany plik
        @param customProgress customowy obiekt obs³uguj¹cy postêp operacji
        */
        virtual const OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr) = 0;
        /**
        Postêp operacji przesy³ania.
        @return postêp wyra¿ony w procentach dla pojedynczego transferu
        */
        virtual const int progress() const = 0;
        /**
        Anuluje obecnie wykonywan¹ operacjê. Czyœci zasoby uzyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort() = 0;
        //! \return Informacja o ostatnim b³êdzie
        virtual const std::string errorMessage() const = 0;
	};

	typedef boost::shared_ptr<IFtpsConnection> FtpsConnectionPtr;
	typedef boost::shared_ptr<const IFtpsConnection> FtpsConnectionConstPtr;
}

#endif	//	HEADER_GUARD___IFTPSCONNECTION_H__
