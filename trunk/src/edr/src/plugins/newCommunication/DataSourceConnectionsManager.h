/********************************************************************
    created:  2012/03/07
    created:  7:3:2012   13:29
    filename: DataSourceConnectionsManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
#define HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__

#include "WSConnection.h"
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
	const WSConnectionPtr & motionFileStoremanWSConnection() const;
	//! \return Połączenie z usługą BasicQueriesWS dla danych ruchu
	const WSConnectionPtr & motionBasicQueriesWSConnection() const;
	//! \return Połączenie z usługą BasicUpdatesWS dla danych ruchu
	const WSConnectionPtr & motionBasicUpdatesWSConnection() const;

	//! \return Połączenie z usługą FileStoremanWS dla danych medycznych
	const WSConnectionPtr & medicalFileStoremanWSConnection() const;
	//! \return Połączenie z usługą BasicQueriesWS dla danych medycznych
	const WSConnectionPtr & medicalBasicQueriesWSConnection() const;
	//! \return Połączenie z usługą BasicUpdatesWS dla danych medycznych
	const WSConnectionPtr & medicalBasicUpdatesWSConnection() const;


	//! \return Połączenie z usługą AcvcountFactoryWS
	const WSConnectionPtr & accountFactoryWSConnection() const;
	//! \return Połączenie z usługą UserPersonalSpaceWS
	const WSConnectionPtr & userPersonalSpaceWSConnection() const;
	//! \return Połączenie z usługą AdministrationWS
	const WSConnectionPtr & administrationWSConnection() const;
	//! \return Połączenie z usługą AuthorizationWS
	const WSConnectionPtr & authorizationWSConnection() const;

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

	WSConnectionPtr motionFilesStoremanWSConnection_;
	WSConnectionPtr motionBasicQueriesWSConnection_;
	WSConnectionPtr motionBasicUpdatesWSConnection_;

	WSConnectionPtr medicalFilesStoremanWSConnection_;
	WSConnectionPtr medicalBasicQueriesWSConnection_;
	WSConnectionPtr medicalBasicUpdatesWSConnection_;

	WSConnectionPtr accountFactoryWSConnection_;
	WSConnectionPtr userPersonalSpaceWSConnection_;
	WSConnectionPtr administrationWSConnection_;
	WSConnectionPtr authorizationWSConnection_;

	webservices::FtpsConnectionPtr motionFtps_;
	webservices::FtpsConnectionPtr medicalFtps_;

	static DataSourceConnectionManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
