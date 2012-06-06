#include "CommunicationPCH.h"
#include "DataSourceWidget.h"
#include "DownloadStatusWidget.h"
#include "DataSource.h"
#include "DataSourcePathsManager.h"
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QHBoxLayout>
#include <QtCore/QRegExp>
#include <QtGui/QKeyEvent>

#include <boost/tokenizer.hpp>

#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include "Patient.h"

#include "DataSourceWebServicesManager.h"
#include "DataSourceShallowCopyUtils.h"

#include "DefaultPatientCardWidget.h"
#include "DataSourceBaseContent.h"

#include <QtCore/QThread>
#include "FilesHelper.h"
#include <plugins/subject/ISubjectService.h>
#include <webserviceslib/DateTimeUtils.h>
#include <QtGui/QInputDialog>
#include <plugins/kinematic/Wrappers.h>

using namespace communication;
using namespace webservices;

LocalDataLoader::LocalDataLoader(DataSourceWidget * sourceWidget) : sourceWidget(sourceWidget), counter(-1)
{

}

LocalDataLoader::~LocalDataLoader()
{
		
}

void LocalDataLoader::run()
{
	bool synch = true;

	auto localStorage = DataSourceLocalStorage::instance();

	//jesli mamy synchronizacjê to próbujemy parsowaæ dane, ustawiæ je, jesli wszystko ok to ³adujemy do localstorage
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
		//od razu ³adujemy do local storage
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

	//definitywnie koñczymy pobieranie i jego obs³ugê
	if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
		//TODO - jedna wspolna metoda
		//usuwam p³ytk¹ kopiê
		for(auto it = sourceWidget->downloadedFiles.begin(); it != sourceWidget->downloadedFiles.end(); ++it){
			try{
				core::Filesystem::deleteFile(*it);
			}catch(...){
				
			}
		}
	}else{
		//nie by³ to shallowCopy wiêc zwyk³y transfer - muszê odœwiezyæ status plików!!
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
	//obs³uga komunikatu
	if(sourceWidget->downloadCanceled == true){
		//anulowano pobieranie
		QMessageBox messageBox(sourceWidget);
		if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
			messageBox.setWindowTitle(tr("Synchronization canceled"));
			messageBox.setText(tr("Synchronization successfully canceled"));
		}else{
			messageBox.setWindowTitle(tr("Download canceled"));
			messageBox.setText(tr("Download successfully canceled"));
		}

		messageBox.setIcon(QMessageBox::Information);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else if(sourceWidget->downloadCrashed == true){
		//b³¹d pobierania
		QMessageBox messageBox(sourceWidget);
		if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
			messageBox.setWindowTitle(tr("Synchronization error"));
			messageBox.setText(tr("Synchronization has failed with the following error: ") + sourceWidget->downloadError + "\n" + tr("Please try to synchronize later. If this error continues to happen contact producer"));
		}else{
			messageBox.setWindowTitle(tr("Download error"));
			messageBox.setText(tr("Download has failed with the following error: ") + sourceWidget->downloadError + "\n" + tr("Please try to download later. If this error continues to happen contact producer"));
		}

		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else{
		//wszystko ok
		QMessageBox messageBox(sourceWidget);
		if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
			messageBox.setWindowTitle(tr("Synchronization successful"));
			messageBox.setText(tr("Synchronization has finished successfully"));
		}else{
			messageBox.setWindowTitle(tr("Download successful"));
			messageBox.setText(tr("Download has finished successfully"));
		}

		messageBox.setIcon(QMessageBox::Information);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}

	//próbujemy chowaæ po 3 sekundach
	QTimer::singleShot(3000, sourceWidget, SLOT(tryHideStatusWidget()));
}

DataSourceWidget::LoginEventFilter::LoginEventFilter(DataSourceWidget * sourceWidget, QObject * parent) : QObject(parent), sourceWidget(sourceWidget)
{

}

bool DataSourceWidget::LoginEventFilter::eventFilter(QObject * watched, QEvent * event)
{
	if(event->type() == QEvent::KeyPress){
		QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
			sourceWidget->onLogin();
			return true;
		} else
			return false;
	}
	return false;
}


DataSourceWidget::DataSourceWidget(CommunicationDataSource * dataSource, QWidget * parent) : QTabWidget(parent), loginEventFilter(nullptr), dataSource(dataSource),
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

	//inicjujê karty pacjenta i ustawiam domyœln¹
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

	//ustawiamy aktualn¹ perspektywê
	perspectiveComboBox->setCurrentIndex(0);

	dataViewWidget->layout()->addWidget(downloadStatusWidget);

	loginEventFilter = new LoginEventFilter(this);
	loginPage->installEventFilter(loginEventFilter);
	userEdit->installEventFilter(loginEventFilter);
	passwordEdit->installEventFilter(loginEventFilter);
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
	//odœwie¿am bie¿¹c¹
	contentManager.refreshCurrentContent(perspectiveManager.currentPerspectiveWidget(), filteredShallowCopy, perspectiveManager.currentPerspective().get(),
		filteredShallowCopyStatus.get(),dataSource->fullShallowCopyStatus.get(), true);

	//aktualna perspektywa jest ju¿ ok
	perspectivesContent[perspectiveManager.currentPerspectiveWidget()] = contentManager.currentContentIndex();

	//sprawdzam czy mam jakiegoœ selecta - je¿eli nie to próbuje coœ zaznaczyæ

	auto selected = perspectiveManager.currentPerspectiveWidget()->selectedItems();

	if(selected.empty() == true && perspectiveManager.currentPerspectiveWidget()->topLevelItemCount() > 0){
		perspectiveManager.currentPerspectiveWidget()->topLevelItem(0)->setSelected(true);
	}
}

void DataSourceWidget::connectionModeChanged()
{
	QCheckBox * widget = qobject_cast<QCheckBox*>(sender());

	if(widget == nullptr){
		return;
	}

	dataSource->setOfflineMode(widget->isChecked());
}

void DataSourceWidget::refreshStatus()
{
	//ustaw kursor na myslenie
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();
	//odœwie¿am status plików
	dataSource->fileStatusManager->refreshFilesStatus();
	//odœwie¿am status pe³nej plytkiej kopii
	dataSource->fullShallowCopyStatus->setShallowCopy(&dataSource->fullShallowCopy);
	//odœwie¿am status przefiltrowanej p³ytkiej kopii
	filteredShallowCopyStatus->setShallowCopy(&filteredShallowCopy);
	//uniewa¿niam wszystkie perspektywy
	invalidatePerspectivesContent();
	//odœwie¿am bierz¹c¹ perspektywê
	refreshCurrentPerspectiveContent();
	//przywracam kursor
	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::refreshStatus(const std::set<int> & filesIDs)
{
	//odœwie¿am status plików
	dataSource->fileStatusManager->refreshFilesStatus(filesIDs);
	//odœwie¿am status pe³nej plytkiej kopii
	dataSource->fullShallowCopyStatus->refreshDataStatus(filesIDs);
	//odœwie¿am status przefiltrowanej p³ytkiej kopii
	filteredShallowCopyStatus->refreshDataStatus(filesIDs);
	//uniewa¿niam wszystkie perspektywy
	invalidatePerspectivesContent();
	//odœwie¿am bie¿¹c¹
	refreshCurrentPerspectiveContent();
}

void DataSourceWidget::refreshStatusAfterDownload()
{
	refreshStatus(filesToDownload);
}


QIcon DataSourceWidget::statusIcon(const communication::DataStorage storage, const communication::DataUsage usage)
{
	//zaczynamy ze sposobem sk³adowania
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

	// teraz stan u¿ycia
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

	//// mamy ju¿ wszystko - generujemy ikonê
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

	perspectiveComboBox->addItem(QString::fromUtf8(perspective->name().c_str()));
}

void DataSourceWidget::registerPatientCard(communication::IPatientCard * patientCard)
{
	int idx = patientCardManager.registerPatientCard(patientCard);

	auto w = patientCardManager.patientCardWidget(idx);
	patientCardPlaceholerWidget->layout()->addWidget(w);
	w->setVisible(false);

	patientCardPlaceholerWidget->layout()->addWidget(w);
	//TODO
	//w³asne menu kontekstowe?
	//dodaæ jakieœ combo do zmiany widoku karty pacjenta?
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

void DataSourceWidget::onPerspectiveSelectionChanged()
{
	QTreeWidget * perspective = qobject_cast<QTreeWidget*>(sender());

	if(perspective == nullptr){
		return;
	}

	QTreeWidgetItem * current = nullptr;

	auto sel = perspective->selectedItems();
	if(sel.empty() == false){
		current = sel.first();
	}

	//próbuje ustaliæ pacjenta, je¿eli siê uda to ustawiam kartê pacjenta
	const MedicalShallowCopy::Patient * patient = nullptr;
	const MotionShallowCopy::Performer * subject = nullptr;
	getPatientAndSubject(current, patient, subject);

	patientCardManager.currentPatientCard()->setPatient(patient, subject, QPixmap(), dataSource->currentUser_.userData());
	
	//nag³ówek danych
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
			//nie trzeba filtrowaæ - wystarczy przepisaæ
			tmpShallow = dataSource->fullShallowCopy;
		}
	
		//ustawiam przefiltorwane dane
		std::swap(filteredShallowCopy, tmpShallow);

		//ustawiamy aktualny filtr
		filterManager.setCurrentFilter(idx);

		//odœwie¿am status przefiltrowanych danych
		filteredShallowCopyStatus->setShallowCopy(&filteredShallowCopy);

		//uniewa¿niamy perspektywy
		perspectiveManager.invalidatePerspectives();
		//odœwie¿amy aktualn¹ perspektywê
		perspectiveManager.rebuildCurrentPerspective(filteredShallowCopy);
		//uniewa¿niam content - zmieni³a siê zawartoœæ
		invalidatePerspectivesContent();
		//wype³niamy content aktualnej perspektywy
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
	// je¿eli zalogowany to wyloguj

	if(dataSource->isLogged() == true){

		if(currentDownloadRequest != nullptr){
			//mamy jakieœ œci¹ganie w miêdzyczasie!!
			//pytamy czy na pewno wylogowaæ - wtedy je anulujemy

			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Logout process"));
			messageBox.setText(tr("Some download is currently performed. Do you want to logout and cancel it?"));
			messageBox.setIcon(QMessageBox::Warning);
			messageBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
			messageBox.setDefaultButton(QMessageBox::No);
			int ret = messageBox.exec();
			if(ret == QMessageBox::No){
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

		//usuwamy wszystkie elementy pluginu subject
		unloadSubjectHierarchy();

		//wy³adowaæ wszystkie dane
		for(auto it = filesLoadedToDM.begin(); it != filesLoadedToDM.end(); ++it){
			try{
				std::vector<core::ObjectWrapperPtr> objects;
				const auto & p = dataSource->fileStatusManager->filePath(*it);
				dataSource->fileDM->removeFile(p);
				//unloadedFiles.insert(*it);
			}catch(std::exception & ){
				//unloadingErrors[*it] = std::string(e.what());
			}catch(...){
				//unknownErrors.push_back(*it);
			}
		}

		trySaveProjects();

		dataSource->logout();

		setTabEnabled(0, false);
		setTabEnabled(1, false);
		setTabEnabled(2, true);
		setCurrentWidget(configTab);

		onFilterChange(-1);
		onPerspectiveChange(0);
		onContentChange(0);

		projects.swap(std::map<std::string, std::set<int>>());

		loginButton->setText(tr("Login"));
	}else{

		// pierwsza weryfikacja danych wejœciowych do logowania - czy podano niezbêdne dane
		if(userEdit->text().isEmpty() == true || passwordEdit->text().isEmpty() == true){

			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Login problem"));
			messageBox.setText(tr("Both fields: User and Password need to be filled. Please fill them and retry."));
			messageBox.setIcon(QMessageBox::Warning);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);
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

			//mamy jakieœ dane wejœciowe - mo¿emy próbowaæ logowaæ

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

			// b³¹d logowania - b³¹d po³¹czenia z webserwisami

			if(wasError == true){

				error += ". " + tr("Please verify your internet connection, ensure firewall pass through EDR communication. If problem continues contact the producer.");

				QMessageBox messageBox(this);
				messageBox.setWindowTitle(tr("Login error"));
				messageBox.setText(error);
				messageBox.setIcon(QMessageBox::Critical);
				messageBox.setStandardButtons(QMessageBox::Ok);
				messageBox.setDefaultButton(QMessageBox::Ok);
				messageBox.exec();

			}else if(dataSource->isLogged() == true){

				bool synch = false;
				bool shallowCopyAvailable = false;

				dataSource->pathsManager->createUserDataPaths();

				communication::ShallowCopy userShallowCopy;

				loginButton->setText(tr("Logout"));

				// poprawna komunikacja, u¿ytkownik zweryfikowany || brak komunikacji i logowanie lokalne

				if(dataSource->currentUser_.id() == -2){
					// je¿eli jestem zalogowany lokalnie to informujê o tym
					QMessageBox messageBox(this);
					messageBox.setWindowTitle(tr("Login information"));
					messageBox.setText(tr("User is logged locally - there might be a problem with internet connection and remote services might not work correctly."));
					messageBox.setIcon(QMessageBox::Information);
					messageBox.setStandardButtons(QMessageBox::Ok);
					messageBox.setDefaultButton(QMessageBox::Ok);
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
							//coœ nie tak z nasza p³ytk¹kopi¹ bazy danych - trzeba j¹ walidowaæ i poprawiaæ - pewnie obcinaæ a¿ bêdzie poprawna i spójna
						}else{
							shallowCopyAvailable = true;
						}

					}catch(std::exception & ){

					}catch(...){

					}

					if(extractStatus == false){
						QMessageBox messageBox(this);
						messageBox.setWindowTitle(tr("Synchronization error"));
						messageBox.setText(tr("Synchronization data is corrupted. Please try again later. If problem continues contact producer"));
						messageBox.setIcon(QMessageBox::Warning);
						messageBox.setStandardButtons(QMessageBox::Ok);
						messageBox.setDefaultButton(QMessageBox::Ok);
						messageBox.exec();
					}else{

						//pobierz datê ostatenij modyfikacji i porównaj
						//jesli nowsza to zaproponuj synchronizacjê
						//jeœli odmówi za³aduj ju¿ sparsowan¹ p³ytk¹ kopiê bazy danych
						try{
							auto time = DataSourceWebServicesManager::instance()->motionBasicQueriesService()->dataModificationTime();

							if(DataSourceShallowCopyUtils::shallowCopyRequiresRefresh(userShallowCopy, time) == true){
								QMessageBox messageBox(this);
								messageBox.setWindowTitle(tr("Synchronization required"));
								messageBox.setText(tr("Database was updated. Some data might be not available. Would You like to synchronize?"));
								messageBox.setIcon(QMessageBox::Information);
								messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
								messageBox.setDefaultButton(QMessageBox::Yes);
								auto ret = messageBox.exec();
								if(ret == QMessageBox::Yes){
									synch = true;
								}
							}
						}catch(std::exception & ){

						}catch(...){

						}
					}
				}else{

					QMessageBox messageBox(this);
					messageBox.setWindowTitle(tr("Synchronization required"));
					messageBox.setText(tr("Some data are not available. Synchronization is required for further data processing. Would You like to synchronize?"));
					messageBox.setIcon(QMessageBox::Warning);
					messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
					messageBox.setDefaultButton(QMessageBox::Yes);
					int ret = messageBox.exec();
					if(ret == QMessageBox::Yes){
						synch = true;
					}

				}

				if(synch == true){
					performShallowCopyUpdate();
				}else if(shallowCopyAvailable == true){
					//ustawiamy now¹ p³ytk¹ kopiê bazy danych
					dataSource->setShallowCopy(userShallowCopy);
					//odœwie¿am przefiltrowan¹ p³ytk¹ kopiê danych co wi¹¿e siê z uniewa¿nieniem dotychczasowych perspektyw
					onFilterChange(filterManager.currentFilterIndex());
				}

				tryLoadProjects();

				setTabEnabled(0, true);
				setTabEnabled(1, true);
				setTabEnabled(2, true);
				setCurrentWidget(motionDataTab);

			}else{

				//poprawna komunikacja, nie uda³o siê zweryfikowaæ u¿ytkownika

				QMessageBox messageBox(this);
				messageBox.setWindowTitle(tr("Login unsuccessful"));
				messageBox.setText(tr("Given user or password is incorrect. Please correct it and try again. If problem continues contact database administrators."));
				messageBox.setIcon(QMessageBox::Warning);
				messageBox.setStandardButtons(QMessageBox::Ok);
				messageBox.setDefaultButton(QMessageBox::Ok);
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
		//zareagowaæ kiedy nie mo¿emy zapisaæ danych do loaclStorage - w sumie ju¿ ich nie potrzebujemy, tylko przy kolejnym logowaniu trzeba znów p³ytk¹ kopiê œci¹gn¹c
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

		//generuje liste problemów
		QString message;

		for(int i = 0; i < verification.size(); ++i){
			message += QString::number(i+1) + ". " + verification[i] + "\n";
		}

		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Registration form validation"));
		messageBox.setText(message);
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else {
		
		if(dataSource->registerUser(regLoginEdit->text().toStdString(), emailEdit->text().toStdString(), regPassEdit->text().toStdString(),
			nameEdit->text().toStdString(), surnameEdit->text().toStdString()) == true){

			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Registration successful"));
			messageBox.setText(tr("Your registration has finished. Activate Your account to be able to login and get access to database."));
			messageBox.setIcon(QMessageBox::Information);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);
			messageBox.exec();

			if(dataSource->isLogged() == false){
				//jeœli nikt nie jest jeszcze zalogowany to przenosimy do strony logowania
				toolBox->setCurrentIndex(2);
				//czêsciowo wype³niam ju¿ dane usera
				activationLoginEdit->setText(regLoginEdit->text());
			}
		} else{
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Registration failed"));
			messageBox.setText(tr("Could not registrate. Please change login or email and try again. If problem continues contact producer."));
			messageBox.setIcon(QMessageBox::Critical);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);
			messageBox.exec();
		}
	}
}

void DataSourceWidget::onActivate()
{
	if(activationLoginEdit->text().isEmpty() == true || activationCodeEdit->text().isEmpty() == true){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Activation validation"));
		messageBox.setText(tr("Both fields: Login and Activation code must be filled. Please correct them and try again."));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else if(dataSource->tryActivateAccount(activationLoginEdit->text().toStdString(), activationCodeEdit->text().toStdString()) == true){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Activation successful"));
		messageBox.setText(tr("Given login has been successfully activated. You can now login using this account."));
		messageBox.setIcon(QMessageBox::Information);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();

		if(dataSource->isLogged() == false){
			//jeœli nikt nie jest jeszcze zalogowany to przenosimy do strony logowania
			toolBox->setCurrentIndex(0);
			//czêsciowo wype³niam ju¿ dane usera
			userEdit->setText(activationLoginEdit->text());
		}
	}else{
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Activation failed"));
		messageBox.setText(tr("Could not activate given login with provided activation code. Verify data and try again. If problem continiues please contact the producer."));
		messageBox.setIcon(QMessageBox::Critical);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}
}

void DataSourceWidget::onPerspectiveChange(int idx)
{
	auto currentPW = perspectiveManager.currentPerspectiveWidget();
	if(currentPW != nullptr){
		disconnect(currentPW, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(perspectiveContextMenu(QPoint)));
		disconnect(currentPW, SIGNAL(itemSelectionChanged()), this, SLOT(onPerspectiveSelectionChanged()));
	}

	try{
		//ustawiamy aktualn¹ perspektywê
		perspectiveManager.setCurrentPerspective(idx);
		//odœwie¿amy aktualn¹ perspektywê
		perspectiveManager.rebuildCurrentPerspective(filteredShallowCopy);

		currentPW = perspectiveManager.currentPerspectiveWidget();
		if(currentPW != nullptr){
			connect(currentPW, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(perspectiveContextMenu(QPoint)));
			connect(currentPW, SIGNAL(itemSelectionChanged()), this, SLOT(onPerspectiveSelectionChanged()));
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
			//wype³niamy content aktualnej perspektywy
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
		//ustawiamy aktualn¹ perspektywê
		contentManager.setCurrentContent(idx);

		//wype³niamy content aktualnej perspektywy
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

	filesToLoad.swap(std::set<int>());
	filesToUnload.swap(std::set<int>());
	filesToDownload.swap(std::set<int>());

	setCursor(Qt::WaitCursor);
	QApplication::processEvents();

	//generujemy menu kontekstowe - download, load, unload + updateShallowCopy
	QMenu menu;

	auto selectedItems = perspective->selectedItems();
	if(selectedItems.empty() == false){		
		generateItemSpecyficContextMenu(menu, perspective);
	}else{
		generateGeneralContextMenu(menu, perspective);
	}

	menu.addSeparator();

	generateCommonContextMenu(menu, perspective);

	setCursor(Qt::ArrowCursor);
	menu.exec(perspective->mapToGlobal(pos));

	currentPerspectiveItem = nullptr;
}

QString DataSourceWidget::formatFileSize(unsigned long long size)
{
	double num = size;
	QStringList list;
	list << "KB" << "MB" << "GB" << "TB";

	QStringListIterator i(list);
	QString unit("bytes");

	while(num >= 1024.0 && i.hasNext())
	{
		unit = i.next();
		num /= 1024.0;
	}

	return QString().setNum(num,'d',2) + " " + unit;
}

void DataSourceWidget::generateItemSpecyficContextMenu(QMenu & menu, QTreeWidget * perspective)
{
	auto load = menu.addAction(tr("Load"));
	auto unload = menu.addAction(tr("Unload"));
	menu.addSeparator();
	auto download = menu.addAction(tr("Download"));
	menu.addSeparator();

	auto refresh = menu.addAction(tr("Refresh status"));

	load->setEnabled(false);
	unload->setEnabled(false);
	download->setEnabled(false);
	refresh->setEnabled(false);

	//skoro coœ œci¹gam muszê poczekaæ!! nie przetwarzam reszty tylko pokazuje nizainicjalizowane menu
	if(currentDownloadRequest == nullptr){

		//aktywujemy podstaweowe operacje
		connect(refresh, SIGNAL(triggered()), this, SLOT(refreshStatus()));
		refresh->setEnabled(true);

		//sprawdzamy co to za item, potrzebujemy wszystkich plików z nim zwi¹zanych
		std::set<int> filesIDs;

		auto selectedItems = perspective->selectedItems();
		const auto & extensions = dataSource->fileDM->getSupportedFilesExtensions();				

		//sprawdzam czy to cos co mogê za³adowaæ?

		if(isItemLoadable(selectedItems[0]) == true){

			currentPerspectiveItem = selectedItems[0];
			//pobieram pliki dla wybranego elementu
			getItemsFiles(currentPerspectiveItem, filesIDs, filteredShallowCopy);
			//UTILS_ASSERT(filesIDs.empty() == false, "Brak danych w drzewie");

			if(filesIDs.empty() == false){
				//mam dane - mogê pracowaæ
				//wyci¹gam dane lokalne i zdalne
				//wyci¹gam dane za³¹dowane i nieza³adowane
				//na bazie tych info odpowiednio ³¹cze akcje ze slotami i aktywuje je + zapamiêtuje te info do wykonania ich!!

				//pomijamy wszystkie niekompatybilne z DM pliki

				std::set<int> dmOKFiles;

				//filtruje pliki obs³ugiwane przez DM
				FilesHelper::filterFiles(filesIDs, extensions, dmOKFiles, *(dataSource->fileStatusManager));

				//pliki do za³adowania
				FilesHelper::filterFiles(dmOKFiles, DataStatus(Local, Unloaded), filesToLoad, *(dataSource->fileStatusManager));

				//pliki do wy³adowania - EXPERIMENTAL
				FilesHelper::filterFiles(dmOKFiles, communication::Loaded, filesToUnload, *(dataSource->fileStatusManager));

				//pliki do œci¹gniêcia
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
}

void DataSourceWidget::generateGeneralContextMenu(QMenu & menu, QTreeWidget * perspective)
{
	//poszczeólne akcje
	auto loadAll = menu.addAction(tr("Load All"));
	auto unloadAll = menu.addAction(tr("Unload All"));
	auto downloadAll = menu.addAction(tr("Download All"));

	//skoro coœ œci¹gam muszê poczekaæ!! nie przetwarzam reszty tylko pokazuje nizainicjalizowane menu
	if(currentDownloadRequest == nullptr){

		const auto & extensions = dataSource->fileDM->getSupportedFilesExtensions();	

		std::set<int> allFiles;

		filteredFiles(allFiles);

		std::set<int> dmOkFiles;
		FilesHelper::filterFiles(allFiles, extensions, dmOkFiles, *(dataSource->fileStatusManager));
		//pliki do za³adowania
		FilesHelper::filterFiles(dmOkFiles, DataStatus(Local, Unloaded), filesToLoad, *(dataSource->fileStatusManager));
		//pliki do wy³adowania
		FilesHelper::filterFiles(dmOkFiles, communication::Loaded, filesToUnload, *(dataSource->fileStatusManager));
		//pliki do œci¹gniêcia
		FilesHelper::filterFiles(dmOkFiles, communication::Remote, filesToDownload, *(dataSource->fileStatusManager));

		if(filesToLoad.empty() == false){
			loadAll->setEnabled(true);
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoad()));
		}

		if(filesToUnload.empty() == false){
			unloadAll->setEnabled(true);
			connect(unloadAll, SIGNAL(triggered()), this, SLOT(onUnload()));
		}

		if(filesToDownload.empty() == false){
			downloadAll->setEnabled(true);
			connect(downloadAll, SIGNAL(triggered()), this, SLOT(onDownload()));
		}
	}else{
		loadAll->setEnabled(false);
		unloadAll->setEnabled(false);
		downloadAll->setEnabled(false);
	}
}

void DataSourceWidget::generateCommonContextMenu(QMenu & menu, QTreeWidget * perspective)
{
	auto saveProject = menu.addAction(tr("Save project as..."));
	auto loadProject = menu.addMenu(tr("Load project"));
	auto deleteProject = menu.addMenu(tr("Delete project"));
	menu.addSeparator();
	auto synch = menu.addAction(tr("Synchronize"));

	//skoro coœ œci¹gam muszê poczekaæ!! nie przetwarzam reszty tylko pokazuje nizainicjalizowane menu
	if(currentDownloadRequest == nullptr){

		connect(saveProject, SIGNAL(triggered()), this, SLOT(onSaveProject()));
		connect(synch, SIGNAL(triggered()), this, SLOT(updateShallowCopy()));

		for(auto it = projects.begin(); it != projects.end(); ++it){
			auto loadAction = loadProject->addAction(QString::fromUtf8(it->first.c_str()));
			auto deleteAction = deleteProject->addAction(QString::fromUtf8(it->first.c_str()));

			connect(loadAction, SIGNAL(triggered()), this, SLOT(onLoadProject()));
			connect(deleteAction, SIGNAL(triggered()), this, SLOT(onDeleteProject()));
		}

		if(projects.empty() == true){
			loadProject->setEnabled(false);
			deleteProject->setEnabled(false);
		}
	}else{
		saveProject->setEnabled(false);
		loadProject->setEnabled(false);
		deleteProject->setEnabled(false);
		synch->setEnabled(false);
	}
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
				//extra dodajê pliki specyficzne sesji
				FilesHelper::getSpecificFiles(motion->value()->session, filesIDs);
			}
			break;
		case FileContent:
			{
				auto file = dynamic_cast<FileItem*>(item);
				auto f = file->value();
				filesIDs.insert(f->fileID);
				//extra dodajê pliki specyficzne sesji
				FilesHelper::getSpecificFiles(f->isSessionFile() == true ? f->session : f->trial->session, filesIDs);
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
					//extra dodajê pliki specyficzne sesji
					FilesHelper::getSpecificFiles((*it)->session, filesIDs);
				}
			}
			break;
		case FilesGroupContent:
			{
				auto filesGroup = dynamic_cast<FilesGroupItem*>(item);
				for(auto it = filesGroup->value().begin(); it != filesGroup->value().end(); ++it){
					filesIDs.insert((*it)->fileID);
					//extra dodajê pliki specyficzne sesji
					FilesHelper::getSpecificFiles((*it)->isSessionFile() == true ? (*it)->session : (*it)->trial->session, filesIDs);
				}
			}
			break;
		case CustomContent:
			found = false;
			break;
		}
	}

	//jezeli nie znalazlem to ide w dó³ hierarchi perspektywy - mo¿e tam coœ znajdê
	if(found == false){
		int childrenCount = item->childCount();
		for(int i = 0; i < childrenCount; ++i){
			getItemsFiles(item->child(i), filesIDs, shallowCopy);
		}
	}	
}

void DataSourceWidget::updateShallowCopy()
{
	//lokujemy - nie mo¿na teraz zmieniæ uzytkownika z zewn¹trz
	//w sumie powinniœmy lokowac ca³e Ÿród³o
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*DataSourcePathsManager::instance());
	if(dataSource->isShallowCopyComplete() == true && dataSource->isShallowCopyCurrent() == true){
		//message box - nie musimy aktualizowaæ, mamy najœwie¿sz¹ wersjê
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Database synchronization"));
		messageBox.setText(tr("Local data already synchronized. Synchronization is not required. Proceed anyway?"));
		messageBox.setIcon(QMessageBox::Question);
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::No);
		int ret = messageBox.exec();
		if(ret == QMessageBox::No){
			return;
		}
	}

	performShallowCopyUpdate();
}

void DataSourceWidget::performShallowCopyUpdate()
{
	//generujemy œcie¿ki

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
	//zanim wystartujemy musimy zarejestrowaæ obiekt który bêdzie obserwowa³ zmiany w requeœcie pobierania + aktualizowa³ progress pobierania
	request->attach(this);
	registeredRequests[request.get()] = request;
	//startujemy request - dodajemy do kolejki, wiêc niekoniecznie po tym wywo³aniu zaczniemy pobieraæ
	//to operuje na communication manager - coœ mo¿e jednak pójœæ nie tak
	try{
		request->start();
	}catch(std::exception & ){
		//TODO
		//message box z informacj¹
	}catch(...){
		//TODO
		//message box z informacj¹
	}
}

bool DataSourceWidget::refreshShallowCopy()
{
	bool ret = true;

	communication::ShallowCopy shallowCopy;
	try{
		if(dataSource->buildShallowCopyFromLocalUserSpace(shallowCopy) == true){

			//ustawiam nowa p³ytka kopiê danych
			dataSource->setShallowCopy(shallowCopy);
			//odœwie¿am przefiltrowan¹ p³ytk¹ kopiê danych co wi¹¿e siê z uniewa¿nieniem dotychczasowych perspektyw
			QMetaObject::invokeMethod(this, "onFilterChange", Q_ARG(int, filterManager.currentFilterIndex()));
		}else{
			ret = false;
		}
	}catch(...){
		ret = false;
	}

	return ret;
}

void DataSourceWidget::filteredFiles(std::set<int> & files) const
{
	if(filteredShallowCopy.motionShallowCopy != nullptr){
		auto filesITEnd = filteredShallowCopy.motionShallowCopy->files.end(); 
		for(auto it = filteredShallowCopy.motionShallowCopy->files.begin(); it != filesITEnd; ++it){
			files.insert(it->first);
		}
	}
}


void DataSourceWidget::onDownload()
{
	// poziom 100MB jako limit ostrze¿enia
	static const long downloadSizeWarningLevel = 104857600;
	//wyliczmy ile miejsca potrzebujemy i ile mamy

	unsigned long long size = 0;

	for(auto it = filesToDownload.begin(); it != filesToDownload.end(); ++it){
		auto f = filteredShallowCopy.motionShallowCopy->files.find(*it);
		size += f->second->fileSize;
	}

	unsigned long long avaiable = dataSource->pathsManager->freeSpace(dataSource->pathsManager->userDataPath());

	//mno¿e razy 2 bo muszê to potem jeszcze do local storage wrzuciæ!!
	if(avaiable < (size << 1)){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Download problem"));
		messageBox.setText(tr("Download request requires %1 of free space while there is only %2 free space left. Download could not be continued. Please free some space or limit your download request.").arg(formatFileSize(size)).arg( formatFileSize(avaiable)));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else{
		//ok - mogê œci¹gaæ ale mo¿e tego jest sporo wiêc trzeba u¿ytkownika uœwiadomiæ
		if(size > downloadSizeWarningLevel){
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Download warning"));
			messageBox.setText(tr("Download request contains %1. This will take some time to download such amount of data. Are you sure you can wait some longer time?").arg(formatFileSize(size)));
			messageBox.setIcon(QMessageBox::Warning);
			messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			messageBox.setDefaultButton(QMessageBox::No);
			
			int ret = messageBox.exec();

			if(ret == QMessageBox::No){
				//u¿ytkownik siê nawróci³ - anulujemy
				return;
			}
		}

		//skoro tu jestem to zaczynamy œci¹gaæ
		try{
			//przygotuj request dla œci¹gania
			auto downloadRequest = dataSource->generateDownloadFilesRequest(filesToDownload);
			//zacznij przetwarzaæ request
			processDataDownload(nullptr, downloadRequest);
			
		}catch(std::exception & e){
			//nie uda³o siê utworzyæ/przygotowaæ requesta wiêc info o b³êdzie
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Download preparation error"));
			messageBox.setText(tr("Error while preparing download request. Error description: ") + QString::fromUtf8(e.what()));
			messageBox.setIcon(QMessageBox::Critical);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);

			messageBox.exec();
		}
	}
}

void DataSourceWidget::onLoad()
{
	loadFiles(filesToLoad);
}

void DataSourceWidget::loadSubjectHierarchy(const std::map<int, std::vector<core::ObjectWrapperPtr>> & loadedFilesObjects)
{
	typedef std::map<int, std::set<int>> MotionFiles;
	typedef std::map<int, std::pair<std::set<int>, MotionFiles>> SessionFiles;
	typedef std::map<int, SessionFiles> SubjectFiles;


	class JointsInitializer : public core::IDataInitializer
	{
	public:
		JointsInitializer(const core::ObjectWrapperConstPtr & dataWrapper, const core::ObjectWrapperConstPtr & modelWrapper) : 
		  dataWrapper(dataWrapper), modelWrapper(modelWrapper)
		  {

		  }

		  virtual void initialize(core::ObjectWrapperPtr & object)
		  {
			  kinematic::SkeletalDataPtr data;
			  kinematic::SkeletalModelPtr model;
			  if(dataWrapper->tryGet(data) == true && modelWrapper->tryGet(model) == true && data != nullptr && model != nullptr){
				  kinematic::JointAnglesCollectionPtr joints(new kinematic::JointAnglesCollection());
				  joints->setSkeletal(model, data);
				  object->trySet(joints);
			  }
		  }

	private:
		core::ObjectWrapperConstPtr dataWrapper;
		core::ObjectWrapperConstPtr modelWrapper;
	};


	//TODO
	//zainicjowaæ wskaŸnik do serwisu!!

	auto subjectService = core::queryServices<PluginSubject::ISubjectService>(dataSource->serviceManager);

	if(subjectService == nullptr){
		return;
	}

	core::NotifyBlocker<core::IMemoryDataManager> blocker(*(dataSource->memoryDM));

	//buduje mapê hierarchii subject -> session -> motion -> files
	//na bazie tej mapy bêdê realizowa³ hierarchiê pluginu subject
	
	//TODO
	//zrewidowaæ plugin subject!!
	//obiekty tej hierarchii powinny byæ edytowalne po stronie Ÿród³a aby mog³o elastyczniej nimi zarz¹dzaæ!!
	SubjectFiles subjectHierarchy;

	auto itEND = loadedFilesObjects.end();
	for(auto it = loadedFilesObjects.begin(); it != itEND; ++it){

		if(it->first < 0){
			auto s = filteredShallowCopy.motionShallowCopy->sessions.find(- it->first);

			if(s != filteredShallowCopy.motionShallowCopy->sessions.end()){
				subjectHierarchy[s->second->performerConf->performer->performerID][s->second->sessionID].first.insert(it->first);
			}else{
				//TODO
				//INFO o nieobs³ugiwanym pliku
			}

		}else{
			auto fileIT = filteredShallowCopy.motionShallowCopy->files.find(it->first);
			if(fileIT == filteredShallowCopy.motionShallowCopy->files.end()){
				//TODO
				//INFO o nieobs³ugiwanym pliku
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
		//tworzê subject jeœli to konieczne!!

		PluginSubject::SubjectPtr subPtr;

		auto subIT = subjectsMapping.find(subjectIT->first);
		if(subIT != subjectsMapping.end()){
			//mam subjecta - nie musze ju¿ nic robiæ
			subPtr = subIT->second.first->get();
		}else{
			//tworzê subjecta
			subPtr = subjectService->createSubject();
			//dodajê do DM
			auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, subPtr);

			core::MetadataPtr meta(new core::Metadata(ow));
			std::stringstream label;

			auto pIT = filteredShallowCopy.medicalShallowCopy->patients.find(subjectIT->first);

			if(pIT == filteredShallowCopy.medicalShallowCopy->patients.end()){
				label << "Subject " << subjectIT->first;
			}else{
				label << pIT->second->name << ", " << pIT->second->surname;
				addPatientObject(pIT->second, subPtr->getID());
			}

			//auto s = filteredShallowCopy.medicalShallowCopy->patients.find(subjectIT->first)->second;
			meta->setValue("label", label.str());

			core::IMemoryDataManager::addData(dataSource->memoryDM, meta);

			//zapamiêtujê mapowanie
			subjectsMapping[subjectIT->first].first = ow;
		}

		//mam subjecta, mogê iœæ dalej do sesji
		for(auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){

			PluginSubject::SessionPtr sPtr;

			auto sIT = sessionsMapping.find(sessionIT->first);
			if(sIT != sessionsMapping.end()){
				//mam subjecta - nie musze ju¿ nic robiæ
				sPtr = sIT->second.first->get();
			}else{
				//tworzê sesjê
				//generujê zbiór ow dla sesji
				std::vector<core::ObjectWrapperConstPtr> sessionObjects;
				for(auto fIT = sessionIT->second.first.begin(); fIT != sessionIT->second.first.end(); ++fIT){
					//pobieram obiekty
					const auto & objects = loadedFilesObjects.find(*fIT)->second;
					//kopiuje
					for(auto oIT = objects.begin(); oIT != objects.end(); ++oIT){
						sessionObjects.push_back(*oIT);
					}
				}

				auto s = filteredShallowCopy.motionShallowCopy->sessions.find(sessionIT->first)->second;

				//dane antropometryczne
				auto antro = createAntropometricData(s->performerConf->attrs);
				auto antroOW = core::IMemoryDataManager::addData(dataSource->memoryDM, antro);
				sessionObjects.push_back(antroOW);
				sessionsMapping[sessionIT->first].second.push_back(antroOW);

				sPtr = subjectService->createSession(subPtr, sessionObjects);
				//dodajê do DM
				auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, sPtr);

				core::MetadataPtr meta(new core::Metadata(ow));
				
				meta->setValue("label", s->sessionName);
				meta->setValue("EMGConf", boost::lexical_cast<std::string>(s->emgConf));
				meta->setValue("data", s->sessionDate);
				if(s->groupAssigment != nullptr){
					meta->setValue("groupID", boost::lexical_cast<std::string>(s->groupAssigment->sessionGroupID));
					auto sgIT = filteredShallowCopy.motionMetaData.sessionGroups.find(s->groupAssigment->sessionGroupID);
					if(sgIT != filteredShallowCopy.motionMetaData.sessionGroups.end()){
						meta->setValue("groupName", sgIT->second.sessionGroupName);
					}
				}

				core::IMemoryDataManager::addData(dataSource->memoryDM, meta);

				//zapamiêtujê mapowanie
				sessionsMapping[sessionIT->first].first = ow;
			}

			//mam sesjê - mogê iœæ dalej z motionami!!
			for(auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){

				PluginSubject::MotionPtr mPtr;

				auto mIT = motionsMapping.find(motionIT->first);
				if(mIT != motionsMapping.end()){
					//mam subjecta - nie musze ju¿ nic robiæ
					//TODO
					//nie powinno mnie tu byæ wg aktualnego dzia³ania pluginu subject!!
					mPtr = mIT->second.first->get();
				}else{
					//tworzê sesjê
					//generujê zbiór ow dla motiona
					std::vector<core::ObjectWrapperConstPtr> motionObjects;
					for(auto fIT = motionIT->second.begin(); fIT != motionIT->second.end(); ++fIT){
						//pobieram obiekty
						const auto & objects = loadedFilesObjects.find(*fIT)->second;
						//kopiuje
						for(auto oIT = objects.begin(); oIT != objects.end(); ++oIT){
							motionObjects.push_back(*oIT);
						}
					}


					//sprawdzamy joint angles - jesli nie ma budujemy i dodajemy do DM
					core::ObjectWrapperConstPtr dataWrapper;
					core::ObjectWrapperConstPtr modelWrapper;
					for (auto it = motionObjects.begin(); it != motionObjects.end(); ++it) {
						if ((*it)->isSupported(typeid(kinematic::JointAnglesCollection))) {
							return;
						} else if ((*it)->isSupported(typeid(kinematic::SkeletalData))) {
							dataWrapper = *it;
							break;
						}
					}

					modelWrapper = sPtr->getWrapperOfType(typeid(kinematic::SkeletalModel));
					core::ObjectWrapperPtr jointsWrapper;
					if (dataWrapper && modelWrapper) {
						jointsWrapper = core::IMemoryDataManager::addData(dataSource->memoryDM, kinematic::JointAnglesCollectionPtr(), core::DataInitializerPtr(new JointsInitializer(dataWrapper, modelWrapper)));						
						motionObjects.push_back(jointsWrapper);
						motionsMapping[motionIT->first].second.push_back(jointsWrapper);
					}

					mPtr = subjectService->createMotion(sPtr,motionObjects);

					if(jointsWrapper != nullptr){
						jointsWrapper->setName(mPtr->getLocalName() + " joints");
						jointsWrapper->setSource("newCommunication->motion->" + mPtr->getLocalName());
					}

					//dodajê do DM
					auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, mPtr);

					core::MetadataPtr meta(new core::Metadata(ow));
					auto s = filteredShallowCopy.motionShallowCopy->trials.find(motionIT->first)->second;
					meta->setValue("label", s->trialName);

					core::IMemoryDataManager::addData(dataSource->memoryDM, meta);

					//zapamiêtujê mapowanie
					motionsMapping[motionIT->first].first = ow;
				}
			}
		}
	}
}

void DataSourceWidget::addPatientObject(const webservices::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectID subjectID)
{
	//generujê listê schorzeñ
	std::vector<communication::Disorder> disorders;
	for(auto it = patient->disorders.begin(); it != patient->disorders.end(); ++it)	{

		communication::Disorder dis;
		dis.focus = it->second.focus;
		dis.diagnosisDate = it->second.diagnosisDate;
		dis.comments = it->second.comments;
		dis.name = it->second.disorder->name;

		disorders.push_back(dis);
	}
	
	PatientPtr pPtr(new Patient(subjectID, patient->name, patient->surname, patient->birthDate,
		Patient::decodeGender(patient->gender), core::shared_ptr<const QPixmap>(), disorders));

	//dodajê do DM
	auto ow = core::IMemoryDataManager::addData(dataSource->memoryDM, pPtr);

	//zapamiêtuje
	patientsMapping[patient->patientID].first = ow;
}

core::shared_ptr<communication::AntropometricData> DataSourceWidget::createAntropometricData(const webservices::MotionShallowCopy::Attrs & attrs)
{
	auto antro = core::shared_ptr<communication::AntropometricData>(new communication::AntropometricData());
	antro->bodyMass = communication::AntropometricData::value_type(getAntropometricValue("BodyMass", attrs), "kg");
	antro->height = communication::AntropometricData::value_type(getAntropometricValue("Height", attrs), "mm");
	antro->interAsisDistance = communication::AntropometricData::value_type(getAntropometricValue("InterAsisDistance", attrs), "mm");

	antro->leftLegLength = communication::AntropometricData::value_type(getAntropometricValue("LeftLegLength", attrs), "mm");
	antro->rightLegLenght = communication::AntropometricData::value_type(getAntropometricValue("RightLegLenght", attrs), "mm");

	antro->leftKneeWidth = communication::AntropometricData::value_type(getAntropometricValue("LeftKneeWidth", attrs), "mm");
	antro->rightKneeWidth = communication::AntropometricData::value_type(getAntropometricValue("RightKneeWidth", attrs), "mm");

	antro->leftAnkleWidth = communication::AntropometricData::value_type(getAntropometricValue("LeftAnkleWidth", attrs), "mm");
	antro->rightAnkleWidth = communication::AntropometricData::value_type(getAntropometricValue("RightAnkleWidth", attrs), "mm");

	antro->leftCircuitThigh = communication::AntropometricData::value_type(getAntropometricValue("LeftCircuitThigh", attrs), "mm");
	antro->rightCircuitThight = communication::AntropometricData::value_type(getAntropometricValue("RightCircuitThight", attrs), "mm");

	antro->leftCircuitShank = communication::AntropometricData::value_type(getAntropometricValue("LeftCircuitShank", attrs), "mm");
	antro->rightCircuitShank = communication::AntropometricData::value_type(getAntropometricValue("RightCircuitShank", attrs), "mm");

	antro->leftShoulderOffset = communication::AntropometricData::value_type(getAntropometricValue("LeftShoulderOffset", attrs), "mm");
	antro->rightShoulderOffset = communication::AntropometricData::value_type(getAntropometricValue("RightShoulderOffset", attrs), "mm");

	antro->leftElbowWidth = communication::AntropometricData::value_type(getAntropometricValue("LeftElbowWidth", attrs), "mm");
	antro->rightElbowWidth = communication::AntropometricData::value_type(getAntropometricValue("RightElbowWidth", attrs), "mm");

	antro->leftWristWidth = communication::AntropometricData::value_type(getAntropometricValue("LeftWristWidth", attrs), "mm");
	antro->rightWristWidth = communication::AntropometricData::value_type(getAntropometricValue("RightWristWidth", attrs), "mm");

	antro->leftWristThickness = communication::AntropometricData::value_type(getAntropometricValue("LeftWristThickness", attrs), "mm");
	antro->rightWristThickness = communication::AntropometricData::value_type(getAntropometricValue("RightWristThickness", attrs), "mm");

	antro->leftHandWidth = communication::AntropometricData::value_type(getAntropometricValue("LeftHandWidth", attrs), "mm");
	antro->rightHandWidth = communication::AntropometricData::value_type(getAntropometricValue("RightHandWidth", attrs), "mm");

	antro->leftHandThickness = communication::AntropometricData::value_type(getAntropometricValue("LeftHandThickness", attrs), "mm");
	antro->rightHandThickness = communication::AntropometricData::value_type(getAntropometricValue("RightHandThickness", attrs), "mm");

	return antro;
}

float DataSourceWidget::getAntropometricValue(const std::string & attribute, const webservices::MotionShallowCopy::Attrs & attrs, float defValue)
{
	auto it = attrs.find(attribute);
	if(it != attrs.end()){
		return boost::lexical_cast<float>(it->second);
	}

	return defValue;
}

void DataSourceWidget::onUnload()
{
	unloadFiles(filesToUnload);
}

void DataSourceWidget::unloadSubjectHierarchy(const std::set<int> & unloadedFilesIDs)
{
	typedef std::map<int, std::set<int>> MotionFiles;
	typedef std::map<int, std::pair<std::set<int>, MotionFiles>> SessionFiles;
	typedef std::map<int, SessionFiles> SubjectFiles;

	//zainicjowaæ wskaŸnik do serwisu!!

	auto subjectService = core::queryServices<PluginSubject::ISubjectService>(dataSource->serviceManager);

	if(subjectService == nullptr){
		return;
	}

	core::NotifyBlocker<core::IMemoryDataManager> blocker(*(dataSource->memoryDM));

	//buduje mapê hierarchii subject -> session -> motion -> files
	//na bazie tej mapy bêdê realizowa³ hierarchiê pluginu subject

	//TODO
	//zrewidowaæ plugin subject!!
	//obiekty tej hierarchii powinny byæ edytowalne po stronie Ÿród³a aby mog³o elastyczniej nimi zarz¹dzaæ!!
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
					//mam motion - sprawdzam czy mogê go usun¹æ
					std::set<int> motionFiles;
					FilesHelper::getMotionFiles(motionIT->first, filteredShallowCopy, motionFiles);

					//teraz robiê ró¿nicê tego co jest w p³ytkiej kopii a tego co mam wy³adowaæ
					std::vector<int> diff(max(motionFiles.size(), motionIT->second.size()));
					auto diffIT = std::set_difference(motionFiles.begin(), motionFiles.end(), motionIT->second.begin(), motionIT->second.end(), diff.begin());

					if(diffIT == diff.begin()){
						//to znaczy ¿e usun¹³em wszystkie pliki motiona -> mogê usuwaæ motiona
						for(auto rIT = mIT->second.second.begin(); rIT != mIT->second.second.end(); ++rIT){
							dataSource->memoryDM->removeData(*rIT);
						}
						
						dataSource->memoryDM->removeData(mIT->second.first);

						motionsMapping.erase(mIT);
					}else{
						//TODO
						//ta sytuacja obecnie nie powinna mieæ miejsca ze wzglêdu na specyfikê pluginu subject!!
						throw std::runtime_error("Plugin subject to redesign");
					}
				}else{
					//TODO
					//tutaj mnie nie powinno byæ - chyba ¿e plugin subject wczeœniej nie dzia³a³
					//w takim razie nie reaguje
				}
			}

			//usun¹³em motiony sesji - czas zobaczyæ czy sesja jeszcze czymœ dysponuje
			PluginSubject::SessionPtr sPtr;

			auto sIT = sessionsMapping.find(sessionIT->first);
			if(sIT != sessionsMapping.end()){
				//mam subjecta - nie musze ju¿ nic robiæ
				sPtr = sIT->second.first->get();
				PluginSubject::Motions motions;
				sPtr->getMotions(motions);
				if(motions.empty() == true){
					//sesja jest pusta - do usuniêcia
					for(auto rIT = sIT->second.second.begin(); rIT != sIT->second.second.end(); ++rIT){
						dataSource->memoryDM->removeData(*rIT);
					}

					dataSource->memoryDM->removeData(sIT->second.first);
					sessionsMapping.erase(sIT);
				}
			}else{
				//TODO
				//tutaj mnie nie powinno byæ - chyba ¿e plugin subject wczeœniej nie dzia³a³
				//w takim razie nie reaguje
			}
		}

		//usuwam subject jeœli to konieczne!!

		PluginSubject::SubjectPtr subPtr;

		auto subIT = subjectsMapping.find(subjectIT->first);
		if(subIT != subjectsMapping.end()){
			//mam subjecta - nie musze ju¿ nic robiæ
			subPtr = subIT->second.first->get();
			PluginSubject::Sessions sessions;
			subPtr->getSessions(sessions);
			if(sessions.empty() == true){

				for(auto rIT = subIT->second.second.begin(); rIT != subIT->second.second.end(); ++rIT){
					dataSource->memoryDM->removeData(*rIT);
				}

				dataSource->memoryDM->removeData(subIT->second.first);

				//musze jeszcze usun¹æ pacjenta jeœli mam!!
				auto patientIT = patientsMapping.find(subIT->first);
				if(patientIT != patientsMapping.end()){

					for(auto rIT = patientIT->second.second.begin(); rIT != patientIT->second.second.end(); ++rIT){
						dataSource->memoryDM->removeData(*rIT);
					}

					dataSource->memoryDM->removeData(patientIT->second.first);

					patientsMapping.erase(patientIT);
				}

				subjectsMapping.erase(subIT);
			}
		}else{
			//TODO
			//tutaj mnie nie powinno byæ - chyba ¿e plugin subject wczeœniej nie dzia³a³
			//w takim razie nie reaguje
		}
	}
}

void DataSourceWidget::unloadSubjectHierarchy()
{
	for(auto it = motionsMapping.begin(); it != motionsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			dataSource->memoryDM->removeData(*rIT);
		}

		dataSource->memoryDM->removeData(it->second.first);
	}

	for(auto it = sessionsMapping.begin(); it != sessionsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			dataSource->memoryDM->removeData(*rIT);
		}

		dataSource->memoryDM->removeData(it->second.first);
	}

	for(auto it = subjectsMapping.begin(); it != subjectsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			dataSource->memoryDM->removeData(*rIT);
		}

		dataSource->memoryDM->removeData(it->second.first);
	}

	for(auto it = patientsMapping.begin(); it != patientsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			dataSource->memoryDM->removeData(*rIT);
		}

		dataSource->memoryDM->removeData(it->second.first);
	}

	patientsMapping.swap(std::map<int, MappingValue>());
	subjectsMapping.swap(std::map<int, MappingValue>());
	sessionsMapping.swap(std::map<int, MappingValue>());
	motionsMapping.swap(std::map<int, MappingValue>());
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

			//rozpocznij odœwie¿anie widgeta w osobnym w¹tku
			downloadRefreshTimer.start(40);
		}
		break;

	case IDownloadRequest::FinishedCancel:
		//info co siê sta³o
		downloadCanceled = true;
		finishDownloadRequest();
		break;

	case IDownloadRequest::FinishedError:
		{
			downloadCrashed = true;
			//info co siê sta³o
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
	//zakoñcz odœwie¿anie widgeta w osobnym w¹tku
	downloadRefreshTimer.stop();
	localDataLoader.reset(new LocalDataLoader(this));
	//synchronizujemy to co siê uda³o w ca³oœci pobraæ (zapisujemy do loaclStorage + informacja co siê sta³o
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

	//zapamiêtuje sobie pliki które siê dobrze œci¹gne³y
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

void DataSourceWidget::loadFiles(const std::set<int> & files)
{
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();

	core::NotifyBlocker<core::IFileDataManager> blocker(*(dataSource->fileDM));

	//! £aduje pliki do DM
	std::set<int> loadedFiles;
	std::map<int, std::vector<core::ObjectWrapperPtr>> loadedFilesObjects;
	std::map<int, std::string> loadingErrors;
	std::vector<int> unknownErrors;	

	for(auto it = files.begin(); it != files.end(); ++it){
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

	//w loadedFilesObjects mamy info o plikach i zwi¹zanych z nimi obiektach domenowych
	//próbujemy teraz przez plugin subject realizowaæ hierarchiê danych

	loadSubjectHierarchy(loadedFilesObjects);


	if(loadingErrors.empty() == true && unknownErrors.empty() == true){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Loading info"));
		messageBox.setText(tr("Data loaded successfully to application."));
		messageBox.setIcon(QMessageBox::Information);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);

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

		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Loading warning"));
		messageBox.setText(message);
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);

		messageBox.exec();
	}

	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::unloadFiles(const std::set<int> & files, bool showMessage)
{
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();

	core::NotifyBlocker<core::IFileDataManager> blocker(*(dataSource->fileDM));

	//próbujemy teraz przez plugin subject realizowaæ hierarchiê danych

	unloadSubjectHierarchy(files);

	//! £aduje pliki do DM
	std::set<int> unloadedFiles;
	std::map<int, std::string> unloadingErrors;
	std::vector<int> unknownErrors;

	for(auto it = files.begin(); it != files.end(); ++it){
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

	if(showMessage == true){
		if(unloadingErrors.empty() == true && unknownErrors.empty() == true){
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Unloading info"));
			messageBox.setText(tr("Data unloaded successfully."));
			messageBox.setIcon(QMessageBox::Information);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);

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

			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Unloading warning"));
			messageBox.setText(message);
			messageBox.setIcon(QMessageBox::Warning);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);

			messageBox.exec();
		}
	}

	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::saveProject(const std::string & projectName, const std::set<int> & projectFiles)
{
	projects[projectName] = projectFiles;

	trySaveProjects();
}

void DataSourceWidget::deleteProject(const std::string & projectName)
{
	projects.erase(projectName);

	trySaveProjects();
}

void DataSourceWidget::loadProject(const std::string & projectName)
{
	//make intersection of user project with data in shallow
	auto const & projectFiles = projects.find(projectName)->second;

	std::vector<int> accessibleFiles;
	auto filesITEnd = filteredShallowCopy.motionShallowCopy->files.end();
	for(auto it = projectFiles.begin(); it != projectFiles.end(); ++it){
		if(filteredShallowCopy.motionShallowCopy->files.find(*it) != filesITEnd){
			accessibleFiles.push_back(*it);
		}
	}

	if(accessibleFiles.size() < projectFiles.size()){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Loading project warning"));
		messageBox.setText(tr("Some project files are no longer accessible. Contact data owner to grant required privilages. Do You want to load the project without those files now?"));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::Yes);

		int ret = messageBox.exec();

		if(ret == QMessageBox::No){
			return;
		}
	}

	std::set<int> toVerify(accessibleFiles.begin(), accessibleFiles.end());
	std::set<int> dmOKFiles;
	//filtruje pliki obs³ugiwane przez DM
	const auto & extensions = dataSource->fileDM->getSupportedFilesExtensions();
	FilesHelper::filterFiles(toVerify, extensions, dmOKFiles, *(dataSource->fileStatusManager));

	filesToDownload.swap(std::set<int>());

	//pliki do œci¹gniêcia
	FilesHelper::filterFiles(dmOKFiles, communication::Remote, filesToDownload, *(dataSource->fileStatusManager));

	if(filesToDownload.empty() == false){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Loading project warning"));
		messageBox.setText(tr("Some project files must are missing. Data download is required. Do You want to continue?"));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::Yes);

		int ret = messageBox.exec();

		if(ret == QMessageBox::No){
			return;
		}

		//TODO
		//obs³uga œci¹gania - jak siê ca³e skoñczy to ³¹duje projekt z powodzeniem, jak tylko czêsciowo to warning!!

	}

	filesToDownload.swap(std::set<int>());	
	filesToLoad.swap(std::set<int>());	
	//pliki do za³adowania
	FilesHelper::filterFiles(dmOKFiles, Local, filesToLoad, *(dataSource->fileStatusManager));

	unloadFiles(filesLoadedToDM, false);

	loadFiles(filesToLoad);
}

void DataSourceWidget::onSaveProject()
{
	//TODO
	//widget z opcj¹ podania nazwy
	//nazwa staje siê nazwa projektu jezeli jeszcze takiego nie ma, jak jest to pytam czy nadpisaæ
	bool again = false;
	do{
		QInputDialog input;
		input.setInputMode(QInputDialog::TextInput);
		input.setLabelText(tr("Project name:"));
		input.setOkButtonText(tr("Save"));
		input.setWindowTitle(tr("Save project as..."));
		int ret = input.exec();
		
		if(!ret){
			return;
		}

		if(input.textValue().isEmpty() == true){
			again = true;
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Save project warning"));
			messageBox.setText(tr("Project name can not be empty. Please give a name to the project and continue."));
			messageBox.setIcon(QMessageBox::Warning);
			messageBox.setStandardButtons(QMessageBox::Ok);
			messageBox.setDefaultButton(QMessageBox::Ok);
			messageBox.exec();
		}else{
			again = false;
			//realizacja zapisu
			saveProject(input.textValue().toStdString(), filesLoadedToDM);
		}
	}while(again);

}

void DataSourceWidget::onDeleteProject()
{
	auto action = qobject_cast<QAction*>(sender());

	deleteProject(action->text().toStdString());
}

void DataSourceWidget::onLoadProject()
{
	auto action = qobject_cast<QAction*>(sender());

	loadProject(action->text().toStdString());
}

void DataSourceWidget::trySaveProjects()
{
	if(projects.empty() == true){
		std::string projectsFile(DataSourcePathsManager::instance()->projectsPath().filename().string());
		if(DataSourceLocalStorage::instance()->fileIsLocal(projectsFile) == true){
			try{
				DataSourceLocalStorage::instance()->removeFile(projectsFile);
			}catch(...){

			}
		}
		return;
	}

	std::ofstream projectsOut(DataSourcePathsManager::instance()->projectsPath().string());

	if(projectsOut.is_open() == false){
		//TODO
		//b³¹d zapisu
	}

	try{

	for(auto it = projects.begin(); it != projects.end(); ++it){
		projectsOut << it->first << ":";
		auto fIT = it->second.begin();
		projectsOut << *fIT;
		++fIT;
		for( ; fIT != it->second.end(); ++ fIT){
			projectsOut<< "," << *fIT;
		}
		projectsOut << ";" << std::endl;
	}

	projectsOut.close();

	DataSourceLocalStorage::instance()->loadFile(DataSourcePathsManager::instance()->projectsPath());

	}catch(...){

	}
}

void DataSourceWidget::tryLoadProjects()
{
	try{
		
		if(DataSourceLocalStorage::instance()->fileIsLocal(DataSourcePathsManager::instance()->projectsPath().filename().string()) == false){
			return;
		}

		DataSourceLocalStorage::instance()->extractFile(DataSourcePathsManager::instance()->projectsPath().filename().string(), DataSourcePathsManager::instance()->projectsPath());

		std::ifstream projectsInput(DataSourcePathsManager::instance()->projectsPath().string());

		while(projectsInput.good()){

			std::string line;
			std::getline(projectsInput, line);
			
			if(line.empty() == true || line.size() < 4){
				continue;
			}

			//parsujemy
			std::string::size_type pos = line.find(':', 1);

			if(pos != std::string::npos){
				using namespace boost;
				//mamy nazwê
				std::string projectName(line.substr(0, pos));
				std::set<int> files;
				//szukam plików
				static const char_separator<char> sep(":,;");
				auto filesString = line.substr(pos+1, line.size()-1);
				tokenizer< char_separator<char> > tokens(filesString, sep);
				for(auto it = tokens.begin(); it != tokens.end(); ++it){

					if((*it).empty() == false){
						try{
							files.insert(boost::lexical_cast<int>(*it));
						}catch(...){

						}
					}
				}

				if(files.empty() == false){
					projects[projectName] = files;
				}
			}
		}
	}catch(...){

	}
}

void DataSourceWidget::showUserData()
{
	setCurrentWidget(userDataTab);
}

void DataSourceWidget::showConfiguration()
{
	setCurrentWidget(configTab);
}

void DataSourceWidget::showPatientCard()
{
	setCurrentWidget(motionDataTab);
}

void DataSourceWidget::setPatientCard(webservices::MedicalShallowCopy::Patient * patient, webservices::MotionShallowCopy::Performer * subject)
{
	perspectiveManager.currentPerspectiveWidget()->clearSelection();
	patientCardManager.currentPatientCard()->setPatient(patient, subject, QPixmap(), dataSource->currentUser_.userData());
}
