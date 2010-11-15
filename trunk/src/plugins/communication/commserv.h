#ifndef COMMSERV_H
#define COMMSERV_H

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
//#include <QtXml/QDomDocument>
#include <map>
#include "SettingsDialog.h"
#include "core/CommunicationService.h"

class CommServ : public QDialog {

public:
	CommServ(QWidget* parent = 0);
	virtual ~CommServ();
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
	Q_OBJECT
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
	EDRCM::CommunicationService* commServ;
	EDRCM::TransportWSDL_FTPS* transportManager;
	EDRCM::QueryWSDL* queryManager;
	bool validateQuerySettings();
	std::vector<EDRCM::Session>* sessions;
	std::vector<EDRCM::Trial>* trials;
	std::vector<EDRCM::File>* files;
	QString session_folder;
	QString trial_folder;
//	void SessionDetailsWithAttributesParser(const std::string& data);
//	void parseSession(const QDomElement &element);
};
#endif
