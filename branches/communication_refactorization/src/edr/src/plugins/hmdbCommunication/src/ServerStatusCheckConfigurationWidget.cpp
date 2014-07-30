#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/ServerStatusCheckConfigurationWidget.h>
#include <plugins/hmdbCommunication/ServerStatusWidget.h>
#include <plugins/hmdbCommunication/IHMDBService.h>
#include "ui_ServerStatusCheckConfigurationWidget.h"
#include <corelib/PluginCommon.h>
#include <corelib/IServiceManager.h>
#include <coreui/CoreCursorChanger.h>
#include <coreui/CorePopup.h>

ServerStatusCheckConfigurationWidget::ServerStatusCheckConfigurationWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::ServerStatusCheckConfigurationWidget)
{
	ui->setupUi(this);
	connect(ui->timeoutSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(timeoutMSChanged(int)));
}

ServerStatusCheckConfigurationWidget::~ServerStatusCheckConfigurationWidget()
{

}

const int ServerStatusCheckConfigurationWidget::tiemoutMS() const
{
	return ui->timeoutSpinBox->value();
}

void ServerStatusCheckConfigurationWidget::setTiemoutMS(const int timeoutMS)
{
	ui->timeoutSpinBox->setValue(timeoutMS);
}

void ServerStatusCheckConfigurationWidget::resetTimeoutMS()
{
	ui->timeoutSpinBox->setValue(350);
}

/*
void ServerStatusCheckConfigurationWidget::checkStatus()
{
	coreUI::CoreCursorChanger ccc;

	if (validate() == false){
		return;
	}

	char ret = ServerStatusWidget::UnknownStatus;

	auto sm = plugin::getServiceManager();
	if (sm != nullptr){
		auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService != nullptr){
			QString message;
			try{
				const auto response = hmdbService->serverOnline(ui->serverURLLineEdit->text().toStdString(),
					ui->timeoutSpinBox->value());

				ret = ((response == true) ? ServerStatusWidget::OnlineStatus : ServerStatusWidget::OfflineStatus);

				ccc.restore();

				message = tr("Server status: ") + ((response == true) ? tr("On line") : tr("Off line"));

				emit statusChanged(ret);
			}
			catch (std::exception & e){
				message = tr("Server status check failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Server status failed with UNKNOWN error");
			}

			coreUI::CorePopup::showMessage(tr("Server status check"), message);
		}
		else{
			PLUGIN_LOG_ERROR("Could not find HMDBService");
		}
	}
	else{
		PLUGIN_LOG_ERROR("Uninitialized service manager");
	}
}

*/