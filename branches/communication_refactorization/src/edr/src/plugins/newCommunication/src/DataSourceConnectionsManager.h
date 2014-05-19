/********************************************************************
	created:  2012/03/07
	created:  7:3:2012   13:29
	filename: DataSourceConnectionsManager.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
#define HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__

//#include <hmdbserviceslib/WSConnection.h>
//#include <hmdbserviceslib/FtpsConnection.h>

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
	const hmdbServices::WSSecureConnectionPtr & motionFileStoremanWSConnection() const;
	//! \return Połączenie z usługą BasicQueriesWS dla danych ruchu
	const hmdbServices::WSSecureConnectionPtr & motionBasicQueriesWSConnection() const;
	//! \return Połączenie z usługą BasicUpdatesWS dla danych ruchu
	const hmdbServices::WSSecureConnectionPtr & motionBasicUpdatesWSConnection() const;

	//! \return Połączenie z usługą FileStoremanWS dla danych medycznych
	const hmdbServices::WSSecureConnectionPtr & medicalFileStoremanWSConnection() const;
	//! \return Połączenie z usługą BasicQueriesWS dla danych medycznych
	const hmdbServices::WSSecureConnectionPtr & medicalBasicQueriesWSConnection() const;
	//! \return Połączenie z usługą BasicUpdatesWS dla danych medycznych
	const hmdbServices::WSSecureConnectionPtr & medicalBasicUpdatesWSConnection() const;

	//! \return Połączenie z usługą AcvcountFactoryWS
	const hmdbServices::WSSecureConnectionPtr & accountFactoryWSConnection() const;
	//! \return Połączenie z usługą UserPersonalSpaceWS
	const hmdbServices::WSSecureConnectionPtr & userPersonalSpaceWSConnection() const;
	//! \return Połączenie z usługą AdministrationWS
	const hmdbServices::WSSecureConnectionPtr & administrationWSConnection() const;
	//! \return Połączenie z usługą AuthorizationWS
	const hmdbServices::WSSecureConnectionPtr & authorizationWSConnection() const;

	//! \return Połączenie ftps z danymi ruchu
	const hmdbServices::FtpsConnectionPtr & motionFtps() const;
	//! \return Połączenie ftps z danymi medycznymi
	const hmdbServices::FtpsConnectionPtr & medicalFtps() const;

private:
	//! Prywatny domyślny konstruktor
	DataSourceConnectionManager();
	//! Prywatny destruktor
	~DataSourceConnectionManager();

private:

	hmdbServices::WSSecureConnectionPtr motionFilesStoremanWSConnection_;
	hmdbServices::WSSecureConnectionPtr motionBasicQueriesWSConnection_;
	hmdbServices::WSSecureConnectionPtr motionBasicUpdatesWSConnection_;

	hmdbServices::WSSecureConnectionPtr medicalFilesStoremanWSConnection_;
	hmdbServices::WSSecureConnectionPtr medicalBasicQueriesWSConnection_;
	hmdbServices::WSSecureConnectionPtr medicalBasicUpdatesWSConnection_;

	hmdbServices::WSSecureConnectionPtr accountFactoryWSConnection_;
	hmdbServices::WSSecureConnectionPtr userPersonalSpaceWSConnection_;
	hmdbServices::WSSecureConnectionPtr administrationWSConnection_;
	hmdbServices::WSSecureConnectionPtr authorizationWSConnection_;

	hmdbServices::FtpsConnectionPtr motionFtps_;
	hmdbServices::FtpsConnectionPtr medicalFtps_;

	static DataSourceConnectionManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
