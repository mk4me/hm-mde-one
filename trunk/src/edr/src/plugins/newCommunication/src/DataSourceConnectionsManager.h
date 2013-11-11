/********************************************************************
    created:  2012/03/07
    created:  7:3:2012   13:29
    filename: DataSourceConnectionsManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
#define HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__

#include <webserviceslib/WSConnection.h>
#include <webserviceslib/FtpsConnection.h>

//! Klasa agregująca połączenia webservices - singletone
class DataSourceConnectionManager
{
public:
	//! \return Tworzy innstancję managera połączeń
	static DataSourceConnectionManager * create();
	//! Niszczy instancje managera połączeń
	static void destroy();

	//! \return Instancja managera połączeń
	static DataSourceConnectionManager * instance();

	//! \return Połączenie z usługą FileStoremanWS dla danych ruchu
	const webservices::WSSecureConnectionPtr & motionFileStoremanWSConnection() const;
	//! \return Połączenie z usługą BasicQueriesWS dla danych ruchu
	const webservices::WSSecureConnectionPtr & motionBasicQueriesWSConnection() const;
	//! \return Połączenie z usługą BasicUpdatesWS dla danych ruchu
	const webservices::WSSecureConnectionPtr & motionBasicUpdatesWSConnection() const;

	//! \return Połączenie z usługą FileStoremanWS dla danych medycznych
	const webservices::WSSecureConnectionPtr & medicalFileStoremanWSConnection() const;
	//! \return Połączenie z usługą BasicQueriesWS dla danych medycznych
	const webservices::WSSecureConnectionPtr & medicalBasicQueriesWSConnection() const;
	//! \return Połączenie z usługą BasicUpdatesWS dla danych medycznych
	const webservices::WSSecureConnectionPtr & medicalBasicUpdatesWSConnection() const;


	//! \return Połączenie z usługą AcvcountFactoryWS
	const webservices::WSSecureConnectionPtr & accountFactoryWSConnection() const;
	//! \return Połączenie z usługą UserPersonalSpaceWS
	const webservices::WSSecureConnectionPtr & userPersonalSpaceWSConnection() const;
	//! \return Połączenie z usługą AdministrationWS
	const webservices::WSSecureConnectionPtr & administrationWSConnection() const;
	//! \return Połączenie z usługą AuthorizationWS
	const webservices::WSSecureConnectionPtr & authorizationWSConnection() const;

	//! \return Połączenie ftps z danymi ruchu
	const webservices::FtpsConnectionPtr & motionFtps() const;
	//! \return Połączenie ftps z danymi medycznymi
	const webservices::FtpsConnectionPtr & medicalFtps() const;

private:
	//! Prywatny domyślny konstruktor
	DataSourceConnectionManager();
	//! Prywatny destruktor
	~DataSourceConnectionManager();

private:

	webservices::WSSecureConnectionPtr motionFilesStoremanWSConnection_;
	webservices::WSSecureConnectionPtr motionBasicQueriesWSConnection_;
	webservices::WSSecureConnectionPtr motionBasicUpdatesWSConnection_;
				 
	webservices::WSSecureConnectionPtr medicalFilesStoremanWSConnection_;
	webservices::WSSecureConnectionPtr medicalBasicQueriesWSConnection_;
	webservices::WSSecureConnectionPtr medicalBasicUpdatesWSConnection_;
				 
	webservices::WSSecureConnectionPtr accountFactoryWSConnection_;
	webservices::WSSecureConnectionPtr userPersonalSpaceWSConnection_;
	webservices::WSSecureConnectionPtr administrationWSConnection_;
	webservices::WSSecureConnectionPtr authorizationWSConnection_;

	webservices::FtpsConnectionPtr motionFtps_;
	webservices::FtpsConnectionPtr medicalFtps_;

	static DataSourceConnectionManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
