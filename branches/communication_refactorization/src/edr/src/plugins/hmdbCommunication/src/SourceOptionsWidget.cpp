#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/SourceOptionsWidget.h>
#include "ui_SourceOptionsWidget.h"
#include <plugins/hmdbCommunication/ServerStatusWidget.h>
#include <plugins/hmdbCommunication/IHMDBService.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <coreui/CorePopup.h>
#include <coreui/CoreCursorChanger.h>
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>
#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include "SQLCipherStorage.h"
#include "ShallowCopyUtils.h"
#include <coreui/CoreDockWidget.h>

const bool verifyFileExistance(const QString & filePath)
{
	bool ret = true;

	if (filePath.isEmpty() == false){
		auto p = core::Filesystem::Path(filePath.toStdString());
		if (core::Filesystem::pathExists(p) == false || core::Filesystem::isRegularFile(p) == false){
			ret = false;
		}
	}

	return ret;
}

SourceOptionsWidget::SourceOptionsWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::SourceOptionsWidget),
	serverStatusWidget(new ServerStatusWidget), sourcesPlaceholder_(nullptr)
{
	ui->setupUi(this);

	auto l = new QHBoxLayout;
	l->addWidget(serverStatusWidget);

	ui->statusPlaceholderGroupBox->setLayout(l);
	onSourceModeChange(Qt::Unchecked);

	ui->loginTab->setEnabled(false);

	onRefreshViews();
}

SourceOptionsWidget::~SourceOptionsWidget()
{

}

void SourceOptionsWidget::setSourcesPlaceholder(QMainWindow * placeholder)
{
	sourcesPlaceholder_ = placeholder;
}

const QMainWindow * SourceOptionsWidget::sourcesPlaceholder() const
{
	return sourcesPlaceholder_;
}

void SourceOptionsWidget::onRefreshViews()
{
	ui->loginTab->setEnabled(false);

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr && hmdbSource->sourceContextViewPrototypesCount() > 0){
			ui->loginTab->setEnabled(true);
		}
	}
}

void SourceOptionsWidget::onAddProfile()
{

}

void SourceOptionsWidget::onRemoveProfile()
{

}

void SourceOptionsWidget::onViewChange(int idx)
{

}

void SourceOptionsWidget::onProfileChange(int idx)
{

}

void SourceOptionsWidget::onStoragePathBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Open storage"), ui->storagePathLineEdit->text().isEmpty() == false ? ui->storagePathLineEdit->text() : QString(), tr("Storage File (*.dat)"));

	ui->storagePathLineEdit->setText(sf);
}

void SourceOptionsWidget::onServicesSSLCertificateBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Chose service certificate"), ui->servicesCertificatePathLineEdit->text().isEmpty() == false ? ui->servicesCertificatePathLineEdit->text() : QString(), tr("Certificate File (*.crt)"));

	ui->servicesCertificatePathLineEdit->setText(sf);
}

void SourceOptionsWidget::onLogin()
{
	coreUI::CoreCursorChanger ccc;

	QString message;

	auto sm = plugin::getServiceManager();
	if (sm != nullptr){
		auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService != nullptr){
			QString message;
			try{

				if (ui->storagePathLineEdit->text().isEmpty() == true ||
					verifyFileExistance(ui->storagePathLineEdit->text()) == false){
					message = tr("Login operation failed. Storage file not exists or was not chosen.");
				}
				else{

					std::auto_ptr<hmdbCommunication::SQLCipherStorage> storage(new hmdbCommunication::SQLCipherStorage);

					storage->open(ui->storagePathLineEdit->text().toStdString(),
						ui->storagePasswordLineEdit->text().toStdString());

					if (storage->isOpened() == false){
						message = tr("Login operation failed. Could not open storage.");
					}
					else{

						std::auto_ptr<hmdbCommunication::IHMDBSession> session;

						if (ui->onlineModeCheckBox->isChecked() == true){

							session = std::auto_ptr<hmdbCommunication::IHMDBSession>(hmdbService->createSession(
								ui->motionServiceURLLineEdit->text().toStdString(),
								ui->medicalDataURLLineEdit->text().toStdString(),
								ui->loginLineEdit->text().toStdString(),
								ui->passwordLineEdit->text().toStdString(),
								ui->motionDataURLLineEdit->text().toStdString(),
								ui->medicalDataURLLineEdit->text().toStdString(),
								ui->servicesCertificatePathLineEdit->text().toStdString(),
								networkUtils::HVMatch));
						}

						auto sm = plugin::getSourceManager();
						if (sm != nullptr){
							auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
							if (hmdbSource != nullptr){								
								try{
									auto sc = hmdbSource->createSourceContext(session.release(), storage.release(),
										ui->loginLineEdit->text().toStdString(),
										hmdbCommunication::ShallowCopyUtils::userHash(ui->loginLineEdit->text().toStdString(),
										ui->passwordLineEdit->text().toStdString()));
									
									//utworzenie widgeta aktualnie wybranego widoku z nowym kontekstem
									const auto idx = ui->viewComboBox->currentIndex();
									auto scvp = hmdbSource->sourceContextViewPrototype(idx);
									if (scvp != nullptr){
										auto view = scvp->createView(sc);
										if (view != nullptr){
											auto dw = coreUI::CoreDockWidget::embeddWidget(view,
												scvp->name() + "@" + ui->loginLineEdit->text(),
												Qt::AllDockWidgetAreas, true);

											if (sourcesPlaceholder_ != nullptr){
												sourcesPlaceholder_->addDockWidget(Qt::LeftDockWidgetArea, dw);
											}
											else{
												dw->setFloating(true);
												dw->setVisible(true);
											}

											message = tr("Logged successfully");
										}
									}
								}
								catch (std::exception & e){
									message = tr("Login operation failed with the following error: ") + QString::fromStdString(e.what());
								}
								catch (...){
									message = tr("Login operation failed with UNKNOWN error");
								}
							}
							else{
								message = tr("Login operation failed. Could not find HMDBSource.");
							}
						}
						else{
							message = tr("Login operation failed. Uninitialized source manager.");
						}
					}
				}
			}
			catch (std::exception & e){
				message = tr("Login operation failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Login operation failed with UNKNOWN error");
			}
		}
		else{
			message = tr("Login operation failed. Could not find HMDBService.");
		}
	}
	else{
		message = tr("Login operation failed. Uninitialized service manager.");
	}

	coreUI::CorePopup::showMessage(tr("Login status"), message);
}

void SourceOptionsWidget::onConnectionChanged(int idx)
{

}

void SourceOptionsWidget::onAccountOperationServiceSSLCertificateBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Chose account service certificate"), ui->opServiceCertificatePathLineEdit->text().isEmpty() == false ? ui->opServiceCertificatePathLineEdit->text() : QString(), tr("Certificate File (*.crt)"));

	ui->opServiceCertificatePathLineEdit->setText(sf);
}

void SourceOptionsWidget::onRegister()
{
	coreUI::CoreCursorChanger ccc;

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			QString message;
			try{
				hmdbSource->registerAccount(ui->opServiceURLLineEdit->text().toStdString(),
					ui->regUserLineEdit->text().toStdString(),
					ui->regPasswordLineEdit->text().toStdString(),
					ui->regNameLineEdit->text().toStdString(),
					ui->regSurnameLineEdit->text().toStdString(),
					ui->regEmailLineEdit->text().toStdString(),
					ui->opPropagateCheckBox->isChecked(),
					ui->opServiceCertificatePathLineEdit->text().toStdString());

				ccc.restore();
				message = tr("Registration successful");
			}
			catch (std::exception & e){
				message = tr("Registration failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Registration failed with UNKNOWN error");
			}

			coreUI::CorePopup::showMessage(tr("Registration status"), message);
		}
		else{
			PLUGIN_LOG_ERROR("Could not find HMDBSource");
		}
	}
	else{
		PLUGIN_LOG_ERROR("Uninitialized source manager");
	}
}

void SourceOptionsWidget::onActivate()
{
	coreUI::CoreCursorChanger ccc;

	char ret = ServerStatusWidget::UnknownStatus;

	QString message(tr("Account %1 activation failed for service %2 with UNKNOWN error"));

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){			
			try{
				hmdbSource->activateAccount(ui->opServiceURLLineEdit->text().toStdString(),
					ui->activateUserLineEdit->text().toStdString(), ui->activateActivationCodeLineEdit->text().toStdString(),
					ui->opPropagateCheckBox->isChecked(), ui->opServiceCertificatePathLineEdit->text().toStdString());

				message = tr("Activation successful for account %1 within service %2");
			}
			catch (std::exception & e){
				message = tr("Account %1 activation failed for service %2 with error: ") + QString::fromStdString(e.what());
			}
			catch (...){

			}			
		}
		else{
			message = tr("Account %1 activation failed for service %2 with error: Could not find HMDBService");						
		}
	}
	else{
		message = tr("Account %1 activation failed for service %2 with error: Uninitialized service manager");		
	}

	coreUI::CorePopup::showMessage(tr("Account activation status"), message.arg(ui->activateUserLineEdit->text()).arg(ui->opServiceURLLineEdit->text()));
}

void SourceOptionsWidget::onPasswordReset()
{
	coreUI::CoreCursorChanger ccc;

	char ret = ServerStatusWidget::UnknownStatus;

	QString message(tr("Password reset for email %1 failed within service %2 with UNKNOWN error"));

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			try{
				hmdbSource->resetPassword(ui->opServiceURLLineEdit->text().toStdString(),
					ui->resetEmailLineEdit->text().toStdString(), ui->opPropagateCheckBox->isChecked(),
					ui->opServiceCertificatePathLineEdit->text().toStdString());

				message = tr("Password reset successful for email %1 within service %2");
			}
			catch (std::exception & e){
				message = tr("Password reset for email %1 failed for service %2 with error: ") + QString::fromStdString(e.what());
			}
			catch (...){

			}
		}
		else{
			message = tr("Password reset for email %1 failed for service %2 with error: Could not find HMDBService");
		}
	}
	else{
		message = tr("Password reset for email %1 failed for service %2 with error: Uninitialized service manager");
	}

	coreUI::CorePopup::showMessage(tr("Password reset status"), message.arg(ui->resetEmailLineEdit->text()).arg(ui->opServiceURLLineEdit->text()));
}

void SourceOptionsWidget::onServerChange(int idx)
{

}

void SourceOptionsWidget::onServerSSLCertificateBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Chose server certificate"), ui->statusServerCertificatePathLineEdit->text().isEmpty() == false ? ui->statusServerCertificatePathLineEdit->text() : QString(), tr("Certificate File (*.crt)"));

	ui->statusServerCertificatePathLineEdit->setText(sf);
}

void SourceOptionsWidget::onStatusCheck()
{
	coreUI::CoreCursorChanger ccc;

	char ret = ServerStatusWidget::UnknownStatus;

	auto sm = plugin::getServiceManager();
	if (sm != nullptr){
		auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService != nullptr){
			QString message;
			try{
				const auto response = hmdbService->serverOnline(ui->statusServerURLLineEdit->text().toStdString(),
					ui->statusTimeoutSpinBox->value(), ui->statusServerCertificatePathLineEdit->text().toStdString());

				ret = ((response == true) ? ServerStatusWidget::OnlineStatus : ServerStatusWidget::OfflineStatus);

				ccc.restore();

				message = tr("Server status: ") + ((response == true) ? tr("On line") : tr("Off line"));

				serverStatusWidget->setServerStatus(ret);
			}
			catch (std::exception & e){
				message = tr("Server status check failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Server status check failed with UNKNOWN error");
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

void SourceOptionsWidget::onSourceModeChange(int mode)
{
	if (mode == Qt::Unchecked){
		ui->line_18->setVisible(false);
		ui->loginConnectionsGroupBox->setVisible(false);
	}
	else if (mode == Qt::Checked){
		ui->line_18->setVisible(true);
		ui->loginConnectionsGroupBox->setVisible(true);
	}
}