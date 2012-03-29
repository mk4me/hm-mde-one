/********************************************************************
    created:  2012/03/07
    created:  7:3:2012   13:45
    filename: DataSourceWebServicesManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEWEBSERVICESMANAGER_H__
#define HEADER_GUARD___DATASOURCEWEBSERVICESMANAGER_H__

#include <webserviceslib/IAdministrationWS.h>
#include <webserviceslib/IAuthorizationWS.h>
#include <webserviceslib/IBasicQueriesWS.h>
#include <webserviceslib/IBasicUpdatesWS.h>
#include <webserviceslib/IFileStoremanWS.h>
#include <webserviceslib/IUserPersonalSpaceWS.h>

class DataSourceWebServicesManager
{
public:

    static DataSourceWebServicesManager * create();
    static void destroy();

    static DataSourceWebServicesManager * instance();

	const webservices::MotionBasicQueriesWSPtr & motionBasicQueriesService();
	const webservices::MedicalBasicQueriesWSPtr & medicalBasicQueriesService();

	const webservices::MotionBasicUpdatesWSPtr & motionBasicUpdatesService();
	const webservices::MedicalBasicUpdatesWSPtr & medicalBasicUpdatesService();

	const webservices::MotionFileStoremanWSPtr & motionFileStoremanService();
	const webservices::MedicalFileStoremanWSPtr & medicalFileStoremanService();

	const webservices::AuthorizationWSPtr & authorizationService();
	const webservices::AdministrationWSPtr & administrationService();
	const webservices::UserPersonalSpaceWSPtr & userPersonalSpaceService();

private:

	DataSourceWebServicesManager();
	~DataSourceWebServicesManager();

	static DataSourceWebServicesManager * instance_;

	webservices::MotionBasicQueriesWSPtr motionBasicQueriesService_; 
	webservices::MedicalBasicQueriesWSPtr medicalBasicQueriesService_;

	webservices::MotionBasicUpdatesWSPtr motionBasicUpdatesService_;
	webservices::MedicalBasicUpdatesWSPtr medicalBasicUpdatesService_;

	webservices::MotionFileStoremanWSPtr motionFileStoremanService_;
	webservices::MedicalFileStoremanWSPtr medicalFileSoremanService_;

	webservices::AuthorizationWSPtr authorizationService_;
	webservices::AdministrationWSPtr administrationService_;
	webservices::UserPersonalSpaceWSPtr userPersonalSpaceService_;
};

#endif	//	HEADER_GUARD___DATASOURCEWEBSERVICESMANAGER_H__
