#include "CommunicationPCH.h"
#include "DataSourceContentManager.h"
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>

#include <QtGui/QPainter>

DataSourceContentManager::DataSourceContentManager() : currentContent_(-1)
{

}

DataSourceContentManager::~DataSourceContentManager()
{

}

int DataSourceContentManager::registerContent(IDataSourceContent * content)
{
	auto it = contents.begin();

	while(it != contents.end()){
		if((*it).get() == content){
			break;
		}
		++it;
	}

	if(it != contents.end()){
		throw std::runtime_error("Content already registered");
	}

	int ret = contents.size();
	contents.push_back(DataSourceContentPtr(content));

	return ret;
}

int DataSourceContentManager::currentContentIndex() const
{
	return currentContent_;
}

DataSourceContentPtr DataSourceContentManager::currentContent() const
{
	if(currentContent_ > -1){
		return contents[currentContent_];
	}

	return DataSourceContentPtr();
}

void DataSourceContentManager::setCurrentContent(int idx)
{
	UTILS_ASSERT(idx >= 0 && idx < static_cast<int>(contents.size()));

	currentContent_ = idx;
}

bool DataSourceContentManager::empty() const
{
	return contents.empty();
}

int DataSourceContentManager::size() const
{
	return contents.size();
}

void DataSourceContentManager::refreshCurrentContent(QTreeWidget * treeWidget, const ShallowCopy & shallowCopy, IDataSourcePerspective * perspective,
	const IHMDBStatusManager * statusManager, bool perspectiveFirst)
{
	int topLevelItemsCount = treeWidget->topLevelItemCount();
	for(int i = 0; i < topLevelItemsCount; ++i){
		refrshItemContent(treeWidget->topLevelItem(i), shallowCopy, perspective, statusManager, perspectiveFirst);
	}
}

void getStatus(const TreeWidgetContentItem * root, const ContentType cT,
	const DataType dT, const IHMDBStatusManager * statusManager, DataStatus & status)
{
	for (int i = 0; i < root->childCount(); ++i){
		auto cContent = dynamic_cast<TreeWidgetContentItem*>(root->child(i));
		if (cContent != nullptr && cContent->contentType() == cT){
			status |= statusManager->dataStatus(dT, cContent->id());
		}
	}
}

DataStatus DataSourceContentManager::refrshItemContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy, IDataSourcePerspective * perspective,
	const IHMDBStatusManager * statusManager, bool perspectiveFirst)
{
	DataStatus status;
	int statusColumn = -1;
	QStringList headers;

	//zawartość wg reguł pierwszeństwa i stanu wypełnienia
	if(perspectiveFirst == true){
		if(perspective->fillContent(item, shallowCopy) == false){
			contents[currentContent_]->fillContent(item, shallowCopy);
			statusColumn = contents[currentContent_]->statusField(item);
			contents[currentContent_]->headers(item, headers);
		}else{
			statusColumn = perspective->statusField(item);
			perspective->headers(item, headers);
		}
	}else if(contents[currentContent_]->fillContent(item, shallowCopy) == false){
		perspective->fillContent(item, shallowCopy);
		statusColumn = perspective->statusField(item);
		perspective->headers(item, headers);
	}else{
		statusColumn = contents[currentContent_]->statusField(item);
		contents[currentContent_]->headers(item, headers);
	}

	bool found = false;
	auto content = dynamic_cast<const TreeWidgetContentItem*>(item);
	if(content != nullptr){

		found = true;
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){

		case PatientContent:
			{
				status = statusManager->dataStatus(PatientType, content->id());
			}
			break;
		case SubjectContent:
			{
				status = statusManager->dataStatus(SubjectType, content->id());
			}
			break;
		case SessionContent:
			{
				status = statusManager->dataStatus(SessionType, content->id());
			}
			break;
		case MotionContent:
			{
				status = statusManager->dataStatus(MotionType, content->id());
			}
			break;
		case FileContent:
			{
				status = statusManager->dataStatus(FileType, content->id());
			}
			break;

		case PatientsGroupContent:			
			getStatus(content, PatientContent, PatientType, statusManager, status);
			break;

		case SubjectsGroupContent:
			getStatus(content, SubjectContent, SubjectType, statusManager, status);
			break;

		case SessionsGroupContent:
			getStatus(content, SessionContent, SessionType, statusManager, status);
			break;

		case MotionsGroupContent:
			getStatus(content, MotionContent, MotionType, statusManager, status);				
			break;
		case FilesGroupContent:
			getStatus(content, FileContent, FileType, statusManager, status);				
			break;
		case CustomContent:
			found = false;
			break;
		}
	}

	DataStatus tmpStatus;
	//odświeżam zawartośc dzieci jeśli nie znamy typu obiektu
	int childrenCount = item->childCount();
	for(int i = 0; i < childrenCount; ++i){
		tmpStatus |= refrshItemContent(item->child(i), shallowCopy, perspective, statusManager, perspectiveFirst);
	}

	if(found == false){
		status = tmpStatus;
	}

	//odświeżam status
	if(statusColumn == -1 || statusColumn >= headers.size()){
		statusColumn = 0;
	}

	item->setIcon(statusColumn, statusIcon(status));

	//TODO
	//stan ściągania dla elementów

	return status;
}
