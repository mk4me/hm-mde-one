#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include "ui_DataViewWidget.h"
#include <plugins/hmdbCommunication/IDataSourceContent.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <boost/bind.hpp>
#include <corelib/IThreadPool.h>
#include "DataSourcePerspective.h"
#include "DataSourceDefaultContent.h"
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include "ShallowCopyUtils.h"
#include <QtGui/QPainter>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <corelib/IJobManager.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <QtCore/QMetaType>
#include "HMDBSourceContextOperations.h"


const bool mapContentTypeToDataType(const hmdbCommunication::ContentType ct,
	hmdbCommunication::DataType & dt)
{
	bool ret = true;

	switch (ct)
	{
	case hmdbCommunication::PatientContent:
		dt = hmdbCommunication::PatientType;
		break;

	case hmdbCommunication::SubjectContent:
		dt = hmdbCommunication::SubjectType;
		break;

	case hmdbCommunication::SessionContent:
		dt = hmdbCommunication::SessionType;
		break;

	case hmdbCommunication::MotionContent:
		dt = hmdbCommunication::MotionType;
		break;

	case hmdbCommunication::FileContent:
		dt = hmdbCommunication::FileType;
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

class DataViewMetaTypesInitializerHelper
{
public:
	DataViewMetaTypesInitializerHelper()
	{
		qRegisterMetaType<hmdbCommunication::IDataSourcePerspective*>("hmdbCommunication::IDataSourcePerspective*");
		qRegisterMetaType<hmdbCommunication::IDataSourceContent*>("hmdbCommunication::IDataSourceContent*");
		qRegisterMetaType<hmdbCommunication::ShallowCopyFilter*>("hmdbCommunication::ShallowCopyFilter*");
		qRegisterMetaType<hmdbServices::ID>("hmdbServices::ID");
		qRegisterMetaType<hmdbCommunication::DataType>("hmdbCommunication::DataType");
	}
};

static DataViewMetaTypesInitializerHelper metaInitializer = DataViewMetaTypesInitializerHelper();

const QPixmap mergePixmapsHorizontal(const std::list<QPixmap> & pixmaps)
{
	int maxPixmapHeight = -1;
	int maxPixmapWidth = -1;

	std::for_each(pixmaps.begin(), pixmaps.end(),
		[&](const QPixmap & p)
	{
		maxPixmapWidth = std::max(maxPixmapWidth, p.width());
		maxPixmapHeight = std::max(maxPixmapHeight, p.height());
	});
	
	const int totalWidth = pixmaps.size() * maxPixmapWidth;

	QPixmap ret(totalWidth, maxPixmapHeight);

	QPainter painter(&ret);
	painter.fillRect(0, 0, totalWidth, maxPixmapHeight, Qt::white);

	painter.setOpacity(1);

	QPoint pos(0, 0);
	int counter = 0;

	std::for_each(pixmaps.begin(), pixmaps.end(),
		[&](const QPixmap & p)
	{
		pos.rx() = counter * maxPixmapWidth + (maxPixmapWidth - p.width()) / 2;
		pos.ry() = (maxPixmapHeight - p.height()) / 2;

		painter.drawPixmap(pos, p);
		++counter;
	});

	return ret;
}

using namespace hmdbCommunication;

const QIcon DataViewWidget::statusIcon(const DataStatus dataStatus)
{
	auto it = icons.find(dataStatus);
	if (it != icons.end()){
		return it->second;
	}

	std::list<QPixmap> pixmaps;

	//zaczynamy ze sposobem sk�adowania
	QPixmap s;

	switch (dataStatus.storage()){
	case Local:
		s = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/lokalne.png"));
		break;

	default:
	case Remote:
		s = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/zdalne.png"));
		break;
	case PartiallyLocal:
		s = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/pol-na-pol-green.png"));
		break;
	}

	pixmaps.push_back(s);

	// teraz stan u�ycia
	QPixmap u;

	switch (dataStatus.usage()){
	case Loaded:
		u = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/zaladowane.png"));
		break;

	default:
	case Unloaded:
		u = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/niezaladowane.png"));
		break;
	case PartiallyLoaded:
		u = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/polnapol-ladowanie2.png"));
		break;
	}

	pixmaps.push_back(u);

	if (shallowCopyContext_->shallowCopyRemoteContext() != nullptr){

		// teraz stan u�ycia
		QPixmap v;

		switch (dataStatus.validity()){
		case Valid:
			v = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/zaladowane.png"));
			break;

		default:
		case Outdated:
			v = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/niezaladowane.png"));
			break;
		case PartiallyValid:
			v = QPixmap(QString::fromUtf8(":/hmdbCommunication/icons/polnapol-ladowanie2.png"));
			break;
		}

		pixmaps.push_back(v);
	}

	auto ret = QIcon(mergePixmapsHorizontal(pixmaps));

	icons[dataStatus] = ret;

	//// mamy ju� wszystko - generujemy ikon�
	return ret;
}

DataViewWidget::DataViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
	hmdbCommunication::IDataSourcePerspective * perspective,
	hmdbCommunication::IDataSourceContent * content,
	hmdbCommunication::ShallowCopyFilter * filter,
	QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::DataViewWidget),
	perspective_(perspective), content_(content), filter_(filter),
	shallowCopyContext_(shallowCopyContext), delPerspective(false)
{
	ui->setupUi(this);

	initializeActions();

	if (perspective_ == nullptr){
		perspective_ = new hmdbCommunication::DataSourcePatientPerspective;
		delPerspective = true;
	}

	if (content_ == nullptr){
		content_ = new hmdbCommunication::DataSourceDefaultContent;
		delContent = true;
	}

	setDefaultPerspectiveHeaders();	

	if (shallowCopyContext_->shallowCopyRemoteContext() == nullptr){
		ui->treeWidget->setIconSize(QSize(40, 20));
	}
	else{
		ui->treeWidget->setIconSize(QSize(60, 20));
	}

	QTimer::singleShot(0, this, SLOT(initializeShallowCopy()));
}

DataViewWidget::~DataViewWidget()
{
	if (delPerspective == true && perspective_ != nullptr){
		delete perspective_;
	}

	if (delContent == true && content_ != nullptr){
		delete content_;
	}
}

void DataViewWidget::initializeActions()
{
	ui->treeWidget->addAction(ui->actionLoad);
	ui->treeWidget->addAction(ui->actionLoadAll);
	ui->treeWidget->addAction(ui->actionUnload);
	ui->treeWidget->addAction(ui->actionUnloadAll);
	ui->treeWidget->addAction(ui->actionRebuild);
	ui->treeWidget->addAction(ui->actionRefreshStatus);
	ui->treeWidget->addAction(ui->actionSynchronize);
	ui->treeWidget->addAction(ui->actionForceSynchronize);
	ui->treeWidget->addAction(ui->actionDownload);
	ui->treeWidget->addAction(ui->actionDownloadAll);
	ui->treeWidget->addAction(ui->actionForceDownload);
}

const bool DataViewWidget::tryRebuildDataStatus()
{
	auto sc = shallowCopyContext_->shallowCopyDataContext()->shallowCopy();

	if (sc == nullptr){
		sc = shallowCopy_;
	}

	if (sc != nullptr){
		coreUI::CoreCursorChanger cc;
		shallowCopyContext_->shallowCopyDataContext()->dataStatusManager()->rebuild(shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->storage(), sc);
	}

	return sc != nullptr;
}

void DataViewWidget::initializeShallowCopy()
{
	if (shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->shallowCopyExists() == false){
		if (shallowCopyContext_->shallowCopyRemoteContext() == nullptr){
			noShallowCopyInStorageAndLocalOnly();
		}
		else{			
			//fire once synch
			noShallowCopyInStorage();
		}
	}
	else{
		if (shallowCopyContext_->shallowCopyRemoteContext() == nullptr){
			setRemoteOperationsEnabled(false);
			extractLocalShallowCopy();
		}
		else{			
			//fire once synch
			extractLocalShallowCopyAndTryUpdate();
		}
	}
}

void DataViewWidget::extractLocalShallowCopy()
{
	coreUI::CoreCursorChanger cc;
	auto sc = shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->createShallowCopy();
	shallowCopyContext_->shallowCopyDataContext()->setShallowCopy(sc);
	setShallowCopy(sc);
}

void DataViewWidget::extractLocalShallowCopyAndTryUpdate()
{
	coreUI::CoreCursorChanger cc;

	bool setShallow = true;
	auto sc = shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->createShallowCopy();	
	
	const auto mq = shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->session()->motionQueries();
	if (sc != nullptr && mq != nullptr){
		const auto dbTime = mq->dataModificationTime();
		if (hmdbCommunication::ShallowCopyUtils::shallowCopyRequiresRefresh(*sc, dbTime) == true){
			const auto ret = QMessageBox::question(this, tr("Synchronization required"), tr("Since last synchronization (%1) some changes were made in database (last: %2). It is suggested to synchronize data. Would You like to synchronize now?").arg(QString::fromStdString(shallowCopy_->motionShallowCopy.timestamp.toString())).arg(QString::fromStdString(dbTime.toString())));
			if (ret == QMessageBox::StandardButton::Yes){
				setShallow = false;
				onSynchronize();
			}
		}
	}	

	if (setShallow == true){
		shallowCopyContext_->shallowCopyDataContext()->setShallowCopy(sc);
		setShallowCopy(sc);
	}
}

void DataViewWidget::noShallowCopyInStorageAndLocalOnly()
{
	setLocalOperationsEnabled(false);
	setRemoteOperationsEnabled(false);
	QMessageBox::warning(this, tr("No data"), tr("You are working in offline mode and there is no data for You in the storage. All operations are disabled. Try to switch to online mode or change storage."));
}

void DataViewWidget::noShallowCopyInStorage()
{
	const auto ret = QMessageBox::question(this, tr("Synchronization required"), tr("You must synchronize with database to gain access to data. Do You want to synchronize?"));
	if (ret == QMessageBox::StandardButton::Yes){
		onSynchronize();
	}
	else{
		setLocalOperationsEnabled(false);
		setRemoteOperationsEnabled(false);
		ui->actionSynchronize->setEnabled(true);
	}
}

void DataViewWidget::rebuildPerspective()
{
	ui->treeWidget->clear();
	if (shallowCopy_ != nullptr){
		coreUI::CoreCursorChanger cc;
		perspective_->rebuildPerspective(ui->treeWidget->invisibleRootItem(), *shallowCopy_);
		refreshCurrentContent(true);
	}
	else{
		setDefaultPerspectiveHeaders();
	}
}

void DataViewWidget::refreshContent()
{
	if (shallowCopy_ != nullptr){
		coreUI::CoreCursorChanger cc;		
		refreshCurrentContent(true);
	}
}

void DataViewWidget::refreshCurrentContent(bool perspectiveFirst)
{
	int topLevelItemsCount = ui->treeWidget->topLevelItemCount();
	for (int i = 0; i < topLevelItemsCount; ++i){
		refrshItemContent(ui->treeWidget->topLevelItem(i), perspectiveFirst);
	}
}

void DataViewWidget::refreshDataStatus(const bool perspectiveFirst)
{
	int topLevelItemsCount = ui->treeWidget->topLevelItemCount();
	for (int i = 0; i < topLevelItemsCount; ++i){
		refreshDataStatus(ui->treeWidget->topLevelItem(i), perspectiveFirst);
	}
}

const hmdbCommunication::DataStatus DataViewWidget::refreshDataStatus(QTreeWidgetItem * item,
	const bool perspectiveFirst)
{	
	int statusColumn = -1;
	QStringList headers;

	//zawarto�� wg regu� pierwsze�stwa i stanu wype�nienia
	if (perspectiveFirst == true){
		if (perspective_->fillContent(item, *shallowCopy_) == false){
			content_->fillContent(item, *shallowCopy_);
			statusColumn = content_->statusField(item);
			content_->headers(item, headers);
		}
		else{
			statusColumn = perspective_->statusField(item);
			perspective_->headers(item, headers);
		}
	}
	else if (content_->fillContent(item, *shallowCopy_) == false){
		perspective_->fillContent(item, *shallowCopy_);
		statusColumn = perspective_->statusField(item);
		perspective_->headers(item, headers);
	}
	else{
		statusColumn = content_->statusField(item);
		content_->headers(item, headers);
	}

	hmdbCommunication::DataStatus status;

	const auto childrenCount = item->childCount();

	if (childrenCount > 0){

		//od�wie�am zawarto�c dzieci je�li nie znamy typu obiektu

		for (int i = 0; i < childrenCount; ++i){
			status |= refreshDataStatus(item->child(i), perspectiveFirst);
		}
	}
	else{
		auto contentItem = dynamic_cast<const hmdbCommunication::TreeWidgetContentItem*>(item);
		if (contentItem != nullptr){
			const auto id = contentItem->id();
			//wystarczy switch + dynamic_cast
			DataType dt = FileType;
			if (mapContentTypeToDataType(contentItem->contentType(), dt) == true){
				status = shallowCopyContext_->shallowCopyDataContext()->dataStatusManager()->dataStatus(dt, id);
			}
		}
	}

	//od�wie�am status
	if (statusColumn == -1 || statusColumn >= headers.size()){
		statusColumn = 0;
	}

	item->setIcon(statusColumn, statusIcon(status));

	//TODO
	//stan �ci�gania dla element�w

	return status;
}

const hmdbCommunication::DataStatus DataViewWidget::refrshItemContent(QTreeWidgetItem * item,
	const bool perspectiveFirst)
{	
	int statusColumn = -1;
	QStringList headers;

	//zawarto�� wg regu� pierwsze�stwa i stanu wype�nienia
	if (perspectiveFirst == true){
		if (perspective_->fillContent(item, *shallowCopy_) == false){
			content_->fillContent(item, *shallowCopy_);
			statusColumn = content_->statusField(item);
			content_->headers(item, headers);
		}
		else{
			statusColumn = perspective_->statusField(item);
			perspective_->headers(item, headers);
		}
	}
	else if (content_->fillContent(item, *shallowCopy_) == false){
		perspective_->fillContent(item, *shallowCopy_);
		statusColumn = perspective_->statusField(item);
		perspective_->headers(item, headers);
	}
	else{
		statusColumn = content_->statusField(item);
		content_->headers(item, headers);
	}

	hmdbCommunication::DataStatus status;

	//od�wie�am zawarto�c dzieci je�li nie znamy typu obiektu
	const auto childrenCount = item->childCount();

	if (childrenCount > 0){

		for (int i = 0; i < childrenCount; ++i){
			status |= refrshItemContent(item->child(i), perspectiveFirst);
		}
	}
	else{
		auto contentItem = dynamic_cast<const hmdbCommunication::TreeWidgetContentItem*>(item);
		if (contentItem != nullptr){
			const auto id = contentItem->id();
			//wystarczy switch + dynamic_cast
			switch (contentItem->contentType()){
			case hmdbCommunication::FileContent:
			{
				status = shallowCopyContext_->shallowCopyDataContext()->dataStatusManager()->dataStatus(hmdbCommunication::FileType, id);
			}
				break;
			}
		}
	}	

	//od�wie�am status
	if (statusColumn == -1 || statusColumn >= headers.size()){
		statusColumn = 0;
	}

	item->setIcon(statusColumn, statusIcon(status));

	//TODO
	//stan �ci�gania dla element�w

	return status;
}

void DataViewWidget::setDefaultPerspectiveHeaders()
{
	QStringList headers;
	perspective_->defaultHeaders(headers);
	ui->treeWidget->setHeaderLabels(headers);
}

void DataViewWidget::setPerspective(hmdbCommunication::IDataSourcePerspective * perspective)
{
	if (delPerspective == true && perspective_ != nullptr){
		delete perspective_;
		delPerspective = false;
	}

	perspective_ = perspective;
	rebuildPerspective();
}

void DataViewWidget::setContent(hmdbCommunication::IDataSourceContent * content)
{
	if (delContent == true && content_ != nullptr){
		delete content_;
		delContent = false;
	}

	content_ = content;
	refreshContent();
}

void filterShallowCopy(const hmdbCommunication::ShallowCopy & inShallow,
	hmdbCommunication::ShallowCopy & outShallow,
	const hmdbCommunication::ShallowCopyFilter * filter)
{
	using namespace hmdbServices;

	std::set<int> allVerifiedPatients;
	std::set<int> allVerifiedPerformers;

	//schorzenia

	std::map<MedicalShallowCopy::Disorder*, std::set<MedicalShallowCopy::Patient*>> patientsByDisorders;

	auto patientsITEnd = inShallow.medicalShallowCopy.patients.end();
	for (auto patientIT = inShallow.medicalShallowCopy.patients.begin(); patientIT != patientsITEnd; ++patientIT){
		for (auto disorderIT = patientIT->second->disorders.begin(); disorderIT != patientIT->second->disorders.end(); ++disorderIT){
			patientsByDisorders[inShallow.medicalShallowCopy.disorders.find(disorderIT->first)->second].insert(patientIT->second);
		}
	}

	auto disordersITEnd = inShallow.medicalShallowCopy.disorders.end();
	for (auto disorderIT = inShallow.medicalShallowCopy.disorders.begin(); disorderIT != disordersITEnd; ++disorderIT){
		auto patientsToVerifyIT = patientsByDisorders.find(disorderIT->second);

		if (patientsToVerifyIT != patientsByDisorders.end()){
			for (auto it = patientsToVerifyIT->second.begin(); it
				!= patientsToVerifyIT->second.end(); ++it){
				allVerifiedPatients.insert((*it)->patientID);
				if ((*it)->performer != nullptr){
					allVerifiedPerformers.insert((*it)->performer->performerID);
				}
			}
		}

		if (filter->filterDisorder(disorderIT->second) == true){
			if (patientsToVerifyIT != patientsByDisorders.end()
				&& patientsToVerifyIT->second.empty() == false){
				std::set<MedicalShallowCopy::Patient*> verifiedPatients;

				//pacjenci
				for (auto patientIT = patientsToVerifyIT->second.begin(); patientIT != patientsToVerifyIT->second.end(); ++patientIT){
					if (filter->filterPatient(*patientIT) == true){
						if (filter->filterSubject((*patientIT)->performer) == true){
							std::set<MotionShallowCopy::Session*> verifiedSessions;

							//konfiguracje i sesje
							auto performerConfsITEnd = (*patientIT)->performer->performerConfs.end();
							for (auto perfConfIT = (*patientIT)->performer->performerConfs.begin(); perfConfIT != performerConfsITEnd; ++perfConfIT){
								if (filter->filterSession(perfConfIT->second->session) == true){
									std::set<MotionShallowCopy::File*> verifiedSessionFiles;
									std::set<MotionShallowCopy::Trial*> verifiedMotions;

									//filtrujemy pliki sesji
									auto filesITEnd = perfConfIT->second->session->files.end();
									for (auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
										if (filter->filterFile(fileIT->second) == true){
											auto file = new MotionShallowCopy::File();

											file->fileDescription = fileIT->second->fileDescription;
											file->fileID = fileIT->second->fileID;
											file->fileName = fileIT->second->fileName;
											//to potem uzupelniamy
											file->session = nullptr;
											file->subdirPath = fileIT->second->subdirPath;
											file->trial = nullptr;
											file->fileSize = fileIT->second->fileSize;

											verifiedSessionFiles.insert(file);
										}
									}

									//filtrujemy triale
									auto motionsITEnd = perfConfIT->second->session->trials.end();
									for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
										if (filter->filterMotion(motionIT->second) == true){
											std::set<MotionShallowCopy::File*> verifiedMotionFiles;

											//filtrujemy pliki motiona
											auto filesITEnd = motionIT->second->files.end();
											for (auto fileIT = motionIT->second->files.begin(); fileIT != filesITEnd; ++fileIT){
												if (filter->filterFile(fileIT->second) == true){
													auto file = new MotionShallowCopy::File();

													file->fileDescription = fileIT->second->fileDescription;
													file->fileID = fileIT->second->fileID;
													file->fileName = fileIT->second->fileName;
													file->session = nullptr;
													file->subdirPath = fileIT->second->subdirPath;
													file->trial = nullptr;
													file->fileSize = fileIT->second->fileSize;

													verifiedMotionFiles.insert(file);
												}
											}

											if (verifiedMotionFiles.empty() == false){
												//motion
												auto motion = new MotionShallowCopy::Trial();

												motion->attrs = motionIT->second->attrs;
												//to bedziemy p�niej aktualizowa�
												motion->session = nullptr;
												motion->trialDescription = motionIT->second->trialDescription;
												motion->trialID = motionIT->second->trialID;
												motion->trialName = motionIT->second->trialName;

												//teraz aktualizujemy pliki
												for (auto it = verifiedMotionFiles.begin(); it != verifiedMotionFiles.end(); ++it){
													(*it)->trial = motion;
													auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
													outShallow.motionShallowCopy.files.insert(val);
													motion->files.insert(val);
												}

												verifiedMotions.insert(motion);
											}
										}
									}

									if (verifiedMotions.empty() == false || verifiedSessionFiles.empty() == false){
										auto perfConf = new MotionShallowCopy::PerformerConf();
										perfConf->attrs = perfConfIT->second->attrs;
										//to ustawiamy pozniej
										perfConf->performer = nullptr;
										perfConf->performerConfID = perfConfIT->second->performerConfID;
										auto session = new MotionShallowCopy::Session();
										perfConf->session = session;

										session->attrs = perfConfIT->second->session->attrs;
										session->labID = perfConfIT->second->session->labID;
										session->motionKind = perfConfIT->second->session->motionKind;
										session->performerConf = perfConf;
										session->sessionDate = perfConfIT->second->session->sessionDate;
										session->sessionDescription = perfConfIT->second->session->sessionDescription;
										session->sessionID = perfConfIT->second->session->sessionID;
										session->sessionName = perfConfIT->second->session->sessionName;
										session->tags = perfConfIT->second->session->tags;
										session->userID = perfConfIT->second->session->userID;

										if (perfConfIT->second->session->groupAssigment != nullptr){
											auto it = outShallow.motionShallowCopy.groupAssigments.find(perfConfIT->second->session->groupAssigment->sessionGroupID);
											if (it != outShallow.motionShallowCopy.groupAssigments.end()){
												it->second->sessions[session->sessionID] = session;
												session->groupAssigment = it->second;
											}
											else{
												auto ga = new MotionShallowCopy::GroupAssigment();
												ga->sessionGroupID = perfConfIT->second->session->groupAssigment->sessionGroupID;
												ga->sessions[session->sessionID] = session;
												session->groupAssigment = ga;
												outShallow.motionShallowCopy.groupAssigments[ga->sessionGroupID] = ga;
											}
										}

										for (auto it = verifiedSessionFiles.begin(); it != verifiedSessionFiles.end(); ++it){
											(*it)->session = session;
											auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
											//zapisz plik
											outShallow.motionShallowCopy.files.insert(val);
											//aktualizuj sesj�
											session->files.insert(val);
										}

										for (auto it = verifiedMotions.begin(); it != verifiedMotions.end(); ++it){
											(*it)->session = session;
											auto val = MotionShallowCopy::Trials::value_type((*it)->trialID, *it);
											//zapisz motion
											outShallow.motionShallowCopy.trials.insert(val);
											//aktualizuj sesj� - to p�niej
											session->trials.insert(val);
										}

										verifiedSessions.insert(session);
									}
								}
							}

							if (verifiedSessions.empty() == false){
								auto patient = new MedicalShallowCopy::Patient();

								patient->patientID = (*patientIT)->patientID;
								patient->name = (*patientIT)->name;
								patient->surname = (*patientIT)->surname;
								patient->gender = (*patientIT)->gender;
								patient->birthDate = (*patientIT)->birthDate;

								//p�niej trzeba poprawi� o dost�pny zakres i nowe instancje patient->disorders

								auto performer = new MotionShallowCopy::Performer();
								performer->attrs = (*patientIT)->performer->attrs;
								performer->patient = patient;
								performer->performerID = (*patientIT)->performer->performerID;

								//aktualizuje pacjenta
								patient->performer = performer;
								patient->motionPerformerID = (*patientIT)->motionPerformerID;

								for (auto it = verifiedSessions.begin(); it != verifiedSessions.end(); ++it){
									(*it)->performerConf->performer = performer;
									auto val = MotionShallowCopy::PerformerConfs::value_type((*it)->performerConf->performerConfID, (*it)->performerConf);
									outShallow.motionShallowCopy.performerConfs.insert(val);
									performer->performerConfs.insert(val);
									outShallow.motionShallowCopy.sessions.insert(MotionShallowCopy::Sessions::value_type((*it)->sessionID, *it));
								}

								//zapisuje subjecta
								outShallow.motionShallowCopy.performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));
								//zapisuje pacjenta
								outShallow.medicalShallowCopy.patients.insert(MedicalShallowCopy::Patients::value_type(patient->patientID, patient));
								//aktualizuje grupowanie pacjent�w po schorzeniu
								verifiedPatients.insert(patient);
							}
						}
					}
				}

				if (verifiedPatients.empty() == false){
					auto disorder = new MedicalShallowCopy::Disorder();

					disorder->name = disorderIT->second->name;
					disorder->disorderID = disorderIT->second->disorderID;

					outShallow.medicalShallowCopy.disorders.insert(MedicalShallowCopy::Disorders::value_type(disorder->disorderID, disorder));

					/*
					for (auto it = verifiedPatients.begin(); it != verifiedPatients.end(); ++it){
					patients.insert(*it);
					}*/
				}
			}
		}
	}

	//uzupe�niamy choroby pacjenta
	auto outPatientsITEnd = outShallow.medicalShallowCopy.patients.end();
	for (auto patientIT = outShallow.medicalShallowCopy.patients.begin(); patientIT != outPatientsITEnd; ++patientIT){
		auto patient = inShallow.medicalShallowCopy.patients.find(patientIT->first);
		auto disordersITEnd = patient->second->disorders.end();
		for (auto disorderIT = patient->second->disorders.begin(); disorderIT != disordersITEnd; ++disorderIT){
			auto disorder = outShallow.medicalShallowCopy.disorders.find(disorderIT->first);

			if (disorder != outShallow.medicalShallowCopy.disorders.end()){
				MedicalShallowCopy::DisorderOccurence disOcc;

				disOcc.disorder = disorder->second;
				disOcc.comments = disorderIT->second.comments;
				disOcc.diagnosisDate = disorderIT->second.diagnosisDate;
				disOcc.focus = disorderIT->second.focus;

				patientIT->second->disorders.insert(MedicalShallowCopy::PatientDisorders::value_type(disorder->first, disOcc));
			}
		}
	}

	//pacjenci
	for (auto patientIT = inShallow.medicalShallowCopy.patients.begin();
		patientIT != inShallow.medicalShallowCopy.patients.end(); ++patientIT){
		if (allVerifiedPatients.find(patientIT->first) != allVerifiedPatients.end()){
			continue;
		}

		if (patientIT->second->performer != nullptr){
			allVerifiedPerformers.insert(patientIT->second->performer->performerID);
		}

		if (filter->filterPatient(patientIT->second) == true){
			if (filter->filterSubject(patientIT->second->performer) == true){
				std::set<MotionShallowCopy::Session*> verifiedSessions;

				//konfiguracje i sesje
				auto performerConfsITEnd = patientIT->second->performer->performerConfs.end();
				for (auto perfConfIT = patientIT->second->performer->performerConfs.begin(); perfConfIT != performerConfsITEnd; ++perfConfIT){
					if (filter->filterSession(perfConfIT->second->session) == true){
						std::set<MotionShallowCopy::File*> verifiedSessionFiles;
						std::set<MotionShallowCopy::Trial*> verifiedMotions;

						//filtrujemy pliki sesji
						auto filesITEnd = perfConfIT->second->session->files.end();
						for (auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
							if (filter->filterFile(fileIT->second) == true){
								auto file = new MotionShallowCopy::File();

								file->fileDescription = fileIT->second->fileDescription;
								file->fileID = fileIT->second->fileID;
								file->fileName = fileIT->second->fileName;
								//to potem uzupelniamy
								file->session = nullptr;
								file->subdirPath = fileIT->second->subdirPath;
								file->trial = nullptr;
								file->fileSize = fileIT->second->fileSize;

								verifiedSessionFiles.insert(file);
							}
						}

						//filtrujemy triale
						auto motionsITEnd = perfConfIT->second->session->trials.end();
						for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
							if (filter->filterMotion(motionIT->second) == true){
								std::set<MotionShallowCopy::File*> verifiedMotionFiles;

								//filtrujemy pliki motiona
								auto filesITEnd = motionIT->second->files.end();
								for (auto fileIT = motionIT->second->files.begin(); fileIT != filesITEnd; ++fileIT){
									if (filter->filterFile(fileIT->second) == true){
										auto file = new MotionShallowCopy::File();

										file->fileDescription = fileIT->second->fileDescription;
										file->fileID = fileIT->second->fileID;
										file->fileName = fileIT->second->fileName;
										file->session = nullptr;
										file->subdirPath = fileIT->second->subdirPath;
										file->trial = nullptr;
										file->fileSize = fileIT->second->fileSize;

										verifiedMotionFiles.insert(file);
									}
								}

								if (verifiedMotionFiles.empty() == false){
									//motion
									auto motion = new MotionShallowCopy::Trial();

									motion->attrs = motionIT->second->attrs;
									//to bedziemy p�niej aktualizowa�
									motion->session = nullptr;
									motion->trialDescription = motionIT->second->trialDescription;
									motion->trialID = motionIT->second->trialID;
									motion->trialName = motionIT->second->trialName;

									//teraz aktualizujemy pliki
									for (auto it = verifiedMotionFiles.begin(); it != verifiedMotionFiles.end(); ++it){
										(*it)->trial = motion;
										auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
										outShallow.motionShallowCopy.files.insert(val);
										motion->files.insert(val);
									}

									verifiedMotions.insert(motion);
								}
							}
						}

						if (verifiedMotions.empty() == false || verifiedSessionFiles.empty() == false){
							auto perfConf = new MotionShallowCopy::PerformerConf();
							perfConf->attrs = perfConfIT->second->attrs;
							//to ustawiamy pozniej
							perfConf->performer = nullptr;
							perfConf->performerConfID = perfConfIT->second->performerConfID;
							auto session = new MotionShallowCopy::Session();
							perfConf->session = session;

							session->attrs = perfConfIT->second->session->attrs;
							session->labID = perfConfIT->second->session->labID;
							session->motionKind = perfConfIT->second->session->motionKind;
							session->performerConf = perfConf;
							session->sessionDate = perfConfIT->second->session->sessionDate;
							session->sessionDescription = perfConfIT->second->session->sessionDescription;
							session->sessionID = perfConfIT->second->session->sessionID;
							session->sessionName = perfConfIT->second->session->sessionName;
							session->tags = perfConfIT->second->session->tags;
							session->userID = perfConfIT->second->session->userID;

							if (perfConfIT->second->session->groupAssigment != nullptr){
								auto it = outShallow.motionShallowCopy.groupAssigments.find(perfConfIT->second->session->groupAssigment->sessionGroupID);
								if (it != outShallow.motionShallowCopy.groupAssigments.end()){
									it->second->sessions[session->sessionID] = session;
									session->groupAssigment = it->second;
								}
								else{
									auto ga = new MotionShallowCopy::GroupAssigment();
									ga->sessionGroupID = perfConfIT->second->session->groupAssigment->sessionGroupID;
									ga->sessions[session->sessionID] = session;
									session->groupAssigment = ga;
									outShallow.motionShallowCopy.groupAssigments[ga->sessionGroupID] = ga;
								}
							}

							for (auto it = verifiedSessionFiles.begin(); it != verifiedSessionFiles.end(); ++it){
								(*it)->session = session;
								auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
								//zapisz plik
								outShallow.motionShallowCopy.files.insert(val);
								//aktualizuj sesj�
								session->files.insert(val);
							}

							for (auto it = verifiedMotions.begin(); it != verifiedMotions.end(); ++it){
								(*it)->session = session;
								auto val = MotionShallowCopy::Trials::value_type((*it)->trialID, *it);
								//zapisz motion
								outShallow.motionShallowCopy.trials.insert(val);
								//aktualizuj sesj� - to p�niej
								session->trials.insert(val);
							}

							verifiedSessions.insert(session);
						}
					}
				}

				if (verifiedSessions.empty() == false){
					auto patient = new MedicalShallowCopy::Patient();

					patient->patientID = patientIT->second->patientID;
					patient->name = patientIT->second->name;
					patient->surname = patientIT->second->surname;
					patient->gender = patientIT->second->gender;
					patient->birthDate = patientIT->second->birthDate;

					//p�niej trzeba poprawi� o dost�pny zakres i nowe instancje patient->disorders

					auto performer = new MotionShallowCopy::Performer();
					performer->attrs = patientIT->second->performer->attrs;
					performer->patient = patient;
					performer->performerID = patientIT->second->performer->performerID;

					//aktualizuje pacjenta
					patient->performer = performer;
					patient->motionPerformerID = patientIT->second->motionPerformerID;

					for (auto it = verifiedSessions.begin(); it != verifiedSessions.end(); ++it){
						(*it)->performerConf->performer = performer;
						auto val = MotionShallowCopy::PerformerConfs::value_type((*it)->performerConf->performerConfID, (*it)->performerConf);
						outShallow.motionShallowCopy.performerConfs.insert(val);
						performer->performerConfs.insert(val);
						outShallow.motionShallowCopy.sessions.insert(MotionShallowCopy::Sessions::value_type((*it)->sessionID, *it));
					}

					//zapisuje subjecta
					outShallow.motionShallowCopy.performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));
					//zapisuje pacjenta
					outShallow.medicalShallowCopy.patients.insert(MedicalShallowCopy::Patients::value_type(patient->patientID, patient));
				}
			}
		}
	}

	for (auto subjectIT = inShallow.motionShallowCopy.performers.begin();
		subjectIT != inShallow.motionShallowCopy.performers.end(); ++subjectIT){
		if (allVerifiedPerformers.find(subjectIT->first) != allVerifiedPerformers.end()){
			continue;
		}

		if (filter->filterSubject(subjectIT->second) == true){
			std::set<MotionShallowCopy::Session*> verifiedSessions;

			//konfiguracje i sesje
			auto performerConfsITEnd = subjectIT->second->performerConfs.end();
			for (auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != performerConfsITEnd; ++perfConfIT){
				if (filter->filterSession(perfConfIT->second->session) == true){
					std::set<MotionShallowCopy::File*> verifiedSessionFiles;
					std::set<MotionShallowCopy::Trial*> verifiedMotions;

					//filtrujemy pliki sesji
					auto filesITEnd = perfConfIT->second->session->files.end();
					for (auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
						if (filter->filterFile(fileIT->second) == true){
							auto file = new MotionShallowCopy::File();

							file->fileDescription = fileIT->second->fileDescription;
							file->fileID = fileIT->second->fileID;
							file->fileName = fileIT->second->fileName;
							//to potem uzupelniamy
							file->session = nullptr;
							file->subdirPath = fileIT->second->subdirPath;
							file->trial = nullptr;
							file->fileSize = fileIT->second->fileSize;

							verifiedSessionFiles.insert(file);
						}
					}

					//filtrujemy triale
					auto motionsITEnd = perfConfIT->second->session->trials.end();
					for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
						if (filter->filterMotion(motionIT->second) == true){
							std::set<MotionShallowCopy::File*> verifiedMotionFiles;

							//filtrujemy pliki motiona
							auto filesITEnd = motionIT->second->files.end();
							for (auto fileIT = motionIT->second->files.begin(); fileIT != filesITEnd; ++fileIT){
								if (filter->filterFile(fileIT->second) == true){
									auto file = new MotionShallowCopy::File();

									file->fileDescription = fileIT->second->fileDescription;
									file->fileID = fileIT->second->fileID;
									file->fileName = fileIT->second->fileName;
									file->session = nullptr;
									file->subdirPath = fileIT->second->subdirPath;
									file->trial = nullptr;
									file->fileSize = fileIT->second->fileSize;

									verifiedMotionFiles.insert(file);
								}
							}

							if (verifiedMotionFiles.empty() == false){
								//motion
								auto motion = new MotionShallowCopy::Trial();

								motion->attrs = motionIT->second->attrs;
								//to bedziemy p�niej aktualizowa�
								motion->session = nullptr;
								motion->trialDescription = motionIT->second->trialDescription;
								motion->trialID = motionIT->second->trialID;
								motion->trialName = motionIT->second->trialName;

								//teraz aktualizujemy pliki
								for (auto it = verifiedMotionFiles.begin(); it != verifiedMotionFiles.end(); ++it){
									(*it)->trial = motion;
									auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
									outShallow.motionShallowCopy.files.insert(val);
									motion->files.insert(val);
								}

								verifiedMotions.insert(motion);
							}
						}
					}

					if (verifiedMotions.empty() == false || verifiedSessionFiles.empty() == false){
						auto perfConf = new MotionShallowCopy::PerformerConf();
						perfConf->attrs = perfConfIT->second->attrs;
						//to ustawiamy pozniej
						perfConf->performer = nullptr;
						perfConf->performerConfID = perfConfIT->second->performerConfID;
						auto session = new MotionShallowCopy::Session();
						perfConf->session = session;

						session->attrs = perfConfIT->second->session->attrs;
						session->labID = perfConfIT->second->session->labID;
						session->motionKind = perfConfIT->second->session->motionKind;
						session->performerConf = perfConf;
						session->sessionDate = perfConfIT->second->session->sessionDate;
						session->sessionDescription = perfConfIT->second->session->sessionDescription;
						session->sessionID = perfConfIT->second->session->sessionID;
						session->sessionName = perfConfIT->second->session->sessionName;
						session->tags = perfConfIT->second->session->tags;
						session->userID = perfConfIT->second->session->userID;

						if (perfConfIT->second->session->groupAssigment != nullptr){
							auto it = outShallow.motionShallowCopy.groupAssigments.find(perfConfIT->second->session->groupAssigment->sessionGroupID);
							if (it != outShallow.motionShallowCopy.groupAssigments.end()){
								it->second->sessions[session->sessionID] = session;
								session->groupAssigment = it->second;
							}
							else{
								auto ga = new MotionShallowCopy::GroupAssigment();
								ga->sessionGroupID = perfConfIT->second->session->groupAssigment->sessionGroupID;
								ga->sessions[session->sessionID] = session;
								session->groupAssigment = ga;
								outShallow.motionShallowCopy.groupAssigments[ga->sessionGroupID] = ga;
							}
						}

						for (auto it = verifiedSessionFiles.begin(); it != verifiedSessionFiles.end(); ++it){
							(*it)->session = session;
							auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
							//zapisz plik
							outShallow.motionShallowCopy.files.insert(val);
							//aktualizuj sesj�
							session->files.insert(val);
						}

						for (auto it = verifiedMotions.begin(); it != verifiedMotions.end(); ++it){
							(*it)->session = session;
							auto val = MotionShallowCopy::Trials::value_type((*it)->trialID, *it);
							//zapisz motion
							outShallow.motionShallowCopy.trials.insert(val);
							//aktualizuj sesj� - to p�niej
							session->trials.insert(val);
						}

						verifiedSessions.insert(session);
					}
				}
			}

			if (verifiedSessions.empty() == false){
				//p�niej trzeba poprawi� o dost�pny zakres i nowe instancje patient->disorders

				auto performer = new MotionShallowCopy::Performer();
				performer->attrs = subjectIT->second->attrs;
				performer->patient = nullptr;
				performer->performerID = subjectIT->second->performerID;

				for (auto it = verifiedSessions.begin(); it != verifiedSessions.end(); ++it){
					(*it)->performerConf->performer = performer;
					auto val = MotionShallowCopy::PerformerConfs::value_type((*it)->performerConf->performerConfID, (*it)->performerConf);
					outShallow.motionShallowCopy.performerConfs.insert(val);
					performer->performerConfs.insert(val);
					outShallow.motionShallowCopy.sessions.insert(MotionShallowCopy::Sessions::value_type((*it)->sessionID, *it));
				}

				//zapisuje subjecta
				outShallow.motionShallowCopy.performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));
			}
		}
	}
}

void DataViewWidget::setFilter(hmdbCommunication::ShallowCopyFilter * filter)
{
	filter_ = filter;
	setShallowCopy(shallowCopyContext_->shallowCopyDataContext()->shallowCopy());
}

void DataViewWidget::setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy)
{	
	shallowCopy_ = shallowCopy;

	if (filter_ != nullptr && shallowCopy_ != nullptr){		
		plugin::getJobManager()->addJob("DataView", "FilterShallowCopy",
			threadingUtils::IRunnablePtr(new threadingUtils::FunctorRunnable(boost::bind(&DataViewWidget::filterShallowCopy, this))));		
	}
	else{
		tryRebuildDataStatus();
		rebuildPerspective();
	}	
}

void DataViewWidget::rawFilterShallowCopy()
{
	hmdbCommunication::ShallowCopyPtr newShallowCopy(new hmdbCommunication::ShallowCopy);
	::filterShallowCopy(*shallowCopy_, *newShallowCopy, filter_);
	shallowCopy_ = newShallowCopy;
	tryRebuildDataStatus();	
}


void DataViewWidget::filterShallowCopy()
{
	rawFilterShallowCopy();
	QMetaObject::invokeMethod(this, "rebuildPerspective");
}

const hmdbCommunication::IHMDBRemoteContext::OperationConstPtr DataViewWidget::operation() const
{
	return operation_;
}

void DataViewWidget::selectItem(char type, int id)
{

}

void dataItems(const hmdbCommunication::TreeWidgetContentItem * contentItem, std::map<hmdbCommunication::DataType, std::set<hmdbServices::ID>> & dItems)
{
	using namespace hmdbCommunication;

	hmdbCommunication::DataType dt = hmdbCommunication::FileType;

	if (mapContentTypeToDataType(contentItem->contentType(), dt) == true){
		dItems[dt].insert(contentItem->id());
	}else{
		for (int i = 0; i < contentItem->childCount(); ++i){
			auto cItem = dynamic_cast<TreeWidgetContentItem*>(contentItem->child(i));
			if (cItem != nullptr){
				dataItems(cItem, dItems);
			}
		}
	}	
}

void DataViewWidget::onLoad()
{
	coreUI::CoreCursorChanger cursorChanger;

	const auto sItems = ui->treeWidget->selectedItems();

	if (sItems.isEmpty() == false){

		auto sItem = sItems.first();

		auto contentItem = dynamic_cast<hmdbCommunication::TreeWidgetContentItem*>(sItem);

		if (contentItem != nullptr){
			try{

				std::map<hmdbCommunication::DataType, std::set<hmdbServices::ID>> items;

				dataItems(contentItem, items);

				for (auto it = items.begin(); it != items.end(); ++it){
					for (auto iIT = it->second.begin(); iIT != it->second.end(); ++iIT){
						shallowCopyContext_->shallowCopyLocalContext()->load(it->first, *iIT);
					}
				}				
			}
			catch (...){

			}
		}
	}
}

void DataViewWidget::onLoadAll()
{
	coreUI::CoreCursorChanger cursorChanger;

	try{
		shallowCopyContext_->shallowCopyLocalContext()->loadAll();
	}
	catch (...){

	}
}

void DataViewWidget::onUnload()
{
	coreUI::CoreCursorChanger cursorChanger;

	const auto sItems = ui->treeWidget->selectedItems();

	if (sItems.isEmpty() == false){

		auto sItem = sItems.first();

		auto contentItem = dynamic_cast<hmdbCommunication::TreeWidgetContentItem*>(sItem);

		if (contentItem != nullptr){
			try{

				std::map<hmdbCommunication::DataType, std::set<hmdbServices::ID>> items;

				dataItems(contentItem, items);

				for (auto it = items.begin(); it != items.end(); ++it){
					for (auto iIT = it->second.begin(); iIT != it->second.end(); ++iIT){
						shallowCopyContext_->shallowCopyLocalContext()->unload(it->first, *iIT);
					}
				}
			}
			catch (...){

			}
		}
	}
}

void DataViewWidget::onUnloadAll()
{
	coreUI::CoreCursorChanger cursorChanger;

	try{
		shallowCopyContext_->shallowCopyLocalContext()->unloadAll();
	}
	catch (...){

	}
}

void DataViewWidget::onRebuild()
{
	rebuildPerspective();
}

void DataViewWidget::onRefreshStatus()
{
	if (tryRebuildDataStatus() == true){
		refreshDataStatus(true);
	}	
}

void execOperation(hmdbCommunication::IHMDBRemoteContext::OperationPtr operation, utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger)
{
	operation->start();
	operation->wait();
	cursorChanger->restore();
}

void DataViewWidget::onSynchronizeFinished()
{
	rebuildPerspective();
	setRemoteOperationsEnabled(true);
}

void DataViewWidget::synchronize(hmdbCommunication::IHMDBRemoteContext::SynchronizeOperationPtr sOp,
	utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger)
{
	shallowCopyContext_->shallowCopyRemoteContext()->synchronize(sOp);
	shallowCopy_ = shallowCopyContext_->shallowCopyDataContext()->shallowCopy();

	if (filter_ != nullptr && shallowCopy_ != nullptr){
		rawFilterShallowCopy();
	}

	//onRefreshStatus();	
	QMetaObject::invokeMethod(this, "onSynchronizeFinished", Qt::BlockingQueuedConnection);
	operation_.reset();
	cursorChanger->restore();
}

void DataViewWidget::onTrySynchronize()
{
	if (shallowCopyContext_->shallowCopyRemoteContext() != nullptr){
		bool synch = true;

		if (shallowCopy_ != nullptr){
			const auto mq = shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->session()->motionQueries();
			if (mq != nullptr){
				const auto dbTime = mq->dataModificationTime();
				synch = hmdbCommunication::ShallowCopyUtils::shallowCopyRequiresRefresh(*shallowCopy_, dbTime);
			}
		}

		if (synch == true){
			onSynchronize();
		}
	}
}

void DataViewWidget::onForceSynchronize()
{
	onSynchronize();
}

void DataViewWidget::onSynchronize()
{	
	try{
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger(new coreUI::CoreCursorChanger);

		auto sOp = shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->prepareSynchronization(shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->storage());

		threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(boost::bind(&DataViewWidget::synchronize, this, sOp, cursorChanger)));

		core::IThreadPool::Threads threads;
		plugin::getThreadPool()->getThreads("DataViewWidget", threads, 1);
		if (threads.empty() == false){
			remoteOperationThread = threads.front();
			operation_ = sOp;
			setRemoteOperationsEnabled(false);
			emit operationAboutToStart(0);
			remoteOperationThread->start(runnable);
		}
	}
	catch (...){

	}	
}

const hmdbCommunication::StorageFileNames extractItemFiles(const QTreeWidgetItem* item,
	const hmdbCommunication::ShallowCopy & shalowCopy)
{
	hmdbCommunication::StorageFileNames ret;

	bool found = false;
	const auto contentItem = dynamic_cast<const hmdbCommunication::TreeWidgetContentItem*>(item);
	if (contentItem != nullptr){

		hmdbCommunication::DataType dt = hmdbCommunication::FileType;

		if (mapContentTypeToDataType(contentItem->contentType(), dt) == true){
			const auto sf = ShallowCopyUtils::files(dt, contentItem->id(), shalowCopy, true);
			ret.insert(sf.begin(), sf.end());
			found = true;
		}
	}

	if (found == false){
		for (int i = 0; i < item->childCount(); ++i){
			const auto sf = extractItemFiles(item->child(i), shalowCopy);
			ret.insert(sf.begin(), sf.end());
		}
	}

	return ret;
}

void DataViewWidget::onDownloadFinished()
{
	onRefreshStatus();
	setRemoteOperationsEnabled(true);
}

void DataViewWidget::download(const std::list<hmdbCommunication::IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger)
{
	std::for_each(downloads.begin(), downloads.end(), [&](hmdbCommunication::IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->start();
	});

	std::for_each(downloads.begin(), downloads.end(), [&](hmdbCommunication::IHMDBRemoteContext::DownloadOperationPtr d)
	{
		d->wait();
	});

	onRefreshStatus();
	QMetaObject::invokeMethod(this, "onDownloadFinished", Qt::BlockingQueuedConnection);
	operation_.reset();
	cursorChanger->restore();
}

const QString formatFileSize(const unsigned long long size)
{
	double num = size;
	QStringList list;
	list << QObject::tr("KB(s)") << QObject::tr("MB(s)") << QObject::tr("GB(s)") << QObject::tr("TB(s)");

	QStringListIterator i(list);
	QString unit(QObject::tr("byte(s)"));

	while (num >= 1024.0 && i.hasNext())
	{
		unit = i.next();
		num /= 1024.0;
	}

	return QString().setNum(num, 'd', 2) + " " + unit;
}

void DataViewWidget::setupDownload(const hmdbCommunication::StorageFileNames & files)
{
	utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger(new coreUI::CoreCursorChanger);

	try{

		unsigned long long size = 0;

		std::list<hmdbCommunication::IHMDBRemoteContext::DownloadOperationPtr> downloads;
		auto storage = shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->storage();

		std::for_each(files.begin(), files.end(), [&](const hmdbCommunication::StorageFileNames::value_type & i)
		{
			downloads.push_back(shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->prepareFileDownload(i.second,
				storage));
			size += i.second.fileSize;
		});

		if (storage->canStore(size) == false){
			QMessageBox::warning(this, tr("Download disk space requirements"), tr("Requested download requires %1 to store and it exceeds capability of the storage. Please limit amount of elements for download and try again.").arg(formatFileSize(size)));
			return;
		}

		if (storage->shareDiskSpace(plugin::getPaths()->getTmpPath()) == true){
			size <<= 1;
		}
		
		const auto aSpace = core::Filesystem::availableSpace(plugin::getPaths()->getTmpPath());

		if (size > aSpace){
			QMessageBox::warning(this, tr("Download disk space requirements"), tr("Requested download requires %1 to store the data while only %2 is available. Please ensure there is enough space on the disk for download and try again.").arg(formatFileSize(size)).arg(formatFileSize(aSpace)));
			return;
		}

		threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(boost::bind(&DataViewWidget::download, this, downloads, cursorChanger)));

		core::IThreadPool::Threads threads;
		plugin::getThreadPool()->getThreads("DataViewWidget", threads, 1);
		if (threads.empty() == false){
			remoteOperationThread = threads.front();
			//operation_ = cdOp;
			setRemoteOperationsEnabled(false);
			emit operationAboutToStart(0);
			remoteOperationThread->start(runnable);
		}
	}
	catch (...){

	}
}

const hmdbCommunication::StorageFileNames filterOutLocalFiles(const const hmdbCommunication::StorageFileNames & inputFiles,
	hmdbCommunication::IHMDBStoragePtr storage)
{
	hmdbCommunication::StorageFileNames ret;
	auto storageT = storage->transaction();

	std::for_each(inputFiles.begin(), inputFiles.end(), [&](const hmdbCommunication::StorageFileNames::value_type & i)
	{
		if (storageT->exists(i.second.fileName) == false){
			ret.insert(i);
		}
	});

	return ret;
}

void DataViewWidget::onDownload()
{
	const auto sItems = ui->treeWidget->selectedItems();

	if (shallowCopy_ != nullptr && shallowCopyContext_->shallowCopyRemoteContext() != nullptr && sItems.empty() == false){

		coreUI::CoreCursorChanger cc;

		hmdbCommunication::StorageFileNames files;

		std::for_each(sItems.begin(), sItems.end(), [&](const QTreeWidgetItem* i)
		{	
			auto sf = extractItemFiles(i, *shallowCopy_);
			files.insert(sf.begin(), sf.end());
		});

		if (files.empty() == true){
			return;
		}

		hmdbCommunication::StorageFileNames toDownloadFiles = filterOutLocalFiles(files, shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->storage());

		if (toDownloadFiles.empty() == true){
			return;
		}

		setupDownload(toDownloadFiles);
	}
}

void DataViewWidget::onDownloadAll()
{
	if (shallowCopy_ != nullptr && shallowCopyContext_->shallowCopyRemoteContext() != nullptr){

		coreUI::CoreCursorChanger cc;

		hmdbCommunication::StorageFileNames files = ShallowCopyUtils::files(*shallowCopy_);

		if (files.empty() == true){
			return;
		}

		hmdbCommunication::StorageFileNames toDownloadFiles = filterOutLocalFiles(files, shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->storage());

		if (toDownloadFiles.empty() == true){
			return;
		}

		setupDownload(toDownloadFiles);
	}
}

void DataViewWidget::onForceDownload()
{
	const auto sItems = ui->treeWidget->selectedItems();

	if (shallowCopy_ != nullptr && shallowCopyContext_->shallowCopyRemoteContext() != nullptr && sItems.empty() == false){

		coreUI::CoreCursorChanger cc;

		hmdbCommunication::StorageFileNames files;

		std::for_each(sItems.begin(), sItems.end(), [&](const QTreeWidgetItem* i)
		{
			auto sf = extractItemFiles(i, *shallowCopy_);
			files.insert(sf.begin(), sf.end());
		});

		if (files.empty() == true){
			return;
		}		

		setupDownload(files);
	}
}

void DataViewWidget::onForceDownloadAll()
{
	if (shallowCopy_ != nullptr && shallowCopyContext_->shallowCopyRemoteContext() != nullptr){

		coreUI::CoreCursorChanger cc;

		hmdbCommunication::StorageFileNames files = ShallowCopyUtils::files(*shallowCopy_);

		if (files.empty() == true){
			return;
		}		

		setupDownload(files);
	}
}

void DataViewWidget::extractItem(const QTreeWidgetItem * item, char & id, char & type)
{
	id = -1;
	type = -1;

	if (item == nullptr){
		return;
	}

	const auto contentItem = dynamic_cast<const hmdbCommunication::TreeWidgetContentItem*>(item);
	if (contentItem != nullptr){

		hmdbCommunication::DataType dt = hmdbCommunication::FileType;

		if (mapContentTypeToDataType(contentItem->contentType(), dt) == true){
			type = dt;
			id = contentItem->id();
		}		
	}
}

void DataViewWidget::onTreeCurrentItemChange(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	char currentID = -1;
	char currentType = -1;
	char previousID = -1;
	char previousType = -1;

	extractItem(current, currentID, currentType);
	extractItem(previous, previousID, previousType);

	if (currentID != previousID && currentType != previousType){
		emit currentItemChanged(currentType, currentID, previousType, previousID);
	}
}

void DataViewWidget::setRemoteOperationsEnabled(const bool enable)
{
	ui->actionSynchronize->setEnabled(enable);
	ui->actionForceSynchronize->setEnabled(enable);
	ui->actionDownload->setEnabled(enable);
	ui->actionDownloadAll->setEnabled(enable);
	ui->actionForceDownload->setEnabled(enable);
}

void DataViewWidget::setLocalOperationsEnabled(const bool enable)
{
	ui->actionLoad->setEnabled(enable);
	ui->actionLoadAll->setEnabled(enable);
	ui->actionUnload->setEnabled(enable);
	ui->actionUnloadAll->setEnabled(enable);
	ui->actionRebuild->setEnabled(enable);
	ui->actionRefreshStatus->setEnabled(enable);
}