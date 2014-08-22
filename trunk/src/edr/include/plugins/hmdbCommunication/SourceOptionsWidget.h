/********************************************************************
	created:  2014/07/24	08:22:29
	filename: SourceOptionsWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__SOURCEOPTIONSWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__SOURCEOPTIONSWIDGET_H__

#include <QtWidgets/QWidget>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>

class QMainWindow;

namespace Ui
{
	class SourceOptionsWidget;
}

namespace hmdbCommunication
{
	class IHMDBSession;
	class IHMDBFtp;
}

namespace hmdbServices
{
	class ISingleAccountFactoryWS;
	class IMultiAccountFactoryWS;
	class IAdministrationWS;
	class IAuthorizationWS;
	class IGeneralBasicQueriesWS;
	class IMotionBasicQueriesWS;
	class IMotionBasicUpdatesWS;
	class IMedicalBasicUpdatesWS;
	class IMotionFileStoremanWS;
	class IMedicalFileStoremanWS;
	class IUserPersonalSpaceWS;
	class IFileStoremanWS;
}

class ServerStatusWidget;

class SourceOptionsWidget : public QWidget
{
	Q_OBJECT

public:

	SourceOptionsWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~SourceOptionsWidget();

	void setSourcesPlaceholder(QMainWindow * placeholder);
	const QMainWindow * sourcesPlaceholder() const;

	const bool verify(QStringList & messages);

private:

	static void verify(hmdbCommunication::IHMDBSession * session, QStringList & errors);
	static void verify(hmdbServices::ISingleAccountFactoryWS * singleAccountFactory, QStringList & errors);
	static void verify(hmdbServices::IMultiAccountFactoryWS * multiAccountFactory, QStringList & errors);
	static void verify(hmdbServices::IAdministrationWS * administration, QStringList & errors);
	static void verify(hmdbServices::IAuthorizationWS * authorization, QStringList & errors);
	static void verify(hmdbServices::IGeneralBasicQueriesWS * generalQueries, QStringList & errors);
	static void verify(hmdbServices::IMotionBasicQueriesWS * motionQueries, QStringList & errors);
	static void verify(hmdbServices::IMotionBasicUpdatesWS * motionUpdate, QStringList & errors);
	static void verify(hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate, QStringList & errors);
	static void verify(hmdbServices::IMotionFileStoremanWS * motionFilestoreman, QStringList & errors);
	static void verify(hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman, QStringList & errors);
	static void verify(hmdbServices::IUserPersonalSpaceWS * userPersonalSpace, QStringList & errors);
	static void verify(hmdbServices::IFileStoremanWS * fileStoreman,
		hmdbCommunication::IHMDBFtp * ftp,
		const QString & message,
		QStringList & errors);

public slots:

	void onRefreshViews();
	void onRefreshConfigurations();
	void setLoginAdvanceConfiguration(bool show);

private slots:

	void onAddProfile();
	void onRemoveProfile();
	void onViewChange(int idx);
	void onProfileChange(int idx);
	void onStoragePathBrowse();
	void onServicesSSLCertificateBrowse();
	void onLogin();
	void onConnectionChanged(int idx);
	void onAccountOperationServiceSSLCertificateBrowse();	
	void onRegister();
	void onActivate();
	void onPasswordReset();
	void onServerChange(int idx);
	void onServerSSLCertificateBrowse();
	void onStatusCheck();
	void onSourceModeChange(int mode);
	void onVerifyLogin();
	void onConnectionProfileEdit();

private:

	void setConnectionProfile(const hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration & connectionProfile);

private:
	Ui::SourceOptionsWidget * ui;
	ServerStatusWidget * serverStatusWidget;
	QMainWindow * sourcesPlaceholder_;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SOURCEOPTIONSWIDGET_H__