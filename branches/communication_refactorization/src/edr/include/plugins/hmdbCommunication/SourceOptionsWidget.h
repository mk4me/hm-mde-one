/********************************************************************
	created:  2014/07/24	08:22:29
	filename: SourceOptionsWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__SOURCEOPTIONSWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__SOURCEOPTIONSWIDGET_H__

#include <QtWidgets/QWidget>

class QMainWindow;

namespace Ui
{
	class SourceOptionsWidget;
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

public slots:

	void onRefreshViews();

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

private:
	Ui::SourceOptionsWidget * ui;
	ServerStatusWidget * serverStatusWidget;
	QMainWindow * sourcesPlaceholder_;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SOURCEOPTIONSWIDGET_H__