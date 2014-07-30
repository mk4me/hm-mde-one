#include "CommunicationPCH.h"
#include "DataSourcePerspectiveManager.h"
#include <utils/Debug.h>

using namespace hmdbCommunication;

DataSourcePerspectiveManager::DataSourcePerspectiveManager() : currentPerspective_(-1)
{

}

DataSourcePerspectiveManager::~DataSourcePerspectiveManager()
{

}

int DataSourcePerspectiveManager::registerPerspective(IDataSourcePerspective * perspective)
{
	for(auto it = perspectives.begin(); it != perspectives.end(); ++it){
		if((*it)->perspective().get() == perspective){
			throw std::runtime_error("Perspective already registered");
		}
	}

	int ret = perspectives.size();

	perspectives.push_back(InnerDataSourcePerspectivePtr(new DataSourcePerspective(DataSourcePerspectivePtr(perspective))));

	return ret;
}

int DataSourcePerspectiveManager::perspectivesCount() const
{
	return perspectives.size();
}

void DataSourcePerspectiveManager::setCurrentPerspective(int idx)
{
	UTILS_ASSERT(idx > -1 && idx < perspectivesCount(), "Blędny index perspektywy");
	if(currentPerspective_ != -1){
		perspectives[currentPerspective_]->widget()->setVisible(false);
	}

	currentPerspective_ = idx;

	if(currentPerspective_ != -1){
		perspectives[currentPerspective_]->widget()->setVisible(true);
	}
}

DataSourcePerspectivePtr DataSourcePerspectiveManager::currentPerspective() const
{
	if(currentPerspective_ == -1){
		return DataSourcePerspectivePtr();
	}
	
	return perspectives[currentPerspective_]->perspective();
}

int DataSourcePerspectiveManager::currentPerspectiveIndex() const
{
	return currentPerspective_;
}

void DataSourcePerspectiveManager::invalidatePerspectives()
{
	for(auto it = perspectives.begin(); it != perspectives.end(); ++it){
		(*it)->invalidate();
	}
}

void DataSourcePerspectiveManager::rebuildCurrentPerspective(const ShallowCopy & shallowCopy)
{
	InnerDataSourcePerspectivePtr perspective = InnerDataSourcePerspectivePtr();
	if(currentPerspective_ > -1){
		perspective = perspectives[currentPerspective_];
	}

	if(perspective != nullptr && perspective->invalid() == true ){
		perspective->clearPerspective();
		perspective->rebuildPerspective(shallowCopy);
	}
}

QTreeWidget * DataSourcePerspectiveManager::perspectiveWidget(int idx)
{
	if(idx > -1){
		return perspectives[idx]->widget();
	}

	return nullptr;
}

//! \return Drzewo danych perspektywy
QTreeWidget * DataSourcePerspectiveManager::currentPerspectiveWidget()
{
	return perspectiveWidget(currentPerspective_);
}
