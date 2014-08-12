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
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include "ShallowCopyUtils.h"
#include <QtGui/QPainter>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>


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

	//zaczynamy ze sposobem sk³adowania
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

	// teraz stan u¿ycia
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

		// teraz stan u¿ycia
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

	//// mamy ju¿ wszystko - generujemy ikonê
	return ret;
}

DataViewWidget::DataViewWidget(hmdbCommunication::IHMDBShallowCopyContext * shallowCopyContext,
	hmdbCommunication::IDataSourcePerspective * perspective,
	hmdbCommunication::IDataSourceContent * content,
	QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::DataViewWidget),
	perspective_(perspective), content_(content), shallowCopyContext_(shallowCopyContext), delPerspective(false)
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

	initializeShallowCopy();

	if (shallowCopyContext_->shallowCopyRemoteContext() == nullptr){
		ui->treeWidget->setIconSize(QSize(40, 20));
	}
	else{
		ui->treeWidget->setIconSize(QSize(60, 20));
	}
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
	ui->treeWidget->addAction(ui->actionDownload);
	ui->treeWidget->addAction(ui->actionDownloadAll);
	ui->treeWidget->addAction(ui->actionForceDownload);
}

void DataViewWidget::initializeShallowCopy()
{
	if (shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->shallowCopyExists() == false){
		if (shallowCopyContext_->shallowCopyRemoteContext() == nullptr){
			QTimer::singleShot(0, this, SLOT(onNoShallowCopyInStorageAndLocalOnly()));
		}
		else{			
			//fire once synch
			QTimer::singleShot(0, this, SLOT(onNoShallowCopyInStorage()));
		}
	}
	else{
		if (shallowCopyContext_->shallowCopyRemoteContext() == nullptr){
			setRemoteOperationsEnabled(false);
			QTimer::singleShot(0, this, SLOT(onExtractLocalShallowCopy()));
		}
		else{			
			//fire once synch
			QTimer::singleShot(0, this, SLOT(onExtractLocalShallowCopyAndTryUpdate()));
		}
	}
}

void DataViewWidget::onExtractLocalShallowCopy()
{
	coreUI::CoreCursorChanger cc;
	auto sc = shallowCopyContext_->shallowCopyLocalContext()->localContext()->dataContext()->createShallowCopy();
	setShallowCopy(sc);
}

void DataViewWidget::onExtractLocalShallowCopyAndTryUpdate()
{
	coreUI::CoreCursorChanger cc;
	onExtractLocalShallowCopy();
	
	const auto mq = shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->session()->motionQueries();
	if (mq != nullptr){
		const auto dbTime = mq->dataModificationTime();
		if (hmdbCommunication::ShallowCopyUtils::shallowCopyRequiresRefresh(*shallowCopy_, dbTime) == true){
			const auto ret = QMessageBox::question(this, tr("Synchronization required"), tr("Since last synchronization (%1) some changes were made in database (last: %2). It is suggested to synchronize data. Would You like to synchronize now?").arg(QString::fromStdString(shallowCopy_->motionShallowCopy.timestamp.toString())).arg(QString::fromStdString(dbTime.toString())));
			if (ret == QMessageBox::StandardButton::Yes){
				onSynchronize();
			}
		}
	}	
}

void DataViewWidget::onNoShallowCopyInStorageAndLocalOnly()
{
	setLocalOperationsEnabled(false);
	setRemoteOperationsEnabled(false);
	QMessageBox::warning(this, tr("No data"), tr("You are working in offline mode and there is no data for You in the storage. All operations are disabled. Try to switch to online mode or change storage."));
}

void DataViewWidget::onNoShallowCopyInStorage()
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
		perspective_->rebuildPerspective(ui->treeWidget->invisibleRootItem(), *shallowCopy_);
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

const hmdbCommunication::DataStatus DataViewWidget::refrshItemContent(QTreeWidgetItem * item,
	bool perspectiveFirst)
{	
	int statusColumn = -1;
	QStringList headers;

	//zawartoœæ wg regu³ pierwszeñstwa i stanu wype³nienia
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
	
	//odœwie¿am zawartoœc dzieci jeœli nie znamy typu obiektu
	const auto childrenCount = item->childCount();
	for (int i = 0; i < childrenCount; ++i){
		status |= refrshItemContent(item->child(i), perspectiveFirst);
	}

	//odœwie¿am status
	if (statusColumn == -1 || statusColumn >= headers.size()){
		statusColumn = 0;
	}

	item->setIcon(statusColumn, statusIcon(status));

	//TODO
	//stan œci¹gania dla elementów

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

void DataViewWidget::setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy)
{
	shallowCopy_ = shallowCopy;
	rebuildPerspective();
}

const hmdbCommunication::IHMDBRemoteContext::OperationConstPtr DataViewWidget::operation() const
{
	return operation_;
}

void DataViewWidget::selectItem(char type, int id)
{

}

void dataItems(hmdbCommunication::TreeWidgetContentItem * contentItem, std::map<hmdbCommunication::DataType, std::set<hmdbServices::ID>> & dItems)
{
	using namespace hmdbCommunication;

	switch (contentItem->contentType())
	{
	case PatientContent:
		dItems[PatientType].insert(contentItem->id());
		break;
	case SubjectContent:
		dItems[SubjectType].insert(contentItem->id());
		break;
	case SessionContent:
		dItems[SessionType].insert(contentItem->id());
		break;
	case MotionContent:
		dItems[MotionType].insert(contentItem->id());
		break;
	case FileContent:
		dItems[FileType].insert(contentItem->id());
		break;

	case PatientsGroupContent:
	case SubjectsGroupContent:
	case SessionsGroupContent:
	case MotionsGroupContent:
	case FilesGroupContent:
		for (int i = 0; i < contentItem->childCount(); ++i){
			auto cItem = dynamic_cast<TreeWidgetContentItem*>(contentItem->child(i));
			if (cItem != nullptr){
				dataItems(cItem, dItems);
			}
		}
		break;
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

}

void execOperation(hmdbCommunication::IHMDBRemoteContext::OperationPtr operation, utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger)
{
	operation->start();
	operation->wait();
	cursorChanger->restore();
}

void DataViewWidget::onSynchronizeFinished()
{
	onRebuild();
	setRemoteOperationsEnabled(true);
}

void DataViewWidget::synchronize(hmdbCommunication::IHMDBRemoteContext::SynchronizeOperationPtr sOp,
	utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger)
{
	shallowCopyContext_->shallowCopyRemoteContext()->synchronize(sOp);
	shallowCopy_ = shallowCopyContext_->shallowCopyDataContext()->shallowCopy();
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

void DataViewWidget::onSynchronize()
{	
	try{
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger(new coreUI::CoreCursorChanger);

		auto sOp = shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->prepareShallowCopyDownload();

		threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(boost::bind(&DataViewWidget::synchronize, this, sOp, cursorChanger)));

		core::IThreadPool::Threads threads;
		plugin::getThreadPool()->getThreads("DataViewWidget", threads, 1);
		if (threads.empty() == false){
			thread = threads.front();
			operation_ = sOp;
			setRemoteOperationsEnabled(false);
			emit operationAboutToStart(0);
			thread->start(runnable);
		}
	}
	catch (...){

	}	
}

void DataViewWidget::onDownload()
{
	if (shallowCopyContext_->shallowCopyRemoteContext() != nullptr){

		try{
			utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger(new coreUI::CoreCursorChanger);

			auto sOp = shallowCopyContext_->shallowCopyRemoteContext()->remoteContext()->prepareShallowCopyDownload();

			threadingUtils::IRunnablePtr runnable(new threadingUtils::FunctorRunnable(boost::bind(&DataViewWidget::synchronize, this, sOp, cursorChanger)));

			core::IThreadPool::Threads threads;
			plugin::getThreadPool()->getThreads("DataViewWidget", threads, 1);
			if (threads.empty() == false){
				thread = threads.front();
				operation_ = sOp;
				setRemoteOperationsEnabled(false);
				emit operationAboutToStart(0);
				thread->start(runnable);
			}
		}
		catch (...){

		}
	}
}

void DataViewWidget::onDownloadAll()
{

}

void DataViewWidget::onForceDownload()
{

}

void DataViewWidget::onForceDownloadAll()
{

}

void DataViewWidget::onTreeCurrentItemChange(QTreeWidgetItem* previous, QTreeWidgetItem* current)
{

}

void DataViewWidget::setRemoteOperationsEnabled(const bool enable)
{
	ui->actionSynchronize->setEnabled(enable);
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