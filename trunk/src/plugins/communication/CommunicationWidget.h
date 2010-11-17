#ifndef COMMUNICATION_WIDGET_H
#define COMMUNICATION_WIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
//#include <QtXml/QDomDocument>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/CommunicationService.h>
#include "SettingsDialog.h"

class CommunicationService;

class CommunicationWidget : public QWidget, public utils::Observer<communication::CommunicationManager> {
	Q_OBJECT
public:
	CommunicationWidget(CommunicationService* service);
	virtual ~CommunicationWidget();

    virtual void update(const communication::CommunicationManager* subject);
	void writeSettings();
	void readSettings();

	private slots:
		void sessionsTableDoubleClicked(int, int);
		void trialsTableDoubleClicked(int, int);
		void filesTableDoubleClicked(int, int);
		void downloadButtonClicked();
		void optionsButtonClicked();
		void settingsDialogAccepted();
		void sessionButtonClicked();

private:
	const static QStringList sessionsHeader;
	const static QStringList trialHeader;
	const static QStringList fileHeader;

	CommunicationService* m_service;

	QPushButton* m_download_button;
	QPushButton* m_options_button;
	QPushButton* m_list_lab_sessions_button;
	QTableWidget* m_sessions_table;
	QTableWidget* m_trials_table;
	QTableWidget* m_files_table;
	QTabWidget* m_table_tabs;
	SettingsDialog* m_settings_dialog;
	void setTableView(QTableWidget*, const QStringList&);
};
#endif
