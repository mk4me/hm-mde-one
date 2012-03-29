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

//! Klasa agreguj¹ca po³¹czenia webservices - singletone
class DataSourceConnectionManager
{
public:
	//! \return Tworzy innstancjê managera po³¹czeñ
	static DataSourceConnectionManager * create();
	//! Niszczy instancje managera po³¹czeñ
	static void destroy();

	//! \return Instancja managera po³¹czeñ
	static DataSourceConnectionManager * instance();

	//! \return Po³¹czenie z us³ug¹ FileStoremanWS dla danych ruchu
	const WSConnectionPtr & motionFileStoremanWSConnection() const;
	//! \return Po³¹czenie z us³ug¹ BasicQueriesWS dla danych ruchu
	const WSConnectionPtr & motionBasicQueriesWSConnection() const;
	//! \return Po³¹czenie z us³ug¹ BasicUpdatesWS dla danych ruchu
	const WSConnectionPtr & motionBasicUpdatesWSConnection() const;

	//! \return Po³¹czenie z us³ug¹ FileStoremanWS dla danych medycznych
	const WSConnectionPtr & medicalFileStoremanWSConnection() const;
	//! \return Po³¹czenie z us³ug¹ BasicQueriesWS dla danych medycznych
	const WSConnectionPtr & medicalBasicQueriesWSConnection() const;
	//! \return Po³¹czenie z us³ug¹ BasicUpdatesWS dla danych medycznych
	const WSConnectionPtr & medicalBasicUpdatesWSConnection() const;

	//! \return Po³¹czenie z us³ug¹ UserPersonalSpaceWS
	const WSConnectionPtr & userPersonalSpaceWSConnection() const;
	//! \return Po³¹czenie z us³ug¹ AdministrationWS
	const WSConnectionPtr & administrationWSConnection() const;
	//! \return Po³¹czenie z us³ug¹ AuthorizationWS
	const WSConnectionPtr & authorizationWSConnection() const;

	//! \return Po³¹czenie ftps z danymi ruchu
	const webservices::FtpsConnectionPtr & motionFtps() const;
	//! \return Po³¹czenie ftps z danymi medycznymi
	const webservices::FtpsConnectionPtr & medicalFtps() const;

private:
	//! Prywatny domyœlny konstruktor
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

	WSConnectionPtr userPersonalSpaceWSConnection_;
	WSConnectionPtr administrationWSConnection_;
	WSConnectionPtr authorizationWSConnection_;

	webservices::FtpsConnectionPtr motionFtps_;
	webservices::FtpsConnectionPtr medicalFtps_;

	static DataSourceConnectionManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCECONNECTIONSMANAGER_H__
