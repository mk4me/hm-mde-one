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
			Complete,   //! Zako�czona sukcesem
			Error,      //! Zako�czona b��dem
			Cancelled   //! Anulowana
		};

		//! Interfejs do customowej obs�ugi progresu - metoda setProgress powinna by� bardzo wydajna
		class IProgress
		{
		public:
			//! Wirtualny destruktor
			virtual ~IProgress() {}
			//! \param progress Post�p w zakresie od 0 do 100 w %
			virtual void setProgress(double progress) = 0;
			//! Pozwala wp�ywa� na anulowanie zadania
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
        Metoda wysy�aj�ca na serwer plik. Nazwa wys�anego pliku przechowywana jest w zmiennej filename.
        @param localSource nazwa pliku kt�ry wysy�amy na serwer ftp
        @param remoteDestination miejsce zapisu naszego pliku na ftp
        @param customProgress customowy obiekt obs�uguj�cy post�p operacji
        */
        virtual const OperationStatus put(const std::string & localSource, const std::string & remoteDestination, IProgress * customProgress = nullptr) = 0;
        /**
        Metoda pobieraj�ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param remoteSource �cia�ka pliku na serwerze ftp kt�ry chcemy �ci�gn��
        @param localDestination �cia�ka w kt�rej chcemy zpaisa� �ci�gany plik
        @param customProgress customowy obiekt obs�uguj�cy post�p operacji
        */
        virtual const OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr) = 0;
        /**
        Post�p operacji przesy�ania.
        @return post�p wyra�ony w procentach dla pojedynczego transferu
        */
        virtual const int progress() const = 0;
        /**
        Anuluje obecnie wykonywan� operacj�. Czy�ci zasoby uzyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort() = 0;
        //! \return Informacja o ostatnim b��dzie
        virtual const std::string errorMessage() const = 0;
	};

	typedef boost::shared_ptr<IFtpsConnection> FtpsConnectionPtr;
	typedef boost::shared_ptr<const IFtpsConnection> FtpsConnectionConstPtr;
}

#endif	//	HEADER_GUARD___IFTPSCONNECTION_H__
