#include "PythonPCH.h"
#include "MdeBridge.h"
#include "corelib/HierarchyHelper.h"
#include "corelib/IHierarchyItem.h"
#include "corelib/IDataManager.h"
#include "plugins/hmdbCommunication/TreeItemHelper.h"
#include "corelib/HierarchyDataItem.h"
#include "corelib/Variant.h"
#include "corelib/IDataHierarchyManager.h"
#include "objectwrapperlib/ObjectWrapper.h"

namespace py = boost::python;


void python::MdeBridge::setManagers(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IDataManager * memoryDataManager, 
									core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager,	core::IDataHierarchyManager * hierarchyDataManager)
{
	this->sourceManager = sourceManager;
	this->visualizerManager = visualizerManager;
	this->memoryDataManager = memoryDataManager;
	this->streamDataManager = streamDataManager;
	this->fileDataManager = fileDataManager;
}

python::PythonDataChannel python::MdeBridge::getVectorChannel(const std::string& sessionDesc, const std::string& dataDesc)
{
	auto transaction = memoryDataManager->transaction();
	core::ConstVariantsList objects;
	transaction->getObjects(objects, typeid(c3dlib::VectorChannel), false);

	c3dlib::VectorChannelConstPtr channel;
	for (auto& var : objects) {
		std::string source; var->getMetadata("core/source", source);
		std::string name; var->getMetadata("core/name", name);
		if (source.find(sessionDesc) != std::string::npos && name.find(dataDesc) != std::string::npos) {
			channel = var->get();
			break;
		}
	}
	
	if (channel) {
		return PythonDataChannel::convert(channel);
	}
	else {
		throw std::runtime_error("No data");
	}
}

void python::MdeBridge::addVectorChannel(const PythonDataChannel& channel)
{
	if (channel.getFrequency() <= 0.0f) {
		throw std::runtime_error("Invalid channel frequency");
	} else if (channel.getData().empty()) {
		throw std::runtime_error("Channel was not initialized with data");
	}
	c3dlib::VectorChannelPtr c = PythonDataChannel::convert(channel);

	utils::ObjectWrapperPtr wrp = utils::ObjectWrapper::create<c3dlib::VectorChannel>();
	wrp->set(c);
	core::VariantPtr wrapper = core::Variant::create(wrp);
	core::HierarchyHelperPtr helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(wrapper));
	std::string name = c->getName();
	if (name.empty()) {
		name = wrapper->data()->getClassName();
	}
	core::IHierarchyItemPtr dataItem = core::HierarchyItemPtr(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(name), QString(), helper));
	auto hierarchyTransaction = hierarchyDataManager->transaction();
	hierarchyTransaction->addRoot(dataItem);
}

python::DataList python::MdeBridge::listLoadedVectors()
{
	python::DataList dataList;
	auto transaction = memoryDataManager->transaction();
	core::ConstVariantsList objects;
	transaction->getObjects(objects, typeid(c3dlib::VectorChannel), false);
	std::string source;
	std::string name;
	for (auto& var : objects) {
		var->getMetadata("core/source", source);
		var->getMetadata("core/name", name);
		dataList.push_back(std::make_pair(source, name));

	}
	return dataList;
}

python::PythonDataChannel python::MdeBridge::createVectorChannel()
{
	return PythonDataChannel();
}
