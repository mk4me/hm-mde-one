#ifndef COMMUNICATION_WIDGET_H
#define COMMUNICATION_WIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressBar>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QFormLayout>
#include <QtGui/QDialogButtonBox>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/CommunicationService.h>
#include <plugins/communication/Entity.h>

class CommunicationService;

class EntityTrialItem : public QListWidgetItem
{
private:
	int session;
	int id;
public:
	EntityTrialItem() {session = 0;};
	void setSession(int session) {this->session = session;};
	int getSession() {return session;};
	void setID(int ID) {this->id = ID;};
	int getID() {return id;};
};

class LocalTrialItem : public QListWidgetItem
{
private:
	std::string path;
	std::string name;
public:
	LocalTrialItem() {setTextColor(QColor(0, 0, 255));};
	void setPath(const std::string& path) {this->path = path;};
	const std::string& getPath() const {return path;};
	void setName(const std::string& name) {this->name = name;};
	const std::string& getName() const {return name;};
};

class CommunicationWidget : public QWidget, public utils::Observer<communication::CommunicationManager>
{
	Q_OBJECT
public:
	CommunicationWidget(CommunicationService* service);
	virtual ~CommunicationWidget();
	void setBusy(bool busy);
	bool getBusy();
	void setProgress(int value);
	void setInfoLabel(const std::string& info) {infoLabel->setText(QString::fromStdString(info));};
	void showErrorMessage(const std::string& error);
	void refresh();

    virtual void update(const communication::CommunicationManager* subject);

	private slots:
		void updateButtonClicked();
		void download();
		void itemDoubleClicked(QListWidgetItem*);
		void itemClicked(QListWidgetItem*);

private:
	CommunicationService* communicationService;
	OpenThreads::Mutex trialsMutex;

	std::vector<communication::Trial> serverTrials;
	std::vector<LocalTrial> localTrials;
	std::string infoText;
	bool updateView;

	void refreshUI();

	QPushButton* updateButton;
	QPushButton* downloadButton;
	QListWidget* trials;
	QProgressBar* progressBar;
	QLabel* aviableTrialsLabel;
	QLabel* infoLabel;
	bool busy;
};
#endif
