#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include "ui_DataViewWidget.h"
#include <plugins/hmdbCommunication/IDataSourceContent.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <boost/bind.hpp>
#include <corelib/IThreadPool.h>
#include "DataSourcePerspective.h"

DataViewWidget::DataViewWidget(hmdbCommunication::IHMDBShallowCopyContext * shallowCopyContext,
	hmdbCommunication::IDataSourcePerspective * perspective,
	QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::DataViewWidget),
	perspective_(perspective), shallowCopyContext_(shallowCopyContext), delPerspective(false)
{
	ui->setupUi(this);

	if (shallowCopyContext->shallowCopyRemoteContext() == nullptr){
		setRemoteOperationsEnabled(false);
	}

	if (perspective_ == nullptr){
		perspective_ = new hmdbCommunication::DataSourcePatientPerspective;
		delPerspective = true;
	}
}

DataViewWidget::~DataViewWidget()
{
	if (delPerspective == true && perspective_ != nullptr){
		delete perspective_;
	}
}

void DataViewWidget::rebuildPerspective()
{
	ui->treeWidget->clear();
	if (shallowCopy_ != nullptr){
		perspective_->rebuildPerspective(ui->treeWidget->invisibleRootItem(), *shallowCopy_);
	}
}

void DataViewWidget::setPerspective(hmdbCommunication::IDataSourcePerspective * perspective)
{
	perspective_ = perspective;
	delPerspective = false;
	rebuildPerspective();
}

void DataViewWidget::setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy)
{
	shallowCopy_ = shallowCopy;
	rebuildPerspective();
}

void DataViewWidget::setShallowCopyContext(hmdbCommunication::IHMDBShallowCopyContext * shallowCopyContext)
{
	if (shallowCopyContext == nullptr){
		shallowCopyContext_ = shallowCopyContext;
		setRemoteOperationsEnabled(false);
	}
	else if (shallowCopyContext->shallowCopyDataContext() == shallowCopyContext_->shallowCopyDataContext()){
		shallowCopyContext_ = shallowCopyContext;
		setRemoteOperationsEnabled(true);
	}	
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

void DataViewWidget::synchronize(hmdbCommunication::IHMDBRemoteContext::SynchronizeOperationPtr sOp,
	utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger)
{
	shallowCopyContext_->shallowCopyRemoteContext()->synchronize(sOp);
	onRefreshStatus();
	setRemoteOperationsEnabled(true);
	operation_.reset();
}

void DataViewWidget::onSynchronize()
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

}

void DataViewWidget::setLocalOperationsEnabled(const bool enable)
{

}