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
			Complete,   //! Zakończona sukcesem
			Error,      //! Zakończona błędem
			Cancelled   //! Anulowana
		};

		//! Interfejs do customowej obsługi progresu - metoda setProgress powinna być bardzo wydajna
		class IProgress
		{
		public:
			//! Wirtualny destruktor
			virtual ~IProgress() {}
			//! \param progress Postęp w zakresie od 0 do 100 w %
			virtual void setProgress(double progress) = 0;
			//! Pozwala wpływać na anulowanie zadania
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
        Metoda wysyłająca na serwer plik. Nazwa wysłanego pliku przechowywana jest w zmiennej filename.
        @param localSource nazwa pliku który wysyłamy na serwer ftp
        @param remoteDestination miejsce zapisu naszego pliku na ftp
        @param customProgress customowy obiekt obsługujący postęp operacji
        */
        virtual const OperationStatus put(const std::string & localSource, const std::string & remoteDestination, IProgress * customProgress = nullptr) = 0;
        /**
        Metoda pobierająca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param remoteSource ściażka pliku na serwerze ftp który chcemy ściągnąć
        @param localDestination ściażka w której chcemy zpaisać ściągany plik
        @param customProgress customowy obiekt obsługujący postęp operacji
        */
        virtual const OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr) = 0;
        /**
        Postęp operacji przesyłania.
        @return postęp wyrażony w procentach dla pojedynczego transferu
        */
        virtual const int progress() const = 0;
        /**
        Anuluje obecnie wykonywaną operację. Czyści zasoby użyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort() = 0;
        //! \return Informacja o ostatnim błędzie
        virtual const std::string errorMessage() const = 0;
	};

	typedef boost::shared_ptr<IFtpsConnection> FtpsConnectionPtr;
	typedef boost::shared_ptr<const IFtpsConnection> FtpsConnectionConstPtr;
}

#endif	//	HEADER_GUARD___IFTPSCONNECTION_H__
