#include "CommunicationPCH.h"
#include "DataSourceWidget.h"
#include "DownloadStatusWidget.h"
#include "DataSource.h"
#include "DataSourcePathsManager.h"
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QHBoxLayout>
#include <QtCore/QRegExp>

#include "DataSourceWebServicesManager.h"
#include "DataSourceShallowCopyUtils.h"

#include "DefaultPatientCardWidget.h"
#include "DataSourceBaseContent.h"

#include <QtCore/QThread>
#include "FilesHelper.h"
#include <plugins/subject/ISubjectService.h>
#include <webserviceslib/DateTimeUtils.h>

using namespace communication;
using namespace webservices;

LocalDataLoader::LocalDataLoader(DataSourceWidget * sourceWidget) : sourceWidget(sourceWidget)
{

}

LocalDataLoader::~LocalDataLoader()
{
		
}

void LocalDataLoader::run()
{
	bool synch = true;

	auto localStorage = DataSourceLocalStorage::instance();

	//jesli mamy synchronizacj� to pr�bujemy parsowa� dane, ustawi� je, jesli wszystko ok to �adujemy do localstorage
	if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest && sourceWidget->downloadCanceled == false){
		communication::ShallowCopy shallowCopy;
		try{
			synch = sourceWidget->refreshShallowCopy();
		}catch(...){
			synch = false;
		}

		if(synch == false){
			sourceWidget->downloadCrashed = true;
			sourceWidget->downloadError = tr("Synchronization data corrupted");
		}
	}

	if(synch == true){
		QMetaObject::invokeMethod(this, "prepareStatusWidget", Qt::BlockingQueuedConnection);
		//od razu �adujemy do local storage
		QString info(tr("Synchronizing files:") + " %1 / " + QString::number(sourceWidget->downloadedFiles.size()));
		counter = 1;
		for(auto it = sourceWidget->downloadedFiles.begin(); it != sourceWidget->downloadedFiles.end(); ++it){
			try{
				localStorage->loadFile(*it);
			}catch(...){
				//TODO
				//co jak pliku jednak nie ma?
			}
			QMetaObject::invokeMethod(this, "updateStatusWidget", Qt::BlockingQueuedConnection);
			++counter;
		}
	}

	//definitywnie ko�czymy pobieranie i jego obs�ug�
	if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
		//TODO - jedna wspolna metoda
		//usuwam p�ytk� kopi�
		for(auto it = sourceWidget->downloadedFiles.begin(); it != sourceWidget->downloadedFiles.end(); ++it){
			try{
				core::Filesystem::deleteFile(*it);
			}catch(...){
				
			}
		}
	}else{
		//nie by� to shallowCopy wi�c zwyk�y transfer - musz� od�wiezy� status plik�w!!
		QMetaObject::invokeMethod(sourceWidget, "refreshStatusAfterDownload", Qt::BlockingQueuedConnection);
	}

	QMetaObject::invokeMethod(this, "showFinalMessage", Qt::BlockingQueuedConnection);
	if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
		sourceWidget->shallowCopyRequest.reset();
	}

	sourceWidget->filesToDownload.swap(std::set<int>());
	sourceWidget->downloadedFiles.swap(std::vector<std::string>());

	sourceWidget->currentDownloadRequest.reset();
}

void LocalDataLoader::prepareStatusWidget()
{
	sourceWidget->downloadStatusWidget->fileProgressBar->setVisible(false);
	sourceWidget->downloadStatusWidget->fileProgressLabel->setVisible(false);
	sourceWidget->downloadStatusWidget->totalProgressBar->setVisible(true);
	sourceWidget->downloadStatusWidget->totalProgressLabel->setVisible(true);
	sourceWidget->downloadStatusWidget->downloadTextStatus->setVisible(true);

	sourceWidget->downloadStatusWidget->totalProgressBar->setValue(0);
}

void LocalDataLoader::updateStatusWidget()
{
	sourceWidget->downloadStatusWidget->totalProgressBar->setValue(counter * 100 / sourceWidget->downloadedFiles.size());
	sourceWidget->downloadStatusWidget->downloadTextStatus->setText(info.arg(counter));
}

void LocalDataLoader::showFinalMessage()
{
	//obs�uga komunikatu
	if(sourceWidget->downloadCanceled == true){
		//anulowano pobieranie
		QMessageBox messageBox;
		if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
			messageBox.setWindowTitle(tr("Synchronization canceled"));
			messageBox.setText(tr("Synchronization successfully canceled"));
		}else{
			messageBox.setWindowTitle(tr("Download canceled"));
			messageBox.setText(tr("Download successfully canceled"));
		}

		messageBox.setIcon(QMessageBox::Icon::Information);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}else if(sourceWidget->downloadCrashed == true){
		//b��d pobierania
		QMessageBox messageBox;
		if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
			messageBox.setWindowTitle(tr("Synchronization error"));
			messageBox.setText(tr("Synchronization has failed with the following error: ") + sourceWidget->downloadError + "\n" + tr("Please try to synchronize later. If this error continues to happen contact producer"));
		}else{
			messageBox.setWindowTitle(tr("Download error"));
			messageBox.setText(tr("Download has failed with the following error: ") + sourceWidget->downloadError + "\n" + tr("Please try to download later. If this error continues to happen contact producer"));
		}

		messageBox.setIcon(QMessageBox::Icon::Critical);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}else{
		//wszystko ok
		QMessageBox messageBox;
		if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
			messageBox.setWindowTitle(tr("Synchronization successful"));
			messageBox.setText(tr("Synchronization has finished successfully"));
		}else{
			messageBox.setWindowTitle(tr("Download successful"));
			messageBox.setText(tr("Download has finished successfully"));
		}

		messageBox.setIcon(QMessageBox::Icon::Information);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}

	//pr�bujemy chowa� po 3 sekundach
	QTimer::singleShot(3000, sourceWidget, SLOT(tryHideStatusWidget()));
}


DataSourceWidget::DataSourceWidget(CommunicationDataSource * dataSource, QWidget * parent) : QTabWidget(parent), dataSource(dataSource),
	downloadStatusWidget(new DownloadStatusWidget()), downloadCanceled(false), downloadCrashed(false)
{
	setupUi(this);
	loginRecoveryButton->setVisible(false);
	initializeStatusIcons();

	setTabEnabled(0, false);
	setTabEnabled(1, false);
	setTabEnabled(2, true);
	setCurrentWidget(configTab);

	downloadStatusWidget->setVisible(false);
	loginProgressBar->setVisible(false);
	registrationProgressBar->setVisible(false);
	activationProgressBar->setVisible(false);

	connect(&downloadRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshDownloadProgress()));

	//inicjuj� karty pacjenta i ustawiam domy�ln�
	patientCardPlaceholerWidget->setLayout(new QHBoxLayout());
	registerPatientCard(new DefaultPatientCard());
	patientCardManager.setPatientCard(0);
	patientCardManager.currentPatientCard()->setPatient(nullptr);

	//inicjujemy perspektywy
	registerPerspective(new DataSourcePatientPerspective());
	registerPerspective(new DataSourceDisorderPerspective());
	registerPerspective(new DataSourceGenderPerspective());

	//inicjujemy content
	contentManager.registerContent(new DataSourceBaseContent());
	contentManager.setCurrentContent(0);

	filteredShallowCopyStatus.reset(new DataSourceStatusManager(dataSource->fileStatusManager.get()));

	//ustawiamy aktualn� perspektyw�
	perspectiveComboBox->setCurrentIndex(0);

	dataViewWidget->layout()->addWidget(downloadStatusWidget);
}

DataSourceWidget::~DataSourceWidget()
{

}

void DataSourceWidget::initializeStatusIcons()
{
	DataSourceStatusManager::setStatusIcon(DataStatus(Remote, Unloaded),statusIcon(Remote, Unloaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(Remote, Loaded),statusIcon(Remote, Loaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(Remote, PartiallyLoaded),statusIcon(Remote, PartiallyLoaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(Remote, UnknownUsage),statusIcon(Remote, UnknownUsage));

	DataSourceStatusManager::setStatusIcon(DataStatus(Local, Unloaded),statusIcon(Local, Unloaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(Local, Loaded),statusIcon(Local, Loaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(Local, PartiallyLoaded),statusIcon(Local, PartiallyLoaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(Local, UnknownUsage),statusIcon(Local, UnknownUsage));

	DataSourceStatusManager::setStatusIcon(DataStatus(PartiallyLocal, Unloaded),statusIcon(PartiallyLocal, Unloaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(PartiallyLocal, Loaded),statusIcon(PartiallyLocal, Loaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(PartiallyLocal, PartiallyLoaded),statusIcon(PartiallyLocal, PartiallyLoaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(PartiallyLocal, UnknownUsage),statusIcon(PartiallyLocal, UnknownUsage));

	DataSourceStatusManager::setStatusIcon(DataStatus(UnknownStorage, Unloaded),statusIcon(UnknownStorage, Unloaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(UnknownStorage, Loaded),statusIcon(UnknownStorage, Loaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(UnknownStorage, PartiallyLoaded),statusIcon(UnknownStorage, PartiallyLoaded));
	DataSourceStatusManager::setStatusIcon(DataStatus(UnknownStorage, UnknownUsage),statusIcon(UnknownStorage, UnknownUsage));
}

void DataSourceWidget::refreshCurrentPerspectiveContent()
{
	//od�wie�am bie��c�
	contentManager.refreshCurrentContent(perspectiveManager.currentPerspectiveWidget(), filteredShallowCopy, perspectiveManager.currentPerspective().get(),
		filteredShallowCopyStatus.get(),dataSource->fullShallowCopyStatus.get(), true);

	//aktualna perspektywa jest ju� ok
	perspectivesContent[perspectiveManager.currentPerspectiveWidget()] = contentManager.currentContentIndex();

	//sprawdzam czy mam jakiego� selecta - je�eli nie to pr�buje co� zaznaczy�

	auto selected = perspectiveManager.currentPerspectiveWidget()->selectedItems();

	if(selected.empty() == true && perspectiveManager.currentPerspectiveWidget()->topLevelItemCount() > 0){
		perspectiveManager.currentPerspectiveWidget()->topLevelItem(0)->setSelected(true);
	}
}

void DataSourceWidget::refreshStatus()
{
	//ustaw kursor na myslenie
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();
	//od�wie�am status plik�w
	dataSource->fileStatusManager->refreshFilesStatus();
	//od�wie�am status pe�nej plytkiej kopii
	dataSource->fullShallowCopyStatus->setShallowCopy(&dataSource->fullShallowCopy);
	//od�wie�am status przefiltrowanej p�ytkiej kopii
	filteredShallowCopyStatus->setShallowCopy(&filteredShallowCopy);
	//uniewa�niam wszystkie perspektywy
	invalidatePerspectivesContent();
	//od�wie�am bierz�c� perspektyw�
	refreshCurrentPerspectiveContent();
	//przywracam kursor
	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::refreshStatus(const std::set<int> & filesIDs)
{
	//od�wie�am status plik�w
	dataSource->fileStatusManager->refreshFilesStatus(filesIDs);
	//od�wie�am status pe�nej plytkiej kopii
	dataSource->fullShallowCopyStatus->refreshDataStatus(filesIDs);
	//od�wie�am status przefiltrowanej p�ytkiej kopii
	filteredShallowCopyStatus->refreshDataStatus(filesIDs);
	//uniewa�niam wszystkie perspektywy
	invalidatePerspectivesContent();
	//od�wie�am bie��c�
	refreshCurrentPerspectiveContent();
}

void DataSourceWidget::refreshStatusAfterDownload()
{
	refreshStatus(filesToDownload);
}


QIcon DataSourceWidget::statusIcon(const communication::DataStorage storage, const communication::DataUsage usage)
{
	//zaczynamy ze sposobem sk�adowania
	QPixmap s;

	switch(storage){
	case Local:
		s = QPixmap(QString::fromUtf8(":/resources/icons/lokalne.png"));
		break;

	default:
	case Remote:
		s = QPixmap(QString::fromUtf8(":/resources/icons/zdalne.png"));
		break;
	case PartiallyLocal:
		s = QPixmap(QString::fromUtf8(":/resources/icons/pol-na-pol-green.png"));
		break;
	}

	// teraz stan u�ycia
	QPixmap u;

	switch(usage){
	case Loaded:
		u = QPixmap(QString::fromUtf8(":/resources/icons/zaladowane.png"));
		break;

	default:
	case Unloaded:
		u = QPixmap(QString::fromUtf8(":/resources/icons/niezaladowane.png"));
		break;
	case PartiallyLoaded:
		u = QPixmap(QString::fromUtf8(":/resources/icons/polnapol-ladowanie2.png"));
		break;
	}

	//// mamy ju� wszystko - generujemy ikon�
	return QIcon(mergePixmaps(s, u));
}


QPixmap DataSourceWidget::mergePixmaps(const QPixmap & pmL, const QPixmap & pmR)
{
	int halfWidth = max(pmL.width(), pmR.width());
	int width = max(pmL.width() + pmR.width(), 2 * halfWidth);
	int pomHalfWidth = halfWidth/4;
	width += pomHalfWidth;

	int height = max(pmL.height(), pmR.height());

	QPixmap ret(width, height);

	QPainter painter(&ret);    
	painter.fillRect(0,0, width, height, Qt::white);

	painter.setOpacity(1);

	QPoint pos;
	pos.setX((halfWidth - pmL.width()) / 2);
	pos.setY((height - pmL.height()) / 2);

	painter.drawPixmap(pos, pmL);

	pos.setX(halfWidth + pomHalfWidth + (halfWidth - pmR.width()) / 2);
	pos.setY((height - pmR.height()) / 2);

	painter.drawPixmap(pos, pmR);

	return ret;
}

void DataSourceWidget::registerPerspective(communication::IDataSourcePerspective * perspective)
{
	int idx = perspectiveManager.registerPerspective(perspective);

	auto w = perspectiveManager.perspectiveWidget(idx);
	dataViewWidget->layout()->addWidget(w);
	w->setVisible(false);
	w->setSelectionMode(QAbstractItemView::SingleSelection);
	w->setSelectionBehavior(QAbstractItemView::SelectRows);
	w->setContextMenuPolicy(Qt::CustomContextMenu);
	w->setIconSize(QSize(40, 20));

	perspectiveComboBox->addItem(QString::fromStdString(perspective->name()));
}

void DataSourceWidget::registerPatientCard(communication::IPatientCard * patientCard)
{
	int idx = patientCardManager.registerPatientCard(patientCard);

	auto w = patientCardManager.patientCardWidget(idx);
	patientCardPlaceholerWidget->layout()->addWidget(w);
	w->setVisible(false);

	patientCardPlaceholerWidget->layout()->addWidget(w);
	//TODO
	//w�asne menu kontekstowe?
	//doda� jakie� combo do zmiany widoku karty pacjenta?
}

void DataSourceWidget::getPatientAndSubject(QTreeWidgetItem * item, const webservices::MedicalShallowCopy::Patient *& patient,
	const webservices::MotionShallowCopy::Performer *& subject)
{
	while(item != nullptr){
		IContent * content = dynamic_cast<IContent*>(item);
		if(content != nullptr){
			//wystarczy switch + dynamic_cast
			bool found = false;

			switch(content->contentType()){

			case PatientContent:
				patient = dynamic_cast<PatientItem*>(item)->value();
				subject = patient->performer;
				found = true;
				break;
			case SubjectContent:
				subject = dynamic_cast<SubjectItem*>(item)->value();
				patient = subject->patient;
				found = true;
				break;
			case SessionContent:
				subject = dynamic_cast<SessionItem*>(item)->value()->performerConf->performer;
				patient = subject->patient;
				found = true;
				break;
			case MotionContent:
				subject = dynamic_cast<MotionItem*>(item)->value()->session->performerConf->performer;
				patient = subject->patient;
				found = true;
				break;
			case FileContent:
				{
					auto file = dynamic_cast<FileItem*>(item)->value();
					subject = file->isSessionFile() == true ? file->session->performerConf->performer : file->trial->session->performerConf->performer;
					patient = subject->patient;
					found = true;
				}
				break;
			case SessionsGroupContent:
				break;
			case MotionsGroupContent:
				break;
			case FilesGroupContent:
				break;
			}

			if(found == true){
				break;
			}
		}

		item = item->parent();
	}
}

void DataSourceWidget::onPerspectiveCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	//pr�buje ustali� pacjenta, je�eli si� uda to ustawiam kart� pacjenta
	const MedicalShallowCopy::Patient * patient = nullptr;
	const MotionShallowCopy::Performer * subject = nullptr;
	getPatientAndSubject(current, patient, subject);

	patientCardManager.currentPatientCard()->setPatient(patient, subject, QPixmap(), dataSource->currentUser_.userData());
	
	//nag��wek danych
	QStringList headers;
	bool ok = perspectiveManager.currentPerspective()->headers(current, headers);
	if(ok == false){
		headers.clear();
		if(contentManager.currentContent()->headers(current, headers) == false){
			headers.clear();
			headers << tr("Data");
		}
	}

	auto currentPerspective = perspectiveManager.currentPerspectiveWidget();
	currentPerspective->setHeaderLabels(headers);
	for(int i = 0; i < headers.size(); ++i){
		currentPerspective->resizeColumnToContents(i);
	}
}


void DataSourceWidget::onFilterChange(int idx)
{	
	try{

		ShallowCopy tmpShallow;

		if(idx > -1){
			//filtrujemy dane
			DataSourceFilterManager::filterShallowCopy(dataSource->fullShallowCopy, tmpShallow, filterManager.dataFilter(idx));
		}else{
			//nie trzeba filtrowa� - wystarczy przepisa�
			tmpShallow = dataSource->fullShallowCopy;
		}
	
		//ustawiam przefiltorwane dane
		std::swap(filteredShallowCopy, tmpShallow);

		//ustawiamy aktualny filtr
		filterManager.setCurrentFilter(idx);

		//od�wie�am status przefiltrowanych danych
		filteredShallowCopyStatus->setShallowCopy(&filteredShallowCopy);

		//uniewa�niamy perspektywy
		perspectiveManager.invalidatePerspectives();
		//od�wie�amy aktualn� perspektyw�
		perspectiveManager.rebuildCurrentPerspective(filteredShallowCopy);
		//uniewa�niam content - zmieni�a si� zawarto��
		invalidatePerspectivesContent();
		//wype�niamy content aktualnej perspektywy
		refreshCurrentPerspectiveContent();		

	}catch(...){

	}
}

void DataSourceWidget::invalidatePerspectivesContent()
{
	for(auto it = perspectivesContent.begin(); it != perspectivesContent.end(); ++it){
		it->second = -1;
	}
}

void DataSourceWidget::onFilterEdit()
{

}

void DataSourceWidget::onFilterAdd()
{

}

void DataSourceWidget::onFilterRemove()
{

}

void DataSourceWidget::onLogin()
{
	// je�eli zalogowany to wyloguj

	if(dataSource->isLogged() == true){

		if(currentDownloadRequest != nullptr){
			//mamy jakie� �ci�ganie w mi�dzyczasie!!
			//pytamy czy na pewno wylogowa� - wtedy je anulujemy

			QMessageBox messageBox;
			messageBox.setWindowTitle(tr("Logout process"));
			messageBox.setText(tr("Some download is currently performed. Do you want to logout and cancel it?"));
			messageBox.setIcon(QMessageBox::Icon::Warning);
			messageBox.setStandardButtons(QMessageBox::StandardButton::Yes| QMessageBox::StandardButton::No);
			messageBox.setDefaultButton(QMessageBox::StandardButton::No);
			int ret = messageBox.exec();
			if(ret == QMessageBox::StandardButton::No){
				return;
			}else{
				currentDownloadRequest->cancel();
				downloadRefreshTimer.stop();
				if(localDataLoader != nullptr && localDataLoader->isRunning()){
					localDataLoader->terminate();
					localDataLoader->wait();
					localDataLoader.reset();
				}
			}
		}

		//wy�adowa� wszystkie dane
		for(auto it = filesLoadedToDM.begin(); it != filesLoadedToDM.end(); ++it){
			try{
				std::vector<core::ObjectWrapperPtr> objects;
				const auto & p = dataSource->fileStatusManager->filePath(*it);
				dataSource->fileDM->removeFile(p);
				//unloadedFiles.insert(*it);
			}catch(std::exception & e){
				//unloadingErrors[*it] = std::string(e.what());
			}catch(...){
				//unknownErrors.push_back(*it);
			}
		}

		//usuwamy wszystkie elementy pluginu subject
		unloadSubjectHierarchy();

		dataSource->logout();

		setTabEnabled(0, false);
		setTabEnabled(1, false);
		setTabEnabled(2, true);
		setCurrentWidget(configTab);

		onFilterChange(-1);
		onPerspectiveChange(0);
		onContentChange(0);

		loginButton->setText(tr("Login"));
	}else{

		// pierwsza weryfikacja danych wej�ciowych do logowania - czy podano niezb�dne dane
		if(userEdit->text().isEmpty() == true || passwordEdit->text().isEmpty() == true){

			QMessageBox messageBox;
			messageBox.setWindowTitle(tr("Login problem"));
			messageBox.setText(tr("Both fields: User and Password need to be filled. Please fill them and retry."));
			messageBox.setIcon(QMessageBox::Icon::Warning);
			messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
			messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
			messageBox.exec();

		}else{

			////uruchamiam progressbar
			////resetuje go
			//loginProgressBar->setRange(0,100);
			//loginProgressBar->setValue(0);
			////pokazuje
			//loginProgressBar->setVisible(true);
			////uruchamiam
			//loginProgressBar->setRange(0,0);

			setCursor(Qt::WaitCursor);
			QApplication::processEvents();

			//mamy jakie� dane wej�ciowe - mo�emy pr�bowa� logowa�

			QString error(tr("Error during login: "));
			bool wasError = false;
			try{
				dataSource->login(userEdit->text().toStdString(), passwordEdit->text().toStdString());
			}catch(std::exception & e){
				error += e.what();
				wasError = true;
			}catch(...){
				error += tr("UNKNOWN ERROR");
				wasError = true;
			}

			// b��d logowania - b��d po��czenia z webserwisami

			if(wasError == true){

				error += ". " + tr("Please verify your internet connection, ensure firewall pass through EDR communication. If problem continues contact the producer.");

				QMessageBox messageBox;
				messageBox.setWindowTitle(tr("Login error"));
				messageBox.setText(error);
				messageBox.setIcon(QMessageBox::Icon::Critical);
				messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
				messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
				messageBox.exec();

			}else if(dataSource->isLogged() == true){

				bool synch = false;
				bool shallowCopyAvailable = false;

				dataSource->pathsManager->createUserDataPaths();

				communication::ShallowCopy userShallowCopy;

				loginButton->setText(tr("Logout"));

				// poprawna komunikacja, u�ytkownik zweryfikowany || brak komunikacji i logowanie lokalne

				if(dataSource->currentUser_.id() == -2){
					// je�eli jestem zalogowany lokalnie to informuj� o tym
					QMessageBox messageBox;
					messageBox.setWindowTitle(tr("Login information"));
					messageBox.setText(tr("User is logged locally - there might be a problem with internet connection and remote services might not work correctly."));
					messageBox.setIcon(QMessageBox::Icon::Information);
					messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
					messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
					messageBox.exec();
				}

				if(dataSource->isShallowCopyComplete() == true){

					bool extractStatus = false;

					try{
						dataSource->extractShallowCopyFromLocalStorageToUserSpace();
						extractStatus = dataSource->buildShallowCopyFromLocalUserSpace(userShallowCopy);
						dataSource->removeShallowCopyFromUserSpace();
						if(extractStatus == false){
							//TODO
							//co� nie tak z nasza p�ytk�kopi� bazy danych - trzeba j� walidowa� i poprawia� - pewnie obcina� a� b�dzie poprawna i sp�jna
						}else{
							shallowCopyAvailable = true;
						}

					}catch(std::exception & e){

					}catch(...){

					}

					if(extractStatus == false){
						QMessageBox messageBox;
						messageBox.setWindowTitle(tr("Synchronization error"));
						messageBox.setText(tr("Synchronization data is corrupted. Please try again later. If problem continues contact producer"));
						messageBox.setIcon(QMessageBox::Icon::Warning);
						messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
						messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
						messageBox.exec();
					}else{

						//pobierz dat� ostatenij modyfikacji i por�wnaj
						//jesli nowsza to zaproponuj synchronizacj�
						//je�li odm�wi za�aduj ju� sparsowan� p�ytk� kopi� bazy danych
						try{
							auto time = DataSourceWebServicesManager::instance()->motionBasicQueriesService()->dataModificationTime();

							if(DataSourceShallowCopyUtils::shallowCopyRequiresRefresh(userShallowCopy, time) == true){
								QMessageBox messageBox;
								messageBox.setWindowTitle(tr("Synchronization required"));
								messageBox.setText(tr("Database was updated. Some data might be not available. Would You like to synchronize?"));
								messageBox.setIcon(QMessageBox::Icon::Information);
								messageBox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
								messageBox.setDefaultButton(QMessageBox::StandardButton::Yes);
								auto ret = messageBox.exec();
								if(ret = QMessageBox::StandardButton::Yes){
									synch = true;
								}
							}
						}catch(std::exception & e){

						}catch(...){

						}
					}
				}else{

					QMessageBox messageBox;
					messageBox.setWindowTitle(tr("Synchronization required"));
					messageBox.setText(tr("Some data are not available. Synchronization is required for further data processing. Would You like to synchronize?"));
					messageBox.setIcon(QMessageBox::Icon::Warning);
					messageBox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
					messageBox.setDefaultButton(QMessageBox::StandardButton::Yes);
					int ret = messageBox.exec();
					if(ret == QMessageBox::StandardButton::Yes){
						synch = true;
					}

				}

				if(synch == true){
					performShallowCopyUpdate();
				}else if(shallowCopyAvailable == true){
					//ustawiamy now� p�ytk� kopi� bazy danych
					dataSource->setShallowCopy(userShallowCopy);
					//od�wie�am przefiltrowan� p�ytk� kopi� danych co wi��e si� z uniewa�nieniem dotychczasowych perspektyw
					onFilterChange(filterManager.currentFilterIndex());
				}

				setTabEnabled(0, true);
				setTabEnabled(1, true);
				setTabEnabled(2, true);
				setCurrentWidget(motionDataTab);

			}else{

				//poprawna komunikacja, nie uda�o si� zweryfikowa� u�ytkownika

				QMessageBox messageBox;
				messageBox.setWindowTitle(tr("Login unsuccessful"));
				messageBox.setText(tr("Given user or password is incorrect. Please correct it and try again. If problem continues contact database administrators."));
				messageBox.setIcon(QMessageBox::Icon::Warning);
				messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
				messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
				messageBox.exec();
			}

			////koniec progressbara
			////chowam go
			//loginProgressBar->setVisible(false);
			////resetuje go
			//loginProgressBar->setRange(0,100);
			//loginProgressBar->setValue(0);
			setCursor(Qt::ArrowCursor);
		}
	}
}

void DataSourceWidget::saveAndRemoveShallowCopy()
{
	try{
		dataSource->saveShallowCopyInLocalStorage();
		dataSource->removeShallowCopyFromUserSpace();
	}catch(...){
		//TODO
		//zareagowa� kiedy nie mo�emy zapisa� danych do loaclStorage - w sumie ju� ich nie potrzebujemy, tylko przy kolejnym logowaniu trzeba zn�w p�ytk� kopi� �ci�gn�c
	}
}

void DataSourceWidget::onLoginRecovery()
{

}

void DataSourceWidget::onRegistration()
{
	static const QString strPatt("\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\b");
	static const QRegExp rx(strPatt, Qt::CaseInsensitive);

	QStringList verification;
	if(regLoginEdit->text().isEmpty() == true || regPassEdit->text().isEmpty() == true
		|| regRepPassEdit->text().isEmpty() == true || nameEdit->text().isEmpty() == true
		|| surnameEdit->text().isEmpty() == true || emailEdit->text().isEmpty() == true){

			verification.push_back(tr("All fields: Login, Password, Repeat password, Name, Surname and E-mail must be filled. Please correct them and try again."));
	}

	if(regPassEdit->text().isEmpty() == false && regRepPassEdit->text().isEmpty() == false && regPassEdit->text() != regRepPassEdit->text()){
		verification.push_back(tr("Password differs from its repeat. Please verify password."));
	}

	/*if(emailEdit->text().isEmpty() == false && rx.exactMatch(emailEdit->text()) == false){
		verification.push_back(tr("Incorrect E-mail. Please verify Your E-mail address."));
	}*/

	if(verification.empty() == false){

		//generuje liste problem�w
		QString message;

		for(int i = 0; i < verification.size(); ++i){
			message += QString::number(i+1) + ". " + verification[i] + "\n";
		}

		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Registration form validation"));
		messageBox.setText(message);
		messageBox.setIcon(QMessageBox::Icon::Warning);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}else {
		
		if(dataSource->registerUser(regLoginEdit->text().toStdString(), emailEdit->text().toStdString(), regPassEdit->text().toStdString(),
			nameEdit->text().toStdString(), surnameEdit->text().toStdString()) == true){

			QMessageBox messageBox;
			messageBox.setWindowTitle(tr("Registration successful"));
			messageBox.setText(tr("Your registration has finished. Activate Your account to be able to login and get access to database."));
			messageBox.setIcon(QMessageBox::Icon::Information);
			messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
			messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
			messageBox.exec();

			if(dataSource->isLogged() == false){
				//je�li nikt nie jest jeszcze zalogowany to przenosimy do strony logowania
				toolBox->setCurrentIndex(2);
				//cz�sciowo wype�niam ju� dane usera
				activationLoginEdit->setText(regLoginEdit->text());
			}
		} else{
			QMessageBox messageBox;
			messageBox.setWindowTitle(tr("Registration failed"));
			messageBox.setText(tr("Could not registrate. Please change login or email and try again. If problem continues contact producer."));
			messageBox.setIcon(QMessageBox::Icon::Critical);
			messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
			messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
			messageBox.exec();
		}
	}
}

void DataSourceWidget::onActivate()
{
	if(activationLoginEdit->text().isEmpty() == true || activationCodeEdit->text().isEmpty() == true){
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Activation validation"));
		messageBox.setText(tr("Both fields: Login and Activation code must be filled. Please correct them and try again."));
		messageBox.setIcon(QMessageBox::Icon::Warning);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}else if(dataSource->tryActivateAccount(activationLoginEdit->text().toStdString(), activationCodeEdit->text().toStdString()) == true){
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Activation successful"));
		messageBox.setText(tr("Given login has been successfully activated. You can now login using this account."));
		messageBox.setIcon(QMessageBox::Icon::Information);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();

		if(dataSource->isLogged() == false){
			//je�li nikt nie jest jeszcze zalogowany to przenosimy do strony logowania
			toolBox->setCurrentIndex(0);
			//cz�sciowo wype�niam ju� dane usera
			userEdit->setText(activationLoginEdit->text());
		}
	}else{
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Activation failed"));
		messageBox.setText(tr("Could not activate given login with provided activation code. Verify data and try again. If problem continiues please contact the producer."));
		messageBox.setIcon(QMessageBox::Icon::Critical);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}
}

void DataSourceWidget::onPerspectiveChange(int idx)
{
	auto currentPW = perspectiveManager.currentPerspectiveWidget();
	if(currentPW != nullptr){
		disconnect(currentPW, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(perspectiveContextMenu(QPoint)));
		disconnect(currentPW, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem *)), this, SLOT(onPerspectiveCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	}

	try{
		//ustawiamy aktualn� perspektyw�
		perspectiveManager.setCurrentPerspective(idx);
		//od�wie�amy aktualn� perspektyw�
		perspectiveManager.rebuildCurrentPerspective(filteredShallowCopy);

		currentPW = perspectiveManager.currentPerspectiveWidget();
		if(currentPW != nullptr){
			connect(currentPW, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(perspectiveContextMenu(QPoint)));
			connect(currentPW, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem *)), this, SLOT(onPerspectiveCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
		}

		auto it = perspectivesContent.find(perspectiveManager.currentPerspectiveWidget());
		bool refreshContent = false;
		if(it == perspectivesContent.end()){
			perspectivesContent[perspectiveManager.currentPerspectiveWidget()] = -1;
			refreshContent = true;
		}else if(it->second != contentManager.currentContentIndex()){
			refreshContent = true;
		}

		if(refreshContent == true){
			//wype�niamy content aktualnej perspektywy
			refreshCurrentPerspectiveContent();
		}

		if(currentPW->topLevelItemCount() > 0){
			currentPW->setCurrentItem(currentPW->topLevelItem(0));
			currentPW->setItemSelected(currentPW->topLevelItem(0), true);
		}else{
			QStringList headers;
			headers << tr("Data");
			currentPW->setHeaderLabels(headers);
			currentPW->resizeColumnToContents(0);
		}
	}catch(...){

	}
}

void DataSourceWidget::onContentChange(int idx)
{
	try{
		//ustawiamy aktualn� perspektyw�
		contentManager.setCurrentContent(idx);

		//wype�niamy content aktualnej perspektywy
		refreshCurrentPerspectiveContent();
	}catch(...){

	}
}

void DataSourceWidget::perspectiveContextMenu(const QPoint & pos)
{
	QTreeWidget * perspective = qobject_cast<QTreeWidget*>(sender());

	if(perspective == nullptr){
		return;
	}

	setCursor(Qt::WaitCursor);
	QApplication::processEvents();

	//generujemy menu kontekstowe - download, load, unload + updateShallowCopy
	QMenu menu;

	//poszcze�lne akcje
	auto load = menu.addAction(tr("Load"));
	auto unload = menu.addAction(tr("Unload"));
	menu.addSeparator();
	auto download = menu.addAction(tr("Download"));
	menu.addSeparator();
	auto refresh = menu.addAction(tr("Refresh status"));
	menu.addSeparator();
	auto synch = menu.addAction(tr("Synchronize"));

	//chwilowo dezaktywuje - nie wiem co b�dzie
	load->setEnabled(false);
	unload->setEnabled(false);
	download->setEnabled(false);
	refresh->setEnabled(false);
	synch->setEnabled(false);

	//skoro co� �ci�gam musz� poczeka�!!
	if(currentDownloadRequest != nullptr){
		return;
	}

	//aktywujemy podstaweowe operacje
	connect(refresh, SIGNAL(triggered()), this, SLOT(refreshStatus()));
	refresh->setEnabled(true);

	connect(synch, SIGNAL(triggered()), this, SLOT(updateShallowCopy()));
	synch->setEnabled(true);

	//sprawdzamy co to za item, potrzebujemy wszystkich plik�w z nim zwi�zanych
	std::set<int> filesIDs;

	auto selectedItems = perspective->selectedItems();

	if(selectedItems.empty() == false){		
		UTILS_ASSERT(selectedItems.size() == 1, "Zly model selekcji");

		//sprawdzam czy to cos co mog� za�adowa�?

		if(isItemLoadable(selectedItems[0]) == true){
			
			currentPerspectiveItem = selectedItems[0];
			//pobieram pliki dla wybranego elementu
			getItemsFiles(currentPerspectiveItem, filesIDs, filteredShallowCopy);
			UTILS_ASSERT(filesIDs.empty() == false, "Brak danych w drzewie");
		
			if(filesIDs.empty() == false){
				//mam dane - mog� pracowa�
				//wyci�gam dane lokalne i zdalne
				//wyci�gam dane za��dowane i nieza�adowane
				//na bazie tych info odpowiednio ��cze akcje ze slotami i aktywuje je + zapami�tuje te info do wykonania ich!!

				//pomijamy wszystkie niekompatybilne z DM pliki
				const auto & extensions = dataSource->fileDM->getSupportedFilesExtensions();				
				std::set<int> dmOKFiles;

				//filtruje pliki obs�ugiwane przez DM
				FilesHelper::filterFiles(filesIDs, extensions, dmOKFiles, *(dataSource->fileStatusManager));

				//pliki do za�adowania
				FilesHelper::filterFiles(dmOKFiles, DataStatus(Local, Unloaded), filesToLoad, *(dataSource->fileStatusManager));

				//pliki do wy�adowania - EXPERIMENTAL
				FilesHelper::filterFiles(dmOKFiles, communication::Loaded, filesToUnload, *(dataSource->fileStatusManager));

				//pliki do �ci�gni�cia
				FilesHelper::filterFiles(filesIDs, communication::Remote, filesToDownload, *(dataSource->fileStatusManager));

				if(filesToLoad.empty() == false){
					load->setEnabled(true);
					connect(load, SIGNAL(triggered()), this, SLOT(onLoad()));
				}

				if(filesToUnload.empty() == false){
					unload->setEnabled(true);
					connect(unload, SIGNAL(triggered()), this, SLOT(onUnload()));
				}

				if(filesToDownload.empty() == false){
					download->setEnabled(true);
					connect(download, SIGNAL(triggered()), this, SLOT(onDownload()));
				}
			}
		}
	}

	setCursor(Qt::ArrowCursor);
	menu.exec(perspective->mapToGlobal(pos));

	currentPerspectiveItem = nullptr;

	filesToLoad.swap(std::set<int>());
	filesToUnload.swap(std::set<int>());
	//tutaj nie czyszcz� plik�w �ci�ganych bo one s� kopiowane w innym w�tku - tam to robie (LocalDataLoader)
}

bool DataSourceWidget::isItemLoadable(const QTreeWidgetItem * item)
{
	bool ret = false;
	auto content = dynamic_cast<const IContent*>(item);
	if(content != nullptr){

		ret = true;
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){

		case FileContent:
		case FilesGroupContent:
		case CustomContent:
			ret = false;
			break;
		}
	}

	return ret;
}

void DataSourceWidget::getItemsFiles(QTreeWidgetItem * item, std::set<int> & filesIDs, const ShallowCopy & shallowCopy)
{
	bool found = false;
	auto content = dynamic_cast<const IContent*>(item);
	if(content != nullptr){

		found = true;
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){

		case DisorderContent:
			{
				auto disorder = dynamic_cast<DisorderItem*>(item);
				FilesHelper::getFiles(disorder->value(), shallowCopy, filesIDs);
			}
			break;
		case PatientContent:
			{
				auto patient = dynamic_cast<PatientItem*>(item);
				FilesHelper::getFiles(patient->value(), filesIDs);
			}
			break;
		case SubjectContent:
			{
				auto subject = dynamic_cast<SubjectItem*>(item);
				FilesHelper::getFiles(subject->value(), filesIDs);
			}
			break;
		case SessionContent:
			{
				auto session = dynamic_cast<SessionItem*>(item);
				FilesHelper::getFiles(session->value(), filesIDs);
			}
			break;
		case MotionContent:
			{
				auto motion = dynamic_cast<MotionItem*>(item);
				FilesHelper::getFiles(motion->value(), filesIDs);
				//extra dodaj� pliki specyficzne sesji
				FilesHelper::getSpecificFiles(motion->value()->session, filesIDs);
			}
			break;
		case FileContent:
			{
				auto file = dynamic_cast<FileItem*>(item);
				filesIDs.insert(file->value()->fileID);
				//extra dodaj� pliki specyficzne sesji
				FilesHelper::getSpecificFiles(file->value()->isSessionFile() == true ? file->value()->session : file->value()->trial->session, filesIDs);
			}
			break;
		case DisordersGroupContent:
			{
				auto disordersGroup = dynamic_cast<DisordersGroupItem*>(item);
				for(auto it = disordersGroup->value().begin(); it != disordersGroup->value().end(); ++it){
					FilesHelper::getFiles(*it, shallowCopy, filesIDs);
				}
			}
			break;
		case PatientsGroupContent:
			{
				auto patientsGroup = dynamic_cast<PatientsGroupItem*>(item);
				for(auto it = patientsGroup->value().begin(); it != patientsGroup->value().end(); ++it){
					FilesHelper::getFiles(*it, filesIDs);
				}
			}
			break;
		case SubjectsGroupContent:
			{
				auto subjectsGroup = dynamic_cast<SubjectsGroupItem*>(item);
				for(auto it = subjectsGroup->value().begin(); it != subjectsGroup->value().end(); ++it){
					FilesHelper::getFiles(*it, filesIDs);
				}
			}
			break;
		case SessionsGroupContent:
			{
				auto sessionsGroup = dynamic_cast<SessionsGroupItem*>(item);
				for(auto it = sessionsGroup->value().begin(); it != sessionsGroup->value().end(); ++it){
					FilesHelper::getFiles(*it, filesIDs);
				}
			}
			break;
		case MotionsGroupContent:
			{
				auto motionsGroup = dynamic_cast<MotionsGroupItem*>(item);
				for(auto it = motionsGroup->value().begin(); it != motionsGroup->value().end(); ++it){
					FilesHelper::getFiles(*it, filesIDs);
					//extra dodaj� pliki specyficzne sesji
					FilesHelper::getSpecificFiles((*it)->session, filesIDs);
				}
			}
			break;
		case FilesGroupContent:
			{
				auto filesGroup = dynamic_cast<FilesGroupItem*>(item);
				for(auto it = filesGroup->value().begin(); it != filesGroup->value().end(); ++it){
					filesIDs.insert((*it)->fileID);
					//extra dodaj� pliki specyficzne sesji
					FilesHelper::getSpecificFiles((*it)->isSessionFile() == true ? (*it)->session : (*it)->trial->session, filesIDs);
				}
			}
			break;
		case CustomContent:
			found = false;
			break;
		}
	}

	//jezeli nie znalazlem to ide w d� hierarchi perspektywy - mo�e tam co� znajd�
	if(found == false){
		int childrenCount = item->childCount();
		for(int i = 0; i < childrenCount; ++i){
			getItemsFiles(item->child(i), filesIDs, shallowCopy);
		}
	}	
}

void DataSourceWidget::updateShallowCopy()
{
	//lokujemy - nie mo�na teraz zmieni� uzytkownika z zewn�trz
	//w sumie powinni�my lokowac ca�e �r�d�o
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*DataSourcePathsManager::instance());
	if(dataSource->isShallowCopyComplete() == true && dataSource->isShallowCopyCurrent() == true){
		//message box - nie musimy aktualizowa�, mamy naj�wie�sz� wersj�
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Database synchronization"));
		messageBox.setText(tr("Local data already synchronized. Synchronization is not required. Proceed anyway?"));
		messageBox.setIcon(QMessageBox::Icon::Question);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
		messageBox.setDefaultButton(QMessageBox::StandardButton::No);
		int ret = messageBox.exec();
		if(ret == QMessageBox::StandardButton::No){
			return;
		}
	}

	performShallowCopyUpdate();
}

void DataSourceWidget::performShallowCopyUpdate()
{
	//generujemy �cie�ki

	auto downloadRequest = dataSource->generateDownloadShallowCopyRequestToLocalUserSpace();

	if(downloadRequest != nullptr){
		shallowCopyRequest = downloadRequest;
		processShallowCopyDownload(downloadRequest);
	}else{
		//TODO
		//error genrerowania requesta!!
	}
}

void DataSourceWidget::processShallowCopyDownload(const CommunicationDataSource::DownloadRequestPtr & request)
{
	processDownload(request);
}

void DataSourceWidget::processDataDownload(QTreeWidgetItem * item, const CommunicationDataSource::DownloadRequestPtr & request)
{
	processDownload(request);
}

void DataSourceWidget::processDownload(const CommunicationDataSource::DownloadRequestPtr & request)
{
	downloadedFiles.swap(std::vector<std::string>());
	//zanim wystartujemy musimy zarejestrowa� obiekt kt�ry b�dzie obserwowa� zmiany w reque�cie pobierania + aktualizowa� progress pobierania
	request->attach(this);
	registeredRequests[request.get()] = request;
	//startujemy request - dodajemy do kolejki, wi�c niekoniecznie po tym wywo�aniu zaczniemy pobiera�
	//to operuje na communication manager - co� mo�e jednak p�j�� nie tak
	try{
		request->start();
	}catch(std::exception & e){
		//TODO
		//message box z informacj�
	}catch(...){
		//TODO
		//message box z informacj�
	}
}

bool DataSourceWidget::refreshShallowCopy()
{
	bool ret = true;

	communication::ShallowCopy shallowCopy;
	try{
		if(dataSource->buildShallowCopyFromLocalUserSpace(shallowCopy) == true){

			//ustawiam nowa p�ytka kopi� danych
			dataSource->setShallowCopy(shallowCopy);
			//od�wie�am przefiltrowan� p�ytk� kopi� danych co wi��e si� z uniewa�nieniem dotychczasowych perspektyw
			QMetaObject::invokeMethod(this, "onFilterChange", Q_ARG(int, filterManager.currentFilterIndex()));
		}else{
			ret = false;
		}
	}catch(...){
		ret = false;
	}

	return ret;
}


void DataSourceWidget::onDownload()
{
	// poziom 100MB jako limit ostrze�enia
	static const long downloadSizeWarningLevel = 104857600;
	//wyliczmy ile miejsca potrzebujemy i ile mamy

	unsigned long long size = 0;

	for(auto it = filesToDownload.begin(); it != filesToDownload.end(); ++it){
		auto f = filteredShallowCopy.motionShallowCopy->files.find(*it);
		size += f->second->fileSize;
	}

	unsigned long long avaiable = dataSource->pathsManager->freeSpace(dataSource->pathsManager->userDataPath());

	//mno�e razy 2 bo musz� to potem jeszcze do local storage wrzuci�!!
	if(avaiable < (size << 1)){
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Download problem"));
		messageBox.setText(tr("Download request is ") + QString::number(size) + tr(" bytes large while there is only ") + QString::number(avaiable) + tr(" bytes free. Download could not be continued. Please free some space or truncate your download request."));
		messageBox.setIcon(QMessageBox::Icon::Warning);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);
		messageBox.exec();
	}else{
		//ok - mog� �ci�ga� ale mo�e tego jest sporo wi�c trzeba u�ytkownika u�wiadomi�
		if(size > downloadSizeWarningLevel){
			QMessageBox messageBox;
			messageBox.setWindowTitle(tr("Download warning"));
			messageBox.setText(tr("Download request is ") + QString::number(size) + tr(" bytes large. This will take some time to download such amount of data. Are you sure you can wait some longer time?"));
			messageBox.setIcon(QMessageBox::Icon::Warning);
			messageBox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
			messageBox.setDefaultButton(QMessageBox::StandardButton::No);
			
			int ret = messageBox.exec();

			if(ret == QMessageBox::StandardButton::No){
				//u�ytkownik si� nawr�ci� - anulujemy
				return;
			}
		}

		//skoro tu jestem to zaczynamy �ci�ga�
		try{
			//przygotuj request dla �ci�gania
			auto downloadRequest = dataSource->generateDownloadFilesRequest(filesToDownload);
			//zacznij przetwarza� request
			processDataDownload(nullptr, downloadRequest);
			
		}catch(std::exception & e){
			//nie uda�o si� utworzy�/przygotowa� requesta wi�c info o b��dzie
			QMessageBox messageBox;
			messageBox.setWindowTitle(tr("Download preparation error"));
			messageBox.setText(tr("Error while preparing download request. Error description: ") + QString::fromUtf8(e.what()));
			messageBox.setIcon(QMessageBox::Icon::Critical);
			messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
			messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);

			messageBox.exec();
		}
	}
}

void DataSourceWidget::onLoad()
{
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();
	//! �aduje pliki do DM
	std::set<int> loadedFiles;
	std::map<int, std::vector<core::ObjectWrapperPtr>> loadedFilesObjects;
	std::map<int, std::string> loadingErrors;
	std::vector<int> unknownErrors;

	for(auto it = filesToLoad.begin(); it != filesToLoad.end(); ++it){
		try{
			std::vector<core::ObjectWrapperPtr> objects;
			const auto & p = dataSource->fileStatusManager->filePath(*it);
			dataSource->fileDM->addFile(p, objects);
			loadedFiles.insert(*it);
			loadedFilesObjects[*it] = objects;
		}catch(std::exception & e){
			loadingErrors[*it] = std::string(e.what());
		}catch(...){
			unknownErrors.push_back(*it);
		}
	}

	filesLoadedToDM.insert(loadedFiles.begin(), loadedFiles.end());

	refreshStatus(loadedFiles);

	//w loadedFilesObjects mamy info o plikach i zwi�zanych z nimi obiektach domenowych
	//pr�bujemy teraz przez plugin subject realizowa� hierarchi� danych
	
	loadSubjectHierarchy(loadedFilesObjects);


	if(loadingErrors.empty() == true && unknownErrors.empty() == true){
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Loading info"));
		messageBox.setText(tr("Data loaded successfully to application."));
		messageBox.setIcon(QMessageBox::Icon::Information);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);

		messageBox.exec();
	}else{
		QString message(tr("Errors while data loading:"));
		
		int i = 1;

		for(auto it = loadingErrors.begin(); it != loadingErrors.end(); ++it){
			message += tr("\n%1. File ID: %2. Error description: %3").arg(i).arg(it->first).arg(QString::fromUtf8(it->second.c_str()));
			++i;
		}

		for(auto it = unknownErrors.begin(); it != unknownErrors.end(); ++it){
			message += tr("\n%1. File ID: %2. Unknown error.").arg(i).arg(*it);
			++i;
		}

		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Loading warning"));
		messageBox.setText(message);
		messageBox.setIcon(QMessageBox::Icon::Warning);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);

		messageBox.exec();
	}

	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::loadSubjectHierarchy(const std::map<int, std::vector<core::ObjectWrapperPtr>> & loadedFilesObjects)
{
	typedef std::map<int, std::set<int>> MotionFiles;
	typedef std::map<int, std::pair<std::set<int>, MotionFiles>> SessionFiles;
	typedef std::map<int, SessionFiles> SubjectFiles;

	//TODO
	//zainicjowa� wska�nik do serwisu!!

	auto subjectService = core::queryServices<PluginSubject::ISubjectService>(dataSource->serviceManager);

	if(subjectService == nullptr){
		return;
	}

	//buduje map� hierarchii subject -> session -> motion -> files
	//na bazie tej mapy b�d� realizowa� hierarchi� pluginu subject
	
	//TODO
	//zrewidowa� plugin subject!!
	//obiekty tej hierarchii powinny by� edytowalne po stronie �r�d�a aby mog�o elastyczniej nimi zarz�dza�!!
	SubjectFiles subjectHierarchy;

	auto itEND = loadedFilesObjects.end();
	for(auto it = loadedFilesObjects.begin(); it != itEND; ++it){

		if(it->first < 0){
			auto s = filteredShallowCopy.motionShallowCopy->sessions.find(- it->first);

			if(s != filteredShallowCopy.motionShallowCopy->sessions.end()){
				subjectHierarchy[s->second->performerConf->performer->performerID][s->second->sessionID].first.insert(it->first);
			}else{
				//TODO
				//INFO o nieobs�ugiwanym pliku
			}

		}else{
			auto fileIT = filteredShallowCopy.motionShallowCopy->files.find(it->first);
			if(fileIT == filteredShallowCopy.motionShallowCopy->files.end()){
				//TODO
				//INFO o nieobs�ugiwanym pliku
				continue;
			}

			if(fileIT->second->isSessionFile() == true){
				subjectHierarchy[fileIT->second->session->performerConf->performer->performerID][fileIT->second->session->sessionID].first.insert(it->first);
			}else{
				subjectHierarchy[fileIT->second->trial->session->performerConf->performer->performerID][fileIT->second->trial->session->sessionID].second[fileIT->second->trial->trialID].insert(it->first);
			}
		}
	}
	
	for(auto subjectIT = subjectHierarchy.begin(); subjectIT != subjectHierarchy.end(); ++subjectIT){
		//tworz� subject je�li to konieczne!!

		PluginSubject::SubjectPtr subPtr;

		auto subIT = subjectsMapping.find(subjectIT->first);
		if(subIT != subjectsMapping.end()){
			//mam subjecta - nie musze ju� nic robi�
			subPtr = subIT->second->get();
		}else{
			//tworz� subjecta
			subPtr = subjectService->createSubject();
			//dodaj� do DM
			auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, subPtr);

			core::MetadataPtr meta(new core::Metadata(ow));
			std::stringstream label;

			auto sIT = filteredShallowCopy.medicalShallowCopy->patients.find(subjectIT->first);

			if(sIT == filteredShallowCopy.medicalShallowCopy->patients.end()){
				label << "Subject " << subjectIT->first;
			}else{
				label << sIT->second->name << ", " << sIT->second->surname;
			}

			//auto s = filteredShallowCopy.medicalShallowCopy->patients.find(subjectIT->first)->second;
			meta->setValue("label", label.str());

			core::IMemoryDataManager::addData(dataSource->memoryDM, meta);

			//zapami�tuj� mapowanie
			subjectsMapping[subjectIT->first] = ow;
		}

		//mam subjecta, mog� i�� dalej do sesji
		for(auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){

			PluginSubject::SessionPtr sPtr;

			auto sIT = sessionsMapping.find(sessionIT->first);
			if(sIT != sessionsMapping.end()){
				//mam subjecta - nie musze ju� nic robi�
				sPtr = sIT->second->get();
			}else{
				//tworz� sesj�
				//generuj� zbi�r ow dla sesji
				std::vector<core::ObjectWrapperConstPtr> sessionObjects;
				for(auto fIT = sessionIT->second.first.begin(); fIT != sessionIT->second.first.end(); ++fIT){
					//pobieram obiekty
					const auto & objects = loadedFilesObjects.find(*fIT)->second;
					//kopiuje
					for(auto oIT = objects.begin(); oIT != objects.end(); ++oIT){
						sessionObjects.push_back(*oIT);
					}
				}

				sPtr = subjectService->createSession(subPtr, sessionObjects);
				//dodaj� do DM
				auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, sPtr);

				core::MetadataPtr meta(new core::Metadata(ow));
				auto s = filteredShallowCopy.motionShallowCopy->sessions.find(sessionIT->first)->second;
				meta->setValue("label", s->sessionName);
				meta->setValue("EMGConf", boost::lexical_cast<std::string>(s->emgConf));

				core::IMemoryDataManager::addData(dataSource->memoryDM, meta);

				//zapami�tuj� mapowanie
				sessionsMapping[sessionIT->first] = ow;
			}

			//mam sesj� - mog� i�� dalej z motionami!!
			for(auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){

				PluginSubject::MotionPtr mPtr;

				auto mIT = motionsMapping.find(motionIT->first);
				if(mIT != motionsMapping.end()){
					//mam subjecta - nie musze ju� nic robi�
					//TODO
					//nie powinno mnie tu by� wg aktualnego dzia�ania pluginu subject!!
					mPtr = mIT->second->get();
				}else{
					//tworz� sesj�
					//generuj� zbi�r ow dla motiona
					std::vector<core::ObjectWrapperConstPtr> motionObjects;
					for(auto fIT = motionIT->second.begin(); fIT != motionIT->second.end(); ++fIT){
						//pobieram obiekty
						const auto & objects = loadedFilesObjects.find(*fIT)->second;
						//kopiuje
						for(auto oIT = objects.begin(); oIT != objects.end(); ++oIT){
							motionObjects.push_back(*oIT);
						}
					}

					mPtr = subjectService->createMotion(sPtr,motionObjects);
					//dodaj� do DM
					auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, mPtr);

					core::MetadataPtr meta(new core::Metadata(ow));
					auto s = filteredShallowCopy.motionShallowCopy->trials.find(motionIT->first)->second;
					meta->setValue("label", s->trialName);

					core::IMemoryDataManager::addData(dataSource->memoryDM, meta);

					//zapami�tuj� mapowanie
					motionsMapping[motionIT->first] = ow;
				}
			}
		}
	}
}

void DataSourceWidget::onUnload()
{
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();
	//! �aduje pliki do DM
	std::set<int> unloadedFiles;
	std::map<int, std::string> unloadingErrors;
	std::vector<int> unknownErrors;

	for(auto it = filesToUnload.begin(); it != filesToUnload.end(); ++it){
		try{
			std::vector<core::ObjectWrapperPtr> objects;
			const auto & p = dataSource->fileStatusManager->filePath(*it);
			dataSource->fileDM->removeFile(p);
			unloadedFiles.insert(*it);
		}catch(std::exception & e){
			unloadingErrors[*it] = std::string(e.what());
		}catch(...){
			unknownErrors.push_back(*it);
		}
	}

	std::vector<int> diff(filesLoadedToDM.size() - unloadedFiles.size());
	std::set_difference(filesLoadedToDM.begin(), filesLoadedToDM.end(), unloadedFiles.begin(), unloadedFiles.end(), diff.begin());

	filesLoadedToDM.swap(std::set<int>(diff.begin(), diff.end()));

	refreshStatus(unloadedFiles);

	//pr�bujemy teraz przez plugin subject realizowa� hierarchi� danych

	unloadSubjectHierarchy(unloadedFiles);


	if(unloadingErrors.empty() == true && unknownErrors.empty() == true){
		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Unloading info"));
		messageBox.setText(tr("Data unloaded successfully."));
		messageBox.setIcon(QMessageBox::Icon::Information);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);

		messageBox.exec();
	}else{
		QString message(tr("Errors while data unloading:"));

		int i = 1;

		for(auto it = unloadingErrors.begin(); it != unloadingErrors.end(); ++it){
			message += tr("\n%1. File ID: %2. Error description: %3").arg(i).arg(it->first).arg(QString::fromUtf8(it->second.c_str()));
			++i;
		}

		for(auto it = unknownErrors.begin(); it != unknownErrors.end(); ++it){
			message += tr("\n%1. File ID: %2. Unknown error.").arg(i).arg(*it);
			++i;
		}

		QMessageBox messageBox;
		messageBox.setWindowTitle(tr("Unloading warning"));
		messageBox.setText(message);
		messageBox.setIcon(QMessageBox::Icon::Warning);
		messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);
		messageBox.setDefaultButton(QMessageBox::StandardButton::Ok);

		messageBox.exec();
	}

	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::unloadSubjectHierarchy(const std::set<int> & unloadedFilesIDs)
{
	typedef std::map<int, std::set<int>> MotionFiles;
	typedef std::map<int, std::pair<std::set<int>, MotionFiles>> SessionFiles;
	typedef std::map<int, SessionFiles> SubjectFiles;

	//zainicjowa� wska�nik do serwisu!!

	auto subjectService = core::queryServices<PluginSubject::ISubjectService>(dataSource->serviceManager);

	if(subjectService == nullptr){
		return;
	}

	//buduje map� hierarchii subject -> session -> motion -> files
	//na bazie tej mapy b�d� realizowa� hierarchi� pluginu subject

	//TODO
	//zrewidowa� plugin subject!!
	//obiekty tej hierarchii powinny by� edytowalne po stronie �r�d�a aby mog�o elastyczniej nimi zarz�dza�!!
	SubjectFiles subjectHierarchy;

	auto itEND = unloadedFilesIDs.end();
	for(auto it = unloadedFilesIDs.begin(); it != itEND; ++it){
		auto fileIT = filteredShallowCopy.motionShallowCopy->files.find(*it);
		if(fileIT->second->isSessionFile() == true){
			subjectHierarchy[fileIT->second->session->performerConf->performer->performerID][fileIT->second->session->sessionID].first.insert(*it);
		}else{
			subjectHierarchy[fileIT->second->trial->session->performerConf->performer->performerID][fileIT->second->trial->session->sessionID].second[fileIT->second->trial->trialID].insert(*it);
		}
	}

	for(auto subjectIT = subjectHierarchy.begin(); subjectIT != subjectHierarchy.end(); ++subjectIT){

		for(auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){

			for(auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){

				PluginSubject::MotionPtr mPtr;

				auto mIT = motionsMapping.find(motionIT->first);
				if(mIT != motionsMapping.end()){
					//mam motion - sprawdzam czy mog� go usun��
					std::set<int> motionFiles;
					FilesHelper::getMotionFiles(motionIT->first, filteredShallowCopy, motionFiles);

					//teraz robi� r�nic� tego co jest w p�ytkiej kopii a tego co mam wy�adowa�
					std::vector<int> diff(max(motionFiles.size(), motionIT->second.size()));
					auto diffIT = std::set_difference(motionFiles.begin(), motionFiles.end(), motionIT->second.begin(), motionIT->second.end(), diff.begin());

					if(diffIT == diff.begin()){
						//to znaczy �e usun��em wszystkie pliki motiona -> mog� usuwa� motiona
						dataSource->memoryDM->removeData(mIT->second);
						motionsMapping.erase(mIT);
					}else{
						//TODO
						//ta sytuacja obecnie nie powinna mie� miejsca ze wzgl�du na specyfik� pluginu subject!!
						throw std::runtime_error("Plugin subject to redesign");
					}
				}else{
					//TODO
					//tutaj mnie nie powinno by� - chyba �e plugin subject wcze�niej nie dzia�a�
					//w takim razie nie reaguje
				}
			}

			//usun��em motiony sesji - czas zobaczy� czy sesja jeszcze czym� dysponuje
			PluginSubject::SessionPtr sPtr;

			auto sIT = sessionsMapping.find(sessionIT->first);
			if(sIT != sessionsMapping.end()){
				//mam subjecta - nie musze ju� nic robi�
				sPtr = sIT->second->get();
				PluginSubject::Motions motions;
				sPtr->getMotions(motions);
				if(motions.empty() == true){
					//sesja jest pusta - do usuni�cia
					dataSource->memoryDM->removeData(sIT->second);
					sessionsMapping.erase(sIT);
				}
			}else{
				//TODO
				//tutaj mnie nie powinno by� - chyba �e plugin subject wcze�niej nie dzia�a�
				//w takim razie nie reaguje
			}
		}

		//usuwam subject je�li to konieczne!!

		PluginSubject::SubjectPtr subPtr;

		auto subIT = subjectsMapping.find(subjectIT->first);
		if(subIT != subjectsMapping.end()){
			//mam subjecta - nie musze ju� nic robi�
			subPtr = subIT->second->get();
			PluginSubject::Sessions sessions;
			subPtr->getSessions(sessions);
			if(sessions.empty() == true){
				dataSource->memoryDM->removeData(subIT->second);
				subjectsMapping.erase(subIT);
			}
		}else{
			//TODO
			//tutaj mnie nie powinno by� - chyba �e plugin subject wcze�niej nie dzia�a�
			//w takim razie nie reaguje
		}
	}
}

void DataSourceWidget::unloadSubjectHierarchy()
{
	for(auto it = motionsMapping.begin(); it != motionsMapping.end(); ++it){
		dataSource->memoryDM->removeData(it->second);
	}

	for(auto it = sessionsMapping.begin(); it != sessionsMapping.end(); ++it){
		dataSource->memoryDM->removeData(it->second);
	}

	for(auto it = subjectsMapping.begin(); it != subjectsMapping.end(); ++it){
		dataSource->memoryDM->removeData(it->second);
	}

	for(auto it = patientsMapping.begin(); it != patientsMapping.end(); ++it){
		dataSource->memoryDM->removeData(it->second);
	}

	patientsMapping.swap(std::map<int, core::ObjectWrapperPtr>());
	subjectsMapping.swap(std::map<int, core::ObjectWrapperPtr>());
	sessionsMapping.swap(std::map<int, core::ObjectWrapperPtr>());
	motionsMapping.swap(std::map<int, core::ObjectWrapperPtr>());
}

void DataSourceWidget::onUpdateDownloadRequest()
{
	switch(currentDownloadRequest->state()){

	case IDownloadRequest::Started:
		{
			resetDownloadProgressStatus();

			if(currentDownloadRequest->totalFilesToDownload() == 1){
				downloadStatusWidget->fileProgressBar->setVisible(false);
				downloadStatusWidget->fileProgressLabel->setVisible(false);
			}

			refreshDownloadProgress();

			downloadStatusWidget->setVisible(true);

			//rozpocznij od�wie�anie widgeta w osobnym w�tku
			downloadRefreshTimer.start(40);
		}
		break;

	case IDownloadRequest::State::FinishedCancel:
		//info co si� sta�o
		downloadCanceled = true;
		finishDownloadRequest();
		break;

	case IDownloadRequest::FinishedError:
		{
			downloadCrashed = true;
			//info co si� sta�o
			auto reqError = currentDownloadRequest->error();
			if(reqError.empty() == true){
				downloadError = tr("Unknown error");
			}else{
				downloadError = QString::fromStdString(reqError);
			}

			finishDownloadRequest();
		}
		break;

	case IDownloadRequest::FinishedOK:
		finishDownloadRequest();
		break;
	}
}

void DataSourceWidget::finishDownloadRequest()
{
	//zako�cz od�wie�anie widgeta w osobnym w�tku
	downloadRefreshTimer.stop();
	localDataLoader.reset(new LocalDataLoader(this));
	//synchronizujemy to co si� uda�o w ca�o�ci pobra� (zapisujemy do loaclStorage + informacja co si� sta�o
	localDataLoader->start();	
}

void DataSourceWidget::tryHideStatusWidget()
{
	if(getObservedObjects().empty() == true){
		downloadStatusWidget->setVisible(false);
	}
}

void DataSourceWidget::update(const communication::IDownloadRequest * request)
{
	if(request->state() == IDownloadRequest::Pending){
		return;
	}

	if(currentDownloadRequest == nullptr){
		auto it = registeredRequests.find((void*)request);
		currentDownloadRequest = it->second;
		registeredRequests.erase(it);
	}

	//zapami�tuje sobie pliki kt�re si� dobrze �ci�gne�y
	if(currentDownloadRequest->state() == IDownloadRequest::SingleFinished){
		if(core::Filesystem::pathExists(currentDownloadRequest->currentFilePath()) == true){
			downloadedFiles.push_back(currentDownloadRequest->currentFilePath());
		}
	}else if(currentDownloadRequest->state() == IDownloadRequest::Started){
		//TO JEST BARDZO NIEDOBRE ALE INACZEJ NIE ZAGWARANTUJEMY NATYCHMIASTOWEGO ODSWIEZENIA UI WZGLEDEM COMMUNICATION MANAGER
		QMetaObject::invokeMethod(this, "onUpdateDownloadRequest");
	}else{
		//TO JEST BARDZO NIEDOBRE ALE INACZEJ NIE ZAGWARANTUJEMY NATYCHMIASTOWEGO ODSWIEZENIA UI WZGLEDEM COMMUNICATION MANAGER
		QMetaObject::invokeMethod(this, "onUpdateDownloadRequest", Qt::BlockingQueuedConnection);
	}
}

void DataSourceWidget::refreshDownloadProgress()
{
	downloadStatusWidget->totalProgressBar->setValue(currentDownloadRequest->totalProgress());
	downloadStatusWidget->fileProgressBar->setValue(currentDownloadRequest->currentFileProgress());
	downloadStatusWidget->downloadTextStatus->setText(tr("Downloading file %1 [%2/%3]").arg(QString::fromStdString(currentDownloadRequest->currentFileName())).arg(QString::number(currentDownloadRequest->currentFile())).arg(currentDownloadRequest->totalFilesToDownload()));
}

void DataSourceWidget::resetDownloadProgressStatus()
{
	downloadStatusWidget->totalProgressBar->setValue(0);
	downloadStatusWidget->fileProgressBar->setValue(0);
	downloadStatusWidget->downloadTextStatus->setText(QString());
	downloadStatusWidget->downloadTextStatus->setStyleSheet(QString());
}