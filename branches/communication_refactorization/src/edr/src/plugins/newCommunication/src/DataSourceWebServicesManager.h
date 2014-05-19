/********************************************************************
	created:  2012/03/07
	created:  7:3:2012   13:45
	filename: DataSourceWebServicesManager.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEWEBSERVICESMANAGER_H__
#define HEADER_GUARD___DATASOURCEWEBSERVICESMANAGER_H__

#include <hmdbserviceslib/IAdministrationWS.h>
#include <hmdbserviceslib/IAuthorizationWS.h>
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include <hmdbserviceslib/IBasicUpdatesWS.h>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <hmdbserviceslib/IUserPersonalSpaceWS.h>
#include <hmdbserviceslib/IAccountFactoryWS.h>

class DataSourceWebServicesManager
{
public:

	static DataSourceWebServicesManager * create();
	static void destroy();

	static DataSourceWebServicesManager * instance();

	const hmdbServices::MotionBasicQueriesWSPtr & motionBasicQueriesService() const;
	const hmdbServices::MedicalBasicQueriesWSPtr & medicalBasicQueriesService() const;

	const hmdbServices::MotionBasicUpdatesWSPtr & motionBasicUpdatesService() const;
	const hmdbServices::MedicalBasicUpdatesWSPtr & medicalBasicUpdatesService() const;

	const hmdbServices::MotionFileStoremanWSPtr & motionFileStoremanService() const;
	const hmdbServices::MedicalFileStoremanWSPtr & medicalFileStoremanService() const;

	const hmdbServices::MultiAccountFactoryWSPtr & accountFactoryService() const;
	const hmdbServices::AuthorizationWSPtr & authorizationService() const;
	const hmdbServices::AdministrationWSPtr & administrationService() const;
	const hmdbServices::UserPersonalSpaceWSPtr & userPersonalSpaceService() const;

private:

	DataSourceWebServicesManager();
	~DataSourceWebServicesManager();

	static DataSourceWebServicesManager * instance_;

	hmdbServices::MotionBasicQueriesWSPtr motionBasicQueriesService_;
	hmdbServices::MedicalBasicQueriesWSPtr medicalBasicQueriesService_;

	hmdbServices::MotionBasicUpdatesWSPtr motionBasicUpdatesService_;
	hmdbServices::MedicalBasicUpdatesWSPtr medicalBasicUpdatesService_;

	hmdbServices::MotionFileStoremanWSPtr motionFileStoremanService_;
	hmdbServices::MedicalFileStoremanWSPtr medicalFileSoremanService_;

	hmdbServices::MultiAccountFactoryWSPtr accountFactoryService_;
	hmdbServices::AuthorizationWSPtr authorizationService_;
	hmdbServices::AdministrationWSPtr administrationService_;
	hmdbServices::UserPersonalSpaceWSPtr userPersonalSpaceService_;
};

#endif	//	HEADER_GUARD___DATASOURCEWEBSERVICESMANAGER_H__
