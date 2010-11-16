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
#include <plugins/communication/CommunicationManager.h>
#include "SettingsDialog.h"

class CommunicationWidget : public QWidget {
	Q_OBJECT
public:
	CommunicationWidget();
	virtual ~CommunicationWidget();
	void writeSettings();
	void readSettings();
	void setTableView(QTableWidget*, const QStringList&);
	void getSessions();
	void getSessionTrials();
	void getTrialFiles();

	private slots:
		void selectSession(int, int);
		void selectTrial(int, int);
		void selectFile(int, int);
		void downloadSelected();
		void options();
		void setChangedSettings();
		void sessionButtonClicked();

private:
	const static QStringList sessionsHeader;
	const static QStringList trialHeader;
	const static QStringList fileHeader;
	QPushButton* downloadSelectedButton;
	QPushButton* optionsButton;
	QPushButton* sessionButton;
	QTableWidget* sessionTable;
	QTableWidget* trialTable;
	QTableWidget* fileTable;
	QLabel* trialLabel;
	QLabel* fileLabel;
	QTabWidget* tabs;
	SettingsDialog* settings_dialog;
	communication::CommunicationManager* commServ;
	communication::TransportWSDL_FTPS* transportManager;
	communication::QueryWSDL* queryManager;
	bool validateQuerySettings();
	std::vector<communication::Session>* sessions;
	std::vector<communication::Trial>* trials;
	std::vector<communication::File>* files;
	QString session_folder;
	QString trial_folder;
//	void SessionDetailsWithAttributesParser(const std::string& data);
//	void parseSession(const QDomElement &element);
};
#endif
