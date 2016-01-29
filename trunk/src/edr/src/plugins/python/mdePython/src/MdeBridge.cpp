#include "MdeBridge.h"
#include "corelib/IHierarchyItem.h"
#include "corelib/IDataManager.h"
#include "plugins/hmdbCommunication/TreeItemHelper.h"
#include "corelib/HierarchyDataItem.h"
#include "corelib/Variant.h"
#include "corelib/IDataHierarchyManager.h"
#include "objectwrapperlib/ObjectWrapper.h"
#include "coreui/CoreVisualizerWidget.h"
#include "plugins/python/python/PythonPluginUtils.h"
#include "coreui/CoreDockWidget.h"
#include "corelib/ISourceManager.h"
#include "plugins/fileSource/IFileSource.h"
#include "boost/python/class.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"

namespace py = boost::python;




python::PythonDataChannel python::MdeBridge::getVectorChannel(const std::string& sessionDesc, const std::string& dataDesc)
{
	auto transaction = getDataManagerReader()->transaction();
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
	auto hierarchyTransaction = getHierarchyManager()->transaction();
	hierarchyTransaction->addRoot(dataItem);
}

python::DataList python::MdeBridge::listLoadedVectors()
{
	python::DataList dataList;
	auto transaction = getDataManagerReader()->transaction();
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

void python::MdeBridge::addFile(const std::string& file)
{
	auto source = core::querySource<fileSource::IFileSource>(getSourceManager());
	if (source != nullptr) {
		source->addFile(file);
	}
	else {
		throw std::runtime_error("Unable to find FileSource plugin");
	}
}

static void gather(const core::IHierarchyItemConstPtr& item, std::vector<python::Helper>& ret, core::IVisualizerManager* manager)
{
	core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
	if (dataItem) {
		std::list<core::HierarchyHelperPtr> helpers = dataItem->getHelpers();
		for (auto& h : helpers) {
			ret.push_back(python::Helper(h, manager));
		}
	}
	for (int count = item->getNumChildren() - 1; count >= 0; --count) {

		gather(item->getChild(count), ret, manager);
	}
};

std::vector<python::Helper> python::MdeBridge::getHelpers()
{
	std::vector<python::Helper> ret;
	core::IDataHierarchyManager* hierarchyManager = getHierarchyManager();
	if (hierarchyManager != nullptr) {
		auto t = hierarchyManager->transaction();
		std::for_each(t->hierarchyBegin(), t->hierarchyEnd(),
			[&](const core::IHierarchyItemConstPtr& item) { gather(item, ret, getVisualizerManager()); });
	}
	else {
		throw std::runtime_error("Unable to find source");
	}

	return ret;
}


struct SimpleWorld : private python::PythonPluginUtils
{


	void set(std::string msg) { this->msg = msg; }
	std::string greet() { return msg; }
	std::string msg;

	void setN(int n) {
		number = n;
	}
	int getN()
	{
		return number;
	}
	int number;
};


MDE_PYTHON_MODULE(plugin_mdePython)
{
	py::class_<python::Helper>("Helper")
		.def("name", &python::Helper::name)
		.def("createVisualizer", &python::Helper::createVisualizer)
		;


	py::class_<std::vector<python::Helper>>("helpers_vector")
		.def(py::vector_indexing_suite<std::vector<python::Helper>>());

	py::class_<python::MdeBridge>("MdeBridge")
		.def("createVectorChannel", &python::MdeBridge::createVectorChannel)
		.def("getVectorChannel", &python::MdeBridge::getVectorChannel)
		.def("addVectorChannel", &python::MdeBridge::addVectorChannel)
		.def("listLoadedVectors", &python::MdeBridge::listLoadedVectors)
		.def("addFile", &python::MdeBridge::addFile)
		.def("getHelpers", &python::MdeBridge::getHelpers);


	using namespace python;
	typedef osg::Vec3::value_type vval;
	typedef float (osg::Vec3::*get_const)() const;
	typedef void (osg::Vec3::*set3)(vval x, vval y, vval z);

	py::class_<osg::Vec3>("Vec3", py::init<vval, vval, vval>())
		.def("x", get_const(&osg::Vec3::x))
		.def("y", get_const(&osg::Vec3::y))
		.def("z", get_const(&osg::Vec3::z))
		.def("set", set3(&osg::Vec3::set))
		;

	py::class_<python::StringPair>("StringPair")
		.def_readwrite("first", &python::StringPair::first)
		.def_readwrite("second", &python::StringPair::second);

	py::class_<python::DataList>("DataList")
		.def(py::vector_indexing_suite<python::DataList>());

	py::class_<python::PythonDataChannel>("PythonDataChannel")
		.def("getData", &python::PythonDataChannel::getData)
		.def("getFrequency", &python::PythonDataChannel::getFrequency)
		.def("getName", &python::PythonDataChannel::getName)
		.def("setData", &python::PythonDataChannel::setData)
		.def("setFrequency", &python::PythonDataChannel::setFrequency)
		.def("setName", &python::PythonDataChannel::setName)
		;


	py::class_<std::vector<osg::Vec3>>("v3vector")
		.def(py::vector_indexing_suite<std::vector<osg::Vec3>>());
}

void python::Helper::createVisualizer()
{
	auto h = utils::const_pointer_cast<core::HierarchyHelper>(helper.lock());
	if (h) {
		auto visualizer = h->createVisualizer(manager);
		visualizer->getOrCreateWidget();

		auto visWidget = new coreUI::CoreVisualizerWidget(visualizer);
		//visWidget->show();

		auto dockVisWidget = coreUI::CoreDockWidget::embeddWidget(visWidget, "test", Qt::AllDockWidgetAreas, false);

		std::vector<core::Visualizer::Serie*> series;
		h->getSeries(visualizer, "test/t", series);
		dockVisWidget->setMinimumSize((std::max)(280, dockVisWidget->minimumWidth()), (std::max)(280, dockVisWidget->minimumHeight()));
		dockVisWidget->show();
	}
	else {
		throw std::runtime_error("Helper no longer exists");
	}
}
