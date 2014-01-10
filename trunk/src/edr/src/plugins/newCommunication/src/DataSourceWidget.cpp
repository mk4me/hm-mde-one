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
#include <QtCore/QSettings>

#include <openssl/aes.h>

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>

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
#include <corelib/IFileDataManager.h>
#include <corelib/IParserManagerReader.h>
#include <corelib/PluginCommon.h>
#include <plugins/video/Wrappers.h>
#include <plugins/c3d/C3DCollections.h>
#include <boost/lexical_cast.hpp>
#include "TreeBuilder.h"

//Dla wersji demo wystarczy zdefiniować DEMO_VERSION_COMMUNICATION
//aby zablokowac możliwość ściągania danych z bazy danych

//#define DEMO_VERSION_COMMUNICATION


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

	//jeśli mamy synchronizację to próbujemy parsować dane, ustawić je, jeśli wszystko ok to ładujemy do localstorage
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
		//od razu ładujemy do local storage
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

	//definitywnie kończymy pobieranie i jego obsługę
	if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
		//TODO - jedna wspolna metoda
		//usuwam płytką kopię
		for(auto it = sourceWidget->downloadedFiles.begin(); it != sourceWidget->downloadedFiles.end(); ++it){
			try{
				core::Filesystem::deleteFile(*it);
			}catch(...){

			}
		}
	}else{
		//nie był to shallowCopy więc zwykły transfer - muszę odświezyć status plików!!
		QMetaObject::invokeMethod(sourceWidget, "refreshStatusAfterDownload", Qt::BlockingQueuedConnection);
	}

	QMetaObject::invokeMethod(this, "showFinalMessage", Qt::BlockingQueuedConnection);
	if(sourceWidget->currentDownloadRequest == sourceWidget->shallowCopyRequest){
		sourceWidget->shallowCopyRequest.reset();
	}

	std::set<int>().swap(sourceWidget->filesToDownload);
	std::vector<std::string>().swap(sourceWidget->downloadedFiles);

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
	//obsługa komunikatu
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
		//błąd pobierania
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

	//próbujemy chować po 3 sekundach
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
    // wczytanie ostatnio użytego loginu i hasła
    loadCredentials();
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

	//inicjuję karty pacjenta i ustawiam domyślną
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

	//ustawiamy aktualną perspektywę
	perspectiveComboBox->setCurrentIndex(0);

	dataViewWidget->layout()->addWidget(downloadStatusWidget);

	loginEventFilter = new LoginEventFilter(this);
	loginPage->installEventFilter(loginEventFilter);
	userEdit->installEventFilter(loginEventFilter);
	passwordEdit->installEventFilter(loginEventFilter);

#ifdef DEMO_VERSION_COMMUNICATION

	dataSource->setOfflineMode(true);
	onLogin("matiegon", "Matiegon9");
	//onLogin("bdrdemo", ";bdrdemo");
	
	setCurrentWidget(motionDataTab);
	setTabEnabled(indexOf(configTab), false);
	setTabEnabled(indexOf(userDataTab), false);
	
#else
	
	setTabEnabled(indexOf(motionDataTab), false);
    // TODO : czy userDataTab byl potrzebny?
	setTabEnabled(indexOf(userDataTab), false);

	setCurrentWidget(configTab);
#endif
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
	//odświeżam bieżącą
	contentManager.refreshCurrentContent(perspectiveManager.currentPerspectiveWidget(), filteredShallowCopy, perspectiveManager.currentPerspective().get(),
		filteredShallowCopyStatus.get(),dataSource->fullShallowCopyStatus.get(), true);

	//aktualna perspektywa jest już ok
	perspectivesContent[perspectiveManager.currentPerspectiveWidget()] = contentManager.currentContentIndex();

	//sprawdzam czy mam jakiegoś selecta - jeżeli nie to próbuje coś zaznaczyć

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
	//ustaw kursor na myślenie
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();
	//odświeżam status plików
	//dataSource->fileStatusManager->refreshFilesStatus();
	dataSource->refreshFileManager();
	//odświeżam status pełnej płytkiej kopii
	dataSource->fullShallowCopyStatus->setShallowCopy(&dataSource->fullShallowCopy);
	//odświeżam status przefiltrowanej płytkiej kopii
	filteredShallowCopyStatus->setShallowCopy(&filteredShallowCopy);
	//unieważniam wszystkie perspektywy
	invalidatePerspectivesContent();
	//odświeżam bierzącą perspektywę
	refreshCurrentPerspectiveContent();
	//przywracam kursor
	setCursor(Qt::ArrowCursor);
}

void DataSourceWidget::refreshStatus(const std::set<int> & filesIDs)
{
	//odświeżam status plików
	dataSource->fileStatusManager->refreshFilesStatus(filesIDs, dataSource->localStorage);
	//odświeżam status pełnej płytkiej kopii
	dataSource->fullShallowCopyStatus->refreshDataStatus(filesIDs);
	//odświeżam status przefiltrowanej płytkiej kopii
	filteredShallowCopyStatus->refreshDataStatus(filesIDs);
	//unieważniam wszystkie perspektywy
	invalidatePerspectivesContent();
	//odświeżam bieżącą
	refreshCurrentPerspectiveContent();
}

void DataSourceWidget::refreshStatusAfterDownload()
{
	refreshStatus(filesToDownload);
}


QIcon DataSourceWidget::statusIcon(const communication::DataStorage storage, const communication::DataUsage usage)
{
	//zaczynamy ze sposobem składowania
	QPixmap s;

	switch(storage){
	case Local:
		s = QPixmap(QString::fromUtf8(":/newCommunication/icons/lokalne.png"));
		break;

	default:
	case Remote:
		s = QPixmap(QString::fromUtf8(":/newCommunication/icons/zdalne.png"));
		break;
	case PartiallyLocal:
		s = QPixmap(QString::fromUtf8(":/newCommunication/icons/pol-na-pol-green.png"));
		break;
	}

	// teraz stan użycia
	QPixmap u;

	switch(usage){
	case Loaded:
		u = QPixmap(QString::fromUtf8(":/newCommunication/icons/zaladowane.png"));
		break;

	default:
	case Unloaded:
		u = QPixmap(QString::fromUtf8(":/newCommunication/icons/niezaladowane.png"));
		break;
	case PartiallyLoaded:
		u = QPixmap(QString::fromUtf8(":/newCommunication/icons/polnapol-ladowanie2.png"));
		break;
	}

	//// mamy już wszystko - generujemy ikonę
	return QIcon(mergePixmaps(s, u));
}


QPixmap DataSourceWidget::mergePixmaps(const QPixmap & pmL, const QPixmap & pmR)
{
	int halfWidth = (std::max)(pmL.width(), pmR.width());
	int width = (std::max)(pmL.width() + pmR.width(), 2 * halfWidth);
	int pomHalfWidth = halfWidth/4;
	width += pomHalfWidth;

	int height = (std::max)(pmL.height(), pmR.height());

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
	//własne menu kontekstowe?
	//dodać jakieś combo do zmiany widoku karty pacjenta?
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

	//próbuje ustalić pacjenta, jeżeli się uda to ustawiam kartę pacjenta
	const MedicalShallowCopy::Patient * patient = nullptr;
	const MotionShallowCopy::Performer * subject = nullptr;
	getPatientAndSubject(current, patient, subject);

	patientCardManager.currentPatientCard()->setPatient(patient, subject, QPixmap(), dataSource->currentUser_.userData());

	//nagłówek danych
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
			//nie trzeba filtrować - wystarczy przepisać
			tmpShallow = dataSource->fullShallowCopy;
		}

		//ustawiam przefiltorwane dane
		std::swap(filteredShallowCopy, tmpShallow);

		//ustawiamy aktualny filtr
		filterManager.setCurrentFilter(idx);

		//odświeżam status przefiltrowanych danych
		filteredShallowCopyStatus->setShallowCopy(&filteredShallowCopy);

		//unieważniamy perspektywy
		perspectiveManager.invalidatePerspectives();
		//odświeżamy aktualną perspektywę
		perspectiveManager.rebuildCurrentPerspective(filteredShallowCopy);
		//unieważniam content - zmieniła się zawartość
		invalidatePerspectivesContent();
		//wypełniamy content aktualnej perspektywy
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

void DataSourceWidget::onLogin(const QString & user, const QString & password)
{
	setCursor(Qt::WaitCursor);
	QApplication::processEvents();

	//mamy jakieś dane wejściowe - możemy próbować logować

	QString error(tr("Error during login: "));
	bool wasError = false;
	try{
		dataSource->login(user.toStdString(), password.toStdString());
	}catch(std::exception & e){
		error += e.what();
		wasError = true;
	}catch(...){
		error += tr("UNKNOWN ERROR");
		wasError = true;
	}

	// błąd logowania - błąd połączenia z webserwisami

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

        // zapisanie loginu i hasła
        saveCredentials();

		bool synch = false;
		bool shallowCopyAvailable = false;

		dataSource->pathsManager->createUserDataPaths();

		communication::ShallowCopy userShallowCopy;

		loginButton->setText(tr("Logout"));

		// poprawna komunikacja, użytkownik zweryfikowany || brak komunikacji i logowanie lokalne

		if(dataSource->offlineMode() == true){
			// jeżeli jestem zalogowany lokalnie to informuję o tym
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
					//coś nie tak z nasza płytkąkopią bazy danych - trzeba ją walidować i poprawiać - pewnie obcinać aż będzie poprawna i spójna
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
			}else if(dataSource->offlineMode() == false){

				//pobierz datę ostatenij modyfikacji i porównaj
				//jeśli nowsza to zaproponuj synchronizację
				//jeśli odmówi załaduj już sparsowaną płytką kopię bazy danych
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
		}else if(dataSource->offlineMode() == false){

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
			//ustawiamy nową płytką kopię bazy danych
			dataSource->setShallowCopy(userShallowCopy);
			//odświeżam przefiltrowaną płytką kopię danych co wiąże się z unieważnieniem dotychczasowych perspektyw
			onFilterChange(filterManager.currentFilterIndex());
		}

		tryLoadProjects();

		setTabEnabled(0, true);
		setTabEnabled(1, true);
		setTabEnabled(2, true);
		setCurrentWidget(motionDataTab);

	}else{

		//poprawna komunikacja, nie udało się zweryfikować użytkownika

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

void DataSourceWidget::onLogin()
{
	// jeżeli zalogowany to wyloguj

	if(dataSource->isLogged() == true){

		if(currentDownloadRequest != nullptr){
			//mamy jakieś ściąganie w międzyczasie!!
			//pytamy czy na pewno wylogować - wtedy je anulujemy

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

		//wyładować wszystkie dane
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

		std::map<std::string, std::set<int>>().swap(projects);

		loginButton->setText(tr("Login"));
	}else{

		// pierwsza weryfikacja danych wejściowych do logowania - czy podano niezbędne dane
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

			onLogin(userEdit->text(), passwordEdit->text());
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
		//zareagować kiedy nie możemy zapisać danych do loaclStorage - w sumie już ich nie potrzebujemy, tylko przy kolejnym logowaniu trzeba znów płytką kopię ściągnąc
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
				//jeśli nikt nie jest jeszcze zalogowany to przenosimy do strony logowania
				toolBox->setCurrentIndex(2);
				//częsciowo wypełniam już dane usera
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
			//jeśli nikt nie jest jeszcze zalogowany to przenosimy do strony logowania
			toolBox->setCurrentIndex(0);
			//częsciowo wypełniam już dane usera
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

void DataSourceWidget::onPasswordReset()
{
	if(emailResetEdit->text().isEmpty() == true){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Password reset validation"));
		messageBox.setText(tr("E-mail field must be filled correctly for password reset procedure."));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else if(dataSource->tryResetPassword(emailResetEdit->text().toStdString())){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Password reset successful"));
		messageBox.setText(tr("For the given email address further instructions about password reset procedure have been sent. Check your email in several minutes. Check your spam if no email have been received. Contact application administrator if email was not received in more than 2 hours."));
		messageBox.setIcon(QMessageBox::Information);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
		setCurrentIndex(0);
	}else{
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Password reset failed"));
		messageBox.setText(tr("Either given email was not registered or there is some problem with database and connection. Try again in several minutes. If problem continues contact application administrator."));
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
		//ustawiamy aktualną perspektywę
		perspectiveManager.setCurrentPerspective(idx);
		//odświeżamy aktualną perspektywę
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
			//wypełniamy content aktualnej perspektywy
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
		//ustawiamy aktualną perspektywę
		contentManager.setCurrentContent(idx);

		//wypełniamy content aktualnej perspektywy
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

	std::set<int>().swap(filesToLoad);
	std::set<int>().swap(filesToUnload);
	std::set<int>().swap(filesToDownload);

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
#ifdef DEMO_VERSION_COMMUNICATION
	auto download = menu.addAction(tr("Download - not available in demo version"));
#else
  auto download = menu.addAction(tr("Download"));
#endif
	menu.addSeparator();

	auto refresh = menu.addAction(tr("Refresh status"));

	load->setEnabled(false);
	unload->setEnabled(false);
	download->setEnabled(false);
	refresh->setEnabled(false);

	//skoro coś ściągam muszę poczekać!! nie przetwarzam reszty tylko pokazuje nizainicjalizowane menu
	if(currentDownloadRequest == nullptr){

		//aktywujemy podstaweowe operacje
		connect(refresh, SIGNAL(triggered()), this, SLOT(refreshStatus()));
		refresh->setEnabled(true);

		//sprawdzamy co to za item, potrzebujemy wszystkich plików z nim związanych
		std::set<int> filesIDs;

		auto selectedItems = perspective->selectedItems();

		//sprawdzam czy to cos co mogę załadować?

		if(isItemLoadable(selectedItems[0]) == true){

			currentPerspectiveItem = selectedItems[0];
			//pobieram pliki dla wybranego elementu
			getItemsFiles(currentPerspectiveItem, filesIDs, filteredShallowCopy);
			//UTILS_ASSERT(filesIDs.empty() == false, "Brak danych w drzewie");

			if(filesIDs.empty() == false){
				//mam dane - mogę pracować
				//wyciągam dane lokalne i zdalne
				//wyciągam dane załądowane i niezaładowane
				//na bazie tych info odpowiednio łącze akcje ze slotami i aktywuje je + zapamiętuje te info do wykonania ich!!

				//pomijamy wszystkie niekompatybilne z DM pliki

				std::set<int> dmOKFiles;

				//filtruje pliki obsługiwane przez DM
				FilesHelper::filterFiles(filesIDs, dmOKFiles, *(dataSource->fileStatusManager));

				//pliki do załadowania
				FilesHelper::filterFiles(dmOKFiles, DataStatus(Local, Unloaded), filesToLoad, *(dataSource->fileStatusManager));

				//pliki do wyładowania - EXPERIMENTAL
				FilesHelper::filterFiles(dmOKFiles, communication::Loaded, filesToUnload, *(dataSource->fileStatusManager));

				//pliki do ściągnięcia
				FilesHelper::filterFiles(filesIDs, communication::Remote, filesToDownload, *(dataSource->fileStatusManager));

				if(filesToLoad.empty() == false){
					load->setEnabled(true);
					connect(load, SIGNAL(triggered()), this, SLOT(onLoad()));
				}

				if(filesToUnload.empty() == false){
					unload->setEnabled(true);
					connect(unload, SIGNAL(triggered()), this, SLOT(onUnload()));
				}

        #ifndef DEMO_VERSION_COMMUNICATION
				
				if(filesToDownload.empty() == false){
					download->setEnabled(true);
					connect(download, SIGNAL(triggered()), this, SLOT(onDownload()));
				}
				
        #endif
			}
		}
	}
}

void DataSourceWidget::generateGeneralContextMenu(QMenu & menu, QTreeWidget * perspective)
{
	//poszczeólne akcje
	auto loadAll = menu.addAction(tr("Load All"));
	auto unloadAll = menu.addAction(tr("Unload All"));
  #ifdef DEMO_VERSION_COMMUNICATION
	auto downloadAll = menu.addAction(tr("Download All - not available in demo version"));
  #else
  auto downloadAll = menu.addAction(tr("Download All"));
  #endif

	//skoro coś ściągam muszę poczekać!! nie przetwarzam reszty tylko pokazuje nizainicjalizowane menu
	if(currentDownloadRequest == nullptr){

		std::set<int> allFiles;

		filteredFiles(allFiles);

		std::set<int> dmOkFiles;
		FilesHelper::filterFiles(allFiles, dmOkFiles, *(dataSource->fileStatusManager));
		//pliki do załadowania
		FilesHelper::filterFiles(dmOkFiles, DataStatus(Local, Unloaded), filesToLoad, *(dataSource->fileStatusManager));
		//pliki do wyładowania
		FilesHelper::filterFiles(dmOkFiles, communication::Loaded, filesToUnload, *(dataSource->fileStatusManager));
		//pliki do ściągnięcia
		FilesHelper::filterFiles(dmOkFiles, communication::Remote, filesToDownload, *(dataSource->fileStatusManager));

		if(filesToLoad.empty() == false){
			loadAll->setEnabled(true);
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoad()));
		}

		if(filesToUnload.empty() == false){
			unloadAll->setEnabled(true);
			connect(unloadAll, SIGNAL(triggered()), this, SLOT(onUnload()));
		}

    #ifndef DEMO_VERSION_COMMUNICATION
		
		if(filesToDownload.empty() == false){
			downloadAll->setEnabled(true);
			connect(downloadAll, SIGNAL(triggered()), this, SLOT(onDownload()));
		}
		
    #endif

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
  #ifdef DEMO_VERSION_COMMUNICATION
	auto synch = menu.addAction(tr("Synchronize - not available in demo version"));
	synch->setEnabled(false);
  #else
  auto synch = menu.addAction(tr("Synchronize"));
  #endif
	//skoro coś ściągam muszę poczekać!! nie przetwarzam reszty tylko pokazuje nizainicjalizowane menu
	if(currentDownloadRequest == nullptr){

		connect(saveProject, SIGNAL(triggered()), this, SLOT(onSaveProject()));
    #ifndef DEMO_VERSION_COMMUNICATION
		connect(synch, SIGNAL(triggered()), this, SLOT(updateShallowCopy()));
    #endif

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
				//extra dodaję pliki specyficzne sesji
				FilesHelper::getSpecificFiles(motion->value()->session, filesIDs);
			}
			break;
		case FileContent:
			{
				auto file = dynamic_cast<FileItem*>(item);
				auto f = file->value();
				filesIDs.insert(f->fileID);
				//extra dodaję pliki specyficzne sesji
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
					//extra dodaję pliki specyficzne sesji
					FilesHelper::getSpecificFiles((*it)->session, filesIDs);
				}
			}
			break;
		case FilesGroupContent:
			{
				auto filesGroup = dynamic_cast<FilesGroupItem*>(item);
				for(auto it = filesGroup->value().begin(); it != filesGroup->value().end(); ++it){
					filesIDs.insert((*it)->fileID);
					//extra dodaję pliki specyficzne sesji
					FilesHelper::getSpecificFiles((*it)->isSessionFile() == true ? (*it)->session : (*it)->trial->session, filesIDs);
				}
			}
			break;
		case CustomContent:
			found = false;
			break;
		}
	}

	//jezeli nie znalazlem to ide w dół hierarchi perspektywy - może tam coś znajdę
	if(found == false){
		int childrenCount = item->childCount();
		for(int i = 0; i < childrenCount; ++i){
			getItemsFiles(item->child(i), filesIDs, shallowCopy);
		}
	}
}

void DataSourceWidget::updateShallowCopy()
{
	//lokujemy - nie można teraz zmienić użytkownika z zewnątrz
	//w sumie powinniśmy lokować całe źródło
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*DataSourcePathsManager::instance());
	if(dataSource->isShallowCopyComplete() == true && dataSource->isShallowCopyCurrent() == true){
		//message box - nie musimy aktualizować, mamy najświeższą wersję
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
	//generujemy ścieżki

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
	std::vector<std::string>().swap(downloadedFiles);
	//zanim wystartujemy musimy zarejestrować obiekt który będzie obserwował zmiany w requeście pobierania + aktualizował progress pobierania
	request->attach(this);
	registeredRequests[request.get()] = request;
	//startujemy request - dodajemy do kolejki, więc niekoniecznie po tym wywołaniu zaczniemy pobierać
	//to operuje na communication manager - coś może jednak pójść nie tak
	try{
		request->start();
	}catch(std::exception & ){
		//TODO
		//message box z informacją
	}catch(...){
		//TODO
		//message box z informacją
	}
}

bool DataSourceWidget::refreshShallowCopy()
{
	bool ret = true;

	communication::ShallowCopy shallowCopy;
	try{
		if(dataSource->buildShallowCopyFromLocalUserSpace(shallowCopy) == true){			
			//ustawiam nowa płytka kopię danych
			dataSource->setShallowCopy(shallowCopy);
			//odświeżam przefiltrowaną płytką kopię danych co wiąże się z unieważnieniem dotychczasowych perspektyw
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
	// poziom 100MB jako limit ostrzeżenia
	static const long downloadSizeWarningLevel = 104857600;
	//wyliczmy ile miejsca potrzebujemy i ile mamy

	unsigned long long size = 0;

	for(auto it = filesToDownload.begin(); it != filesToDownload.end(); ++it){
		auto f = filteredShallowCopy.motionShallowCopy->files.find(*it);
		size += f->second->fileSize;
	}

	unsigned long long avaiable = dataSource->pathsManager->freeSpace(dataSource->pathsManager->userDataPath());

	//mnoże razy 2 bo muszę to potem jeszcze do local storage wrzucić!!
	if(avaiable < (size << 1)){
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Download problem"));
		messageBox.setText(tr("Download request requires %1 of free space while there is only %2 free space left. Download could not be continued. Please free some space or limit your download request.").arg(formatFileSize(size)).arg( formatFileSize(avaiable)));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStandardButtons(QMessageBox::Ok);
		messageBox.setDefaultButton(QMessageBox::Ok);
		messageBox.exec();
	}else{
		//ok - mogę ściągać ale może tego jest sporo więc trzeba użytkownika uświadomić
		if(size > downloadSizeWarningLevel){
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Download warning"));
			messageBox.setText(tr("Download request contains %1. This will take some time to download such amount of data. Are you sure you can wait some longer time?").arg(formatFileSize(size)));
			messageBox.setIcon(QMessageBox::Warning);
			messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			messageBox.setDefaultButton(QMessageBox::No);

			int ret = messageBox.exec();

			if(ret == QMessageBox::No){
				//użytkownik się nawrócił - anulujemy
				return;
			}
		}

		//skoro tu jestem to zaczynamy ściągać
		try{
			//przygotuj request dla ściągania
			auto downloadRequest = dataSource->generateDownloadFilesRequest(filesToDownload);
			//zacznij przetwarzać request
			processDataDownload(nullptr, downloadRequest);

		}catch(std::exception & e){
			//nie udało się utworzyć/przygotować requesta więc info o błędzie
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

void DataSourceWidget::loadSubjectHierarchy(const std::map<int, std::vector<core::ObjectWrapperConstPtr>> & loadedFilesObjects)
{
	typedef std::map<int, std::set<int>> MotionFiles;
	typedef std::map<int, std::pair<std::set<int>, MotionFiles>> SessionFiles;
	typedef std::map<int, SessionFiles> SubjectFiles;


	class JointsInitializer
	{
	public:
		  static void initialize(core::ObjectWrapper & object, const core::ObjectWrapperConstPtr & dataWrapper, const core::ObjectWrapperConstPtr & modelWrapper)
		  {
			  kinematic::SkeletalDataConstPtr data;
			  kinematic::SkeletalModelConstPtr model;
			  if(dataWrapper->tryGet(data) == true && modelWrapper->tryGet(model) == true && data != nullptr && model != nullptr){
				  kinematic::JointAnglesCollectionPtr joints(new kinematic::JointAnglesCollection());
				  joints->setSkeletal(model, data);
				  object.trySet(joints);
			  }
		  }
	};

	auto subjectService = core::queryServices<PluginSubject::ISubjectService>(plugin::getServiceManager());

	if(subjectService == nullptr){
		return;
	}

	//buduje mapę hierarchii subject -> session -> motion -> files
	//na bazie tej mapy będę realizował hierarchię pluginu subject

	SubjectFiles subjectHierarchy;
    std::set<int> loadedFiles;

	auto itEND = loadedFilesObjects.end();
	for(auto it = loadedFilesObjects.begin(); it != itEND; ++it){
        loadedFiles.insert(it->first);
		if(it->first < 0){
			auto s = filteredShallowCopy.motionShallowCopy->sessions.find(- it->first);

			if(s != filteredShallowCopy.motionShallowCopy->sessions.end()){
				subjectHierarchy[s->second->performerConf->performer->performerID][s->second->sessionID].first.insert(it->first);
			}else{
				//TODO
				//INFO o nieobsługiwanym pliku
			}

		}else{
			auto fileIT = filteredShallowCopy.motionShallowCopy->files.find(it->first);
			if(fileIT == filteredShallowCopy.motionShallowCopy->files.end()){
				//TODO
				//INFO o nieobsługiwanym pliku
				continue;
			}

			if(fileIT->second->isSessionFile() == true){
				subjectHierarchy[fileIT->second->session->performerConf->performer->performerID][fileIT->second->session->sessionID].first.insert(it->first);
			}else{
				subjectHierarchy[fileIT->second->trial->session->performerConf->performer->performerID][fileIT->second->trial->session->sessionID].second[fileIT->second->trial->trialID].insert(it->first);
			}
		}
	}

	auto transaction = dataSource->memoryDM->transaction();
    auto hierarchyTransaction = dataSource->memoryDM->hierarchyTransaction();
	for(auto subjectIT = subjectHierarchy.begin(); subjectIT != subjectHierarchy.end(); ++subjectIT){
		//tworzę subject jeśli to konieczne!!

		PluginSubject::SubjectPtr subPtr;
		core::ObjectWrapperPtr subOW;

		auto subIT = subjectsMapping.find(subjectIT->first);
		if(subIT != subjectsMapping.end()){
			//mam subjecta - nie musze już nic robić
			subOW = subIT->second.first;
			//FIX dla linux RtR
			//subPtr = subOW->get();
			subOW->tryGet(subPtr);
		}else{
			//tworzę subjecta
			subOW = subjectService->createSubject();
			//tworze ow dla subjecta
			//FIX dla linux - RtR nie działa dla boost::shared_ptr
			//subPtr = subOW->get();
			subOW->tryGet(subPtr);
			
			std::stringstream label;

			auto pIT = filteredShallowCopy.medicalShallowCopy->patients.find(subjectIT->first);

			if(pIT == filteredShallowCopy.medicalShallowCopy->patients.end()){
				label << "Subject " << subjectIT->first;
			}else{
				label << pIT->second->name << ", " << pIT->second->surname;
				addPatientObject(pIT->second, subPtr->getID());
			}

			(*subOW)["label"] = label.str();

            
			//dodaję do DM
			transaction->addData(subOW);

			//zapamiętuję mapowanie
			subjectsMapping[subjectIT->first].first = subOW;
		}

		//mam subjecta, mogę iść dalej do sesji
		for(auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){

			PluginSubject::SessionPtr sPtr;
			core::ObjectWrapperPtr sOW;

			auto sIT = sessionsMapping.find(sessionIT->first);
			if(sIT != sessionsMapping.end()){
				//mam subjecta - nie musze już nic robić
				sOW = sIT->second.first;
				//FIX dla linux RtR
				//sPtr = sOW->get();
				sOW->tryGet(sPtr);
			}else{
				//tworzę sesję
				//generuję zbiór ow dla sesji
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
				//tworze OW dla danych antropometrycznych
				auto antroOW = core::ObjectWrapper::create<communication::AntropometricData>();
				antroOW->set(antro);

				sessionObjects.push_back(antroOW);
				sessionsMapping[sessionIT->first].second.push_back(antroOW);


				sOW = subjectService->createSession(subOW);
				//FIX dla linux RtR
				//sPtr = sOW->get();
				sOW->tryGet(sPtr);
				for(auto it = sessionObjects.begin(); it != sessionObjects.end(); ++it){
					sPtr->addData(*it);
				}

                
				(*sOW)["label"] = s->sessionName;
				(*sOW)["EMGConf"] = boost::lexical_cast<std::string>(s->emgConf);
				(*sOW)["data"] = webservices::toString(s->sessionDate);
				if(s->groupAssigment != nullptr){
					(*sOW)["groupID"] = boost::lexical_cast<std::string>(s->groupAssigment->sessionGroupID);
					auto sgIT = filteredShallowCopy.motionMetaData.sessionGroups.find(s->groupAssigment->sessionGroupID);
					if(sgIT != filteredShallowCopy.motionMetaData.sessionGroups.end()){
						(*sOW)["groupName"] = sgIT->second.sessionGroupName;
					}
				}

				transaction->addData(antroOW);
				transaction->addData(sOW);

				//zapamiętuję mapowanie
				sessionsMapping[sessionIT->first].first = sOW;
				subPtr->addSession(sOW);
			}

			//mam sesję - mogę iść dalej z motionami!!
			for(auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){

				PluginSubject::MotionPtr mPtr;

				auto mIT = motionsMapping.find(motionIT->first);
				if(mIT != motionsMapping.end()){
					//mam subjecta - nie musze już nic robić
					//TODO
					//nie powinno mnie tu być wg aktualnego działania pluginu subject!!
					//FIX dla linux RtR
					//mPtr = mIT->second.first->get();
					mIT->second.first->tryGet(mPtr);
				}else{
					//tworzę sesję
					//generuję zbiór ow dla motiona
					std::vector<core::ObjectWrapperConstPtr> motionObjects;
					for(auto fIT = motionIT->second.begin(); fIT != motionIT->second.end(); ++fIT){
						//pobieram obiekty
						const auto & objects = loadedFilesObjects.find(*fIT)->second;
						//kopiuje
						for(auto oIT = objects.begin(); oIT != objects.end(); ++oIT){
							motionObjects.push_back(*oIT);
						}
					}

					auto m = filteredShallowCopy.motionShallowCopy->trials.find(motionIT->first)->second;


					//sprawdzamy joint angles - jeśli nie ma budujemy i dodajemy do DM
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

					core::ObjectWrapperCollection modelWrappers(typeid(kinematic::SkeletalModel), false);
					sPtr->getObjects(modelWrappers);

					core::ObjectWrapperPtr jointsWrapper;

					if(modelWrappers.empty() == false){

						modelWrapper = modelWrappers.front();
						
						if (dataWrapper && modelWrapper) {
							jointsWrapper = core::ObjectWrapper::create<kinematic::JointAnglesCollection>();
							jointsWrapper->set(core::ObjectWrapper::LazyInitializer(boost::bind(&JointsInitializer::initialize, _1, dataWrapper, modelWrapper)));
							motionObjects.push_back(jointsWrapper);
							motionsMapping[motionIT->first].second.push_back(jointsWrapper);
							transaction->addData(jointsWrapper);
						}
					}

					auto mOW = subjectService->createMotion(sOW);
					//FIX dla linux RtR
					//mPtr = mOW->get();
					mOW->tryGet(mPtr);
					for(auto it = motionObjects.begin(); it != motionObjects.end(); ++it){
						mPtr->addData(*it);
					}

                    if (mPtr->hasObject(typeid(VideoChannel), false) && mPtr->hasObject(typeid(MovieDelays), false)) {
                        core::ObjectWrapperCollection videoCollection(typeid(VideoChannel), false);
                        core::ObjectWrapperCollection movieDelaysCollection(typeid(MovieDelays), false);
                        mPtr->getObjects(videoCollection);
                        mPtr->getObjects(movieDelaysCollection);
                        if (movieDelaysCollection.size() == 1 ) {
                            MovieDelaysConstPtr delays = *(movieDelaysCollection.begin())->get();
                            if (delays->size() == videoCollection.size()) {
                                int i = 0;
                                for (auto it = videoCollection.begin(); it != videoCollection.end(); ++it) {
                                    core::ObjectWrapperPtr wrp = utils::const_pointer_cast<core::ObjectWrapper>(*it);
                                    (*wrp)["movieDelay"] = boost::lexical_cast<std::string>(delays->at(i++));
                                }
                            } else {
                                PLUGIN_LOG_ERROR("Unable to map movie delays");
                            }
                        } else {
                            PLUGIN_LOG_ERROR("Wrong movie delays");
                        }
                    }
                    

					if(jointsWrapper != nullptr){
						//metadane
						(*jointsWrapper)["name"] = mPtr->getLocalName() + " joints";
						(*jointsWrapper)["source"] = "newCommunication->motion->" + mPtr->getLocalName();
					}
					
					(*mOW)["label"] = m->trialName;

					transaction->addData(mOW);

					//zapamiętuję mapowanie
					motionsMapping[motionIT->first].first = mOW;
					sPtr->addMotion(mOW);
				}
			}
		}

        std::vector<IHierarchyPerspectivePtr> perspectives = dataSource->getHierarchyPerspectives();
        std::set<core::IHierarchyItemConstPtr> roots;
        for (auto it = perspectives.begin(); it != perspectives.end(); ++it) {
            //auto root = TreeBuilder::createTree("SUB", subPtr);
            auto root = (*it)->getPerspective(subPtr);
            if (root) {
                int childCount = root->getNumChildren();
                for (int c = 0; c < childCount; ++c) {
                    auto r = root->getChild(c);
                    roots.insert(r);
                    hierarchyTransaction->addRoot(r);
                }
            }
        }
        files2roots[loadedFiles] = roots;
	}
}

void DataSourceWidget::addPatientObject(const webservices::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectID subjectID)
{
	//generuję listę schorzeń
	std::vector<communication::Disorder> disorders;
	for(auto it = patient->disorders.begin(); it != patient->disorders.end(); ++it)	{

		communication::Disorder dis;
		dis.focus = it->second.focus;
		dis.diagnosisDate = webservices::toString(it->second.diagnosisDate);
		dis.comments = it->second.comments;
		dis.name = it->second.disorder->name;

		disorders.push_back(dis);
	}

	PatientPtr pPtr(new Patient(subjectID, patient->name, patient->surname, patient->birthDate.toString(),
		Patient::decodeGender(patient->gender), utils::shared_ptr<const QPixmap>(), disorders));

	//dodaję do DM
	auto pOW = core::ObjectWrapper::create<communication::IPatient>();
	pOW->set(pPtr);

	dataSource->memoryDM->addData(pOW);

	//zapamiętuje
	patientsMapping[patient->patientID].first = pOW;
}

utils::shared_ptr<communication::AntropometricData> DataSourceWidget::createAntropometricData(const webservices::MotionShallowCopy::Attrs & attrs)
{
	auto antro = utils::shared_ptr<communication::AntropometricData>(new communication::AntropometricData());
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

	//zainicjować wskaźnik do serwisu!!

	auto subjectService = core::queryServices<PluginSubject::ISubjectService>(plugin::getServiceManager());

	if(subjectService == nullptr){
		return;
	}

	auto transaction = dataSource->memoryDM->transaction();

	//buduje mapę hierarchii subject -> session -> motion -> files
	//na bazie tej mapy będę realizował hierarchię pluginu subject

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
					//mam motion - sprawdzam czy mogę go usunąć
					std::set<int> motionFiles;
					FilesHelper::getMotionFiles(motionIT->first, filteredShallowCopy, motionFiles);

					//teraz robię różnicę tego co jest w płytkiej kopii a tego co mam wyładować
					std::vector<int> diff((std::max)(motionFiles.size(), motionIT->second.size()));
					auto diffIT = std::set_difference(motionFiles.begin(), motionFiles.end(), motionIT->second.begin(), motionIT->second.end(), diff.begin());

					if(diffIT == diff.begin()){
						//to znaczy że usunąłem wszystkie pliki motiona -> mogę usuwać motiona
						for(auto rIT = mIT->second.second.begin(); rIT != mIT->second.second.end(); ++rIT){
							transaction->removeData(*rIT);
						}

						transaction->removeData(mIT->second.first);

						motionsMapping.erase(mIT);
					}else{
						//TODO
						//ta sytuacja obecnie nie powinna mieć miejsca ze względu na specyfikę pluginu subject!!
						throw std::runtime_error("Plugin subject to redesign");
					}
				}else{
					//TODO
					//tutaj mnie nie powinno być - chyba że plugin subject wcześniej nie działał
					//w takim razie nie reaguje
				}
			}

			//usunąłem motiony sesji - czas zobaczyć czy sesja jeszcze czymś dysponuje
			PluginSubject::SessionPtr sPtr;

			auto sIT = sessionsMapping.find(sessionIT->first);
			if(sIT != sessionsMapping.end()){
				//mam subjecta - nie musze już nic robić
				//FIX dla linux RtR
				//sPtr = sIT->second.first->get();
				sIT->second.first->tryGet(sPtr);
				core::ConstObjectsList motions;
				sPtr->getMotions(motions);
				if(motions.empty() == true){
					//sesja jest pusta - do usunięcia
					for(auto rIT = sIT->second.second.begin(); rIT != sIT->second.second.end(); ++rIT){
						transaction->removeData(*rIT);
					}

					transaction->removeData(sIT->second.first);
					sessionsMapping.erase(sIT);
				}
			}else{
				//TODO
				//tutaj mnie nie powinno być - chyba że plugin subject wcześniej nie działał
				//w takim razie nie reaguje
			}
		}

		//usuwam subject jeśli to konieczne!!

		PluginSubject::SubjectPtr subPtr;

		auto subIT = subjectsMapping.find(subjectIT->first);
		if(subIT != subjectsMapping.end()){
			//mam subjecta - nie musze już nic robić
			//FIX dla linux RtR
			//subPtr = subIT->second.first->get();
			subIT->second.first->tryGet(subPtr);
			core::ConstObjectsList sessions;
			subPtr->getSessions(sessions);
			if(sessions.empty() == true){

				for(auto rIT = subIT->second.second.begin(); rIT != subIT->second.second.end(); ++rIT){
					transaction->removeData(*rIT);
				}

				transaction->removeData(subIT->second.first);

				//musze jeszcze usunąć pacjenta jeśli mam!!
				auto patientIT = patientsMapping.find(subIT->first);
				if(patientIT != patientsMapping.end()){

					for(auto rIT = patientIT->second.second.begin(); rIT != patientIT->second.second.end(); ++rIT){
						transaction->removeData(*rIT);
					}

					transaction->removeData(patientIT->second.first);

					patientsMapping.erase(patientIT);
				}

				subjectsMapping.erase(subIT);
			}
		}else{
			//TODO
			//tutaj mnie nie powinno być - chyba że plugin subject wcześniej nie działał
			//w takim razie nie reaguje
		}
	}
}

void DataSourceWidget::unloadSubjectHierarchy()
{
	auto transaction = dataSource->memoryDM->transaction();

	for(auto it = motionsMapping.begin(); it != motionsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			transaction->removeData(*rIT);
		}

		transaction->removeData(it->second.first);
	}

	for(auto it = sessionsMapping.begin(); it != sessionsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			transaction->removeData(*rIT);
		}

		transaction->removeData(it->second.first);
	}

	for(auto it = subjectsMapping.begin(); it != subjectsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			transaction->removeData(*rIT);
		}

		transaction->removeData(it->second.first);
	}

	for(auto it = patientsMapping.begin(); it != patientsMapping.end(); ++it){

		for(auto rIT = it->second.second.begin(); rIT != it->second.second.end(); ++rIT){
			transaction->removeData(*rIT);
		}

		transaction->removeData(it->second.first);
	}

	std::map<int, MappingValue>().swap(patientsMapping);
	std::map<int, MappingValue>().swap(subjectsMapping);
	std::map<int, MappingValue>().swap(sessionsMapping);
	std::map<int, MappingValue>().swap(motionsMapping);
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

			//rozpocznij odświeżanie widgeta w osobnym wątku
			downloadRefreshTimer.start(40);
		}
		break;

	case IDownloadRequest::FinishedCancel:
		//info co się stało
		downloadCanceled = true;
		finishDownloadRequest();
		break;

	case IDownloadRequest::FinishedError:
		{
			downloadCrashed = true;
			//info co się stało
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
	//zakończ odświeżanie widgeta w osobnym wątku
	downloadRefreshTimer.stop();
	localDataLoader.reset(new LocalDataLoader(this));
	//synchronizujemy to co się udało w całości pobrać (zapisujemy do loaclStorage + informacja co się stało
	localDataLoader->start();
}

void DataSourceWidget::tryHideStatusWidget()
{
	//if(getObservedObjects().empty() == true){
		downloadStatusWidget->setVisible(false);
	//}
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

	//zapamiętuje sobie pliki które się dobrze ściągneły
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

	//! Ładuje pliki do DM	
	std::set<int> loadedFiles;
	std::map<int, std::vector<core::ObjectWrapperConstPtr>> loadedFilesObjects;
	std::map<int, std::string> loadingErrors;
	std::vector<int> unknownErrors;

	{
		auto transaction = dataSource->fileDM->transaction();

		for(auto it = filesToLoad.begin(); it != filesToLoad.end(); ++it){
			try{
				const auto & p = dataSource->fileStatusManager->filePath(*it);

				if(core::Filesystem::pathExists(p) == false){

					if(core::Filesystem::pathExists(p.parent_path()) == false){
						core::Filesystem::createDirectory(p.parent_path());
					}

					dataSource->localStorage->extractFile(p.filename().string(), p);
				}

				transaction->addFile(p);
				core::ConstObjectsList oList;
				transaction->getObjects(p, oList);
				loadedFiles.insert(*it);
				loadedFilesObjects[*it] = std::vector<core::ObjectWrapperConstPtr>(oList.begin(), oList.end());
			}catch(std::exception & e){
				loadingErrors[*it] = std::string(e.what());
			}catch(...){
				unknownErrors.push_back(*it);
			}
		}

	}

	filesLoadedToDM.insert(loadedFiles.begin(), loadedFiles.end());

	refreshStatus(loadedFiles);

	//w loadedFilesObjects mamy info o plikach i związanych z nimi obiektach domenowych
	//próbujemy teraz przez plugin subject realizować hierarchię danych

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

	//próbujemy teraz przez plugin subject realizować hierarchię danych

	unloadSubjectHierarchy(files);

	//! Ładuje pliki do DM
	std::set<int> unloadedFiles;
	std::map<int, std::string> unloadingErrors;
	std::vector<int> unknownErrors;

	{

		auto transaction = dataSource->fileDM->transaction();

		for(auto it = files.begin(); it != files.end(); ++it){
			try{
				std::vector<core::ObjectWrapperPtr> objects;
				const auto & p = dataSource->fileStatusManager->filePath(*it);
				transaction->removeFile(p);
				unloadedFiles.insert(*it);
			}catch(std::exception & e){
				unloadingErrors[*it] = std::string(e.what());
			}catch(...){
				unknownErrors.push_back(*it);
			}
		}
	}

    {
        auto hierarchyTransaction = dataSource->memoryDM->hierarchyTransaction();
        auto entry = files2roots.find(files);
        if (entry != files2roots.end()) {
            auto roots = entry->second;
            for (auto it = roots.begin(); it != roots.end(); ++it) {
                hierarchyTransaction->removeRoot(*it);
            }
        }
	}

	std::vector<int> diff(filesLoadedToDM.size() - unloadedFiles.size());
	std::set_difference(filesLoadedToDM.begin(), filesLoadedToDM.end(), unloadedFiles.begin(), unloadedFiles.end(), diff.begin());

    std::set<int> s(diff.begin(), diff.end());
	filesLoadedToDM.swap(s);

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
	//filtruje pliki obsługiwane przez DM
	FilesHelper::filterFiles(toVerify, dmOKFiles, *(dataSource->fileStatusManager));

	std::set<int>().swap(filesToDownload);

	//pliki do ściągnięcia
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
		//obsługa ściągania - jak się całe skończy to łąduje projekt z powodzeniem, jak tylko częsciowo to warning!!

	}

	std::set<int>().swap(filesToDownload);
	std::set<int>().swap(filesToLoad);
	//pliki do załadowania
	FilesHelper::filterFiles(dmOKFiles, Local, filesToLoad, *(dataSource->fileStatusManager));

	unloadFiles(filesLoadedToDM, false);

	loadFiles(filesToLoad);
}

void DataSourceWidget::onSaveProject()
{
	//TODO
	//widget z opcją podania nazwy
	//nazwa staje się nazwa projektu jezeli jeszcze takiego nie ma, jak jest to pytam czy nadpisać
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
		//błąd zapisu
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
				//mamy nazwę
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
    // TODO : czy userDataTab byl potrzebny?
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

void DataSourceWidget::saveCredentials()
{
    QSettings settings(crypt("Credentials", false));
    QString usr = userEdit->text();
    QString pwd = passwordEdit->text();
    settings.setValue(crypt("User", false), crypt(usr, true));
    settings.setValue(crypt("Password", false), crypt(pwd, true));
}

void DataSourceWidget::loadCredentials()
{
    QSettings settings(crypt("Credentials", false));
    QString usr = settings.value(crypt("User", false)).toString();
    QString pwd = settings.value(crypt("Password", false)).toString();
    userEdit->setText(crypt(usr, false));
    passwordEdit->setText(crypt(pwd, false));
}

QString DataSourceWidget::crypt( const QString& input, bool encrypt )
{
    QByteArray tempArray = input.toLocal8Bit();
    const char* indata = tempArray.data();
    auto length = strlen(indata);
    boost::scoped_array<unsigned char> outdata(new unsigned char [length]);
    
    
    unsigned char ckey[] = "XlzuthN1WoyiDzsj";
    unsigned char ivec[] = "4SX9GRId6tAtHzmx";

    AES_KEY key;
    AES_set_encrypt_key(ckey, 128, &key);

    int num = 0;
    AES_cfb128_encrypt((const unsigned char*)indata, outdata.get(), length, &key, ivec, &num, encrypt? AES_ENCRYPT:AES_DECRYPT);

    QString output = QString::fromLocal8Bit((const char*)outdata.get(), length);
    return output;
}

void DataSourceWidget::setCompactMode( bool compact )
{
    //this->horizontalSpacer_2
    this->patientCardPlaceholerWidget->setVisible(!compact);
    this->perspectiveComboBox->setVisible(!compact);
    this->perspectiveLabel->setVisible(!compact);
    this->filterLabel->setVisible(!compact);
    this->filterComboBox->setVisible(!compact);
    int margin = compact ? 0 : 9;
    this->dataViewWidget->setContentsMargins(0,0,0,0);
    this->motionDataTab->setContentsMargins(0,0,0,0);
}
