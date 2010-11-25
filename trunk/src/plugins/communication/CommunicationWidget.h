#ifndef COMMUNICATION_WIDGET_H
#define COMMUNICATION_WIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/CommunicationService.h>
#include "SettingsDialog.h"

class CommunicationService;

class CommunicationWidget : public QWidget, public utils::Observer<communication::CommunicationManager>
{
	Q_OBJECT
public:
	CommunicationWidget(CommunicationService* service);
	virtual ~CommunicationWidget();

    virtual void update(const communication::CommunicationManager* subject);

	private slots:
		void updateButtonClicked();
		void itemDoubleClicked(QTreeWidgetItem*, int);

private:
	CommunicationService* communicationService;

	QPushButton* updateButton;
	QTreeWidget* treeEntities;
};
#endif
