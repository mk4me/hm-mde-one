#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/ResetPasswordWidget.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include "ui_ResetPasswordWidget.h"
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreCursorChanger.h>
#include <coreui/CorePopup.h>

ResetPasswordWidget::ResetPasswordWidget(const QString & defaultServerURL,
	QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::ResetPasswordWidget),
	defaultServerURL(defaultServerURL)
{
	ui->setupUi(this);

	//TODO
	//dodaæ validatory do url i email
	//walidowaæ domyslny jesli jest!!

	if (defaultServerURL.isEmpty() == true){
		ui->resetServiceURLPushButton->setEnabled(false);
	}
	else{
		resetServiceURL();
	}	
}

ResetPasswordWidget::~ResetPasswordWidget()
{

}

const bool ResetPasswordWidget::verify() const
{
	bool ret = true;
	//TODO
	return ret;
}

void ResetPasswordWidget::resetServiceURL()
{
	ui->serviceURLLineEdit->setText(defaultServerURL);
}

void ResetPasswordWidget::resetPassword()
{
	coreUI::CoreCursorChanger ccc;

	if (verify() == false){
		return;
	}

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			QString message;
			try{
				hmdbSource->resetPassword(ui->serviceURLLineEdit->text().toStdString(),
					ui->emailLineEdit->text().toStdString());

				ccc.restore();
				message = tr("Password reset successful");
			}
			catch (std::exception & e){
				message = tr("Password reset failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Password reset failed with UNKNOWN error");
			}

			coreUI::CorePopup::showMessage(tr("Password reset status"), message);
		}
		else{
			PLUGIN_LOG_ERROR("Could not find HMDBSource");
		}
	}
	else{
		PLUGIN_LOG_ERROR("Uninitialized source manager");
	}
}