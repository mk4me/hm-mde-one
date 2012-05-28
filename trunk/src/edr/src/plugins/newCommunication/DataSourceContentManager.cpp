#include "CommunicationPCH.h"
#include "DataSourceContentManager.h"
#include "DataSourceStatusManager.h"

using namespace communication;

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

void DataSourceContentManager::refreshCurrentContent(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy, communication::IDataSourcePerspective * perspective,
	const communication::IDataSourceStatusManager * localStatusManager, const communication::IDataSourceStatusManager * globalStatusManager, bool perspectiveFirst)
{
	int topLevelItemsCount = treeWidget->topLevelItemCount();
	for(int i = 0; i < topLevelItemsCount; ++i){
		refrshItemContent(treeWidget->topLevelItem(i), shallowCopy, perspective, localStatusManager, globalStatusManager, perspectiveFirst);
	}
}

DataStatus DataSourceContentManager::refrshItemContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy, communication::IDataSourcePerspective * perspective,
	const communication::IDataSourceStatusManager * localStatusManager, const communication::IDataSourceStatusManager * globalStatusManager,
	bool perspectiveFirst)
{
	DataStatus status;
	int statusColumn = -1;
	QStringList headers;

	//zawartoœæ wg regu³ pierwszeñstwa i stanu wype³nienia
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
	auto content = dynamic_cast<const IContent*>(item);
	if(content != nullptr){

		found = true;
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){

		case DisorderContent:
			{
				auto disorder = dynamic_cast<DisorderItem*>(item);
				status = globalStatusManager->disorderStatus(disorder->value()->disorderID);
			}
			break;
		case PatientContent:
			{
				auto patient = dynamic_cast<PatientItem*>(item);
				status = globalStatusManager->patientStatus(patient->value()->patientID);
			}
			break;
		case SubjectContent:
			{
				auto subject = dynamic_cast<SubjectItem*>(item);
				status = globalStatusManager->subjectStatus(subject->value()->performerID);
			}
			break;
		case SessionContent:
			{
				auto session = dynamic_cast<SessionItem*>(item);
				status = globalStatusManager->sessionStatus(session->value()->sessionID);
			}
			break;
		case MotionContent:
			{
				auto motion = dynamic_cast<MotionItem*>(item);
				status = globalStatusManager->motionStatus(motion->value()->trialID);
			}
			break;
		case FileContent:
			{
				auto file = dynamic_cast<FileItem*>(item);
				status = globalStatusManager->fileStatus(file->value()->fileID);
			}
			break;
		case DisordersGroupContent:
			{
				auto disordersGroup = dynamic_cast<DisordersGroupItem*>(item);
				for(auto it = disordersGroup->value().begin(); it != disordersGroup->value().end(); ++it){
					status |= globalStatusManager->disorderStatus((*it)->disorderID);
				}
			}
			break;
		case PatientsGroupContent:
			{
				auto patientsGroup = dynamic_cast<PatientsGroupItem*>(item);
				for(auto it = patientsGroup->value().begin(); it != patientsGroup->value().end(); ++it){
					status |= globalStatusManager->patientStatus((*it)->patientID);
				}
			}
			break;
		case SubjectsGroupContent:
			{
				auto subjectsGroup = dynamic_cast<SubjectsGroupItem*>(item);
				for(auto it = subjectsGroup->value().begin(); it != subjectsGroup->value().end(); ++it){
					status |= globalStatusManager->subjectStatus((*it)->performerID);
				}
			}
			break;
		case SessionsGroupContent:
			{
				auto sessionsGroup = dynamic_cast<SessionsGroupItem*>(item);
				for(auto it = sessionsGroup->value().begin(); it != sessionsGroup->value().end(); ++it){
					status |= globalStatusManager->sessionStatus((*it)->sessionID);
				}
			}
			break;
		case MotionsGroupContent:
			{
				auto motionsGroup = dynamic_cast<MotionsGroupItem*>(item);
				for(auto it = motionsGroup->value().begin(); it != motionsGroup->value().end(); ++it){
					status |= globalStatusManager->motionStatus((*it)->trialID);
				}
			}
			break;
		case FilesGroupContent:
			{
				auto filesGroup = dynamic_cast<FilesGroupItem*>(item);
				for(auto it = filesGroup->value().begin(); it != filesGroup->value().end(); ++it){
					status |= globalStatusManager->fileStatus((*it)->fileID);
				}
			}
			break;
		case CustomContent:
			found = false;
			break;
		}
	}	

	DataStatus tmpStatus;		
	//odœwie¿am zawartoœc dzieci jesli nie znamy typu obiektu
	int childrenCount = item->childCount();
	for(int i = 0; i < childrenCount; ++i){
		tmpStatus |= refrshItemContent(item->child(i), shallowCopy, perspective, localStatusManager, globalStatusManager, perspectiveFirst);
	}

	if(found == false){
		status = tmpStatus;
	}

	//odœwie¿am status
	if(statusColumn == -1 || statusColumn >= headers.size()){
		statusColumn = 0;
	}

	item->setIcon(statusColumn, DataSourceStatusManager::statusIcon(status));

	//TODO
	//stan œci¹gania dla elementów

	return status;
}