#include "CommunicationPCH.h"
#include "StatusWidget.h"
#include <QtCore/QTimer>
#include <QtGui/QToolTip>
#include <coreui/CorePopup.h>

communication::StatusWidget::StatusWidget(ServerStatusManagerConstWeakPtr statusManager, const std::string& pingUrl, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
    QWidget(parent, f),
    pingUrl(pingUrl),
    statusManager(statusManager)
{

    connectedPixmap = QPixmap(":/newCommunication/icons/dostepny.png").scaledToHeight(32, Qt::SmoothTransformation);
    disconnectedPixmap = QPixmap(":/newCommunication/icons/niedostepny.png").scaledToHeight(32, Qt::SmoothTransformation);
    setupUi(this);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(1000);

    setConnected(false);
}

void communication::StatusWidget::setConnected( bool connected )
{
    iconLabel->setPixmap(connected ? connectedPixmap : disconnectedPixmap);
}

void communication::StatusWidget::setUserName( const QString& val )
{
    userLabel->setText(tr("User: ") + val);
}

void communication::StatusWidget::updateTimer()
{
    // TODO : moze bedzie lepiej odpytywac o to w osobnym watku (serverStatus jest chroniony mutexem, moze to blokowac aplikacje)
    ServerStatusManagerConstPtr status = statusManager.lock();
    if (status) {
        ServerStatus ss = status->serverStatus(pingUrl);
        iconLabel->setPixmap(ss.online ? connectedPixmap : disconnectedPixmap);
        iconLabel->update();
    }
}

QPushButton* communication::StatusWidget::getLogoutButton()
{
    return this->logoutButton;
}

