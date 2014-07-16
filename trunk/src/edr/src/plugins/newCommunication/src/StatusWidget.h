/********************************************************************
	created:	2014/01/16
	created:	16:1:2014   9:52
	filename: 	StatusWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COMMUNICATION__STATUSWIDGET_H__
#define HEADER_GUARD_COMMUNICATION__STATUSWIDGET_H__

#include <QtWidgets/QWidget>
#include <utils/SmartPtr.h>
#include "CommunicationManager.h"
#include "ui_StatusWidget.h"
#include "ServerStatusManager.h"
#include <QtCore/QTimer>

namespace communication {



class StatusWidget : public QWidget, private Ui::StatusWidget
{
    Q_OBJECT

public:
    StatusWidget(ServerStatusManagerConstWeakPtr statusManager, const std::string& pingUrl, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~StatusWidget();

    QPushButton* getLogoutButton();

	void stopUpdating();
public slots:
    void setConnected(bool connected);
    void setUserName(const QString& val);

private slots:
    void updateTimer();

private:
    QTimer timer;
    ServerStatusManagerConstWeakPtr statusManager;
    std::string pingUrl;
    QPixmap connectedPixmap;
    QPixmap disconnectedPixmap;
};
DEFINE_SMART_POINTERS(StatusWidget);
}

#endif
