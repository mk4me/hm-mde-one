#include "MdeLogic.h"
#include <boost/python.hpp>
#include "corelib/ISourceManager.h"
#include "corelib/PluginCommon.h"
#include "plugins/subject/ISubjectService.h"
#include "corelib/IServiceManager.h"
#include "plugins/python/PythonPluginUtils.h"

#include "plugins/fileSource/IFileSource.h"
#include "corelib/HierarchyHelper.h"
#include "corelib/IDataManager.h"
#include "corelib/IDataHierarchyManager.h"
#include "corelib/IHierarchyItem.h"
#include "corelib/IHierarchyDataItem.h"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"
#include <QtWidgets/QWidget>
#include "coreui/CoreVisualizerWidget.h"
#include "coreui/CoreDockWidget.h"

using namespace boost::python;


struct Helper
{
	Helper() {}
	Helper(core::HierarchyHelperConstWeakPtr helper, core::IVisualizerManager* vm) : helper(helper), manager(vm) {}
	std::string name() { return "nazwa"; }
	void createVisualizer() 
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
		} else {
			throw std::runtime_error("Helper no longer exists");
		}
	}

	bool operator==(const Helper& h)
	{
		return this->helper.lock() == h.helper.lock();
	}

	core::IVisualizerManager* manager;
	core::HierarchyHelperConstWeakPtr helper;
};

struct SimpleWorld : private python::PythonPluginUtils
    {
		void addFile(std::string file)
		{
			auto source = core::querySource<fileSource::IFileSource>(getSourceManager());
			if (source != nullptr) {
				source->addFile(file);
			} else {
				throw std::runtime_error("Unable to find source");
			}
		}

		static void gather(const core::IHierarchyItemConstPtr& item, std::vector<Helper>& ret, core::IVisualizerManager* manager)
		{
			core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
			if (dataItem) {
				std::list<core::HierarchyHelperPtr> helpers = dataItem->getHelpers();
				for (auto& h : helpers) {
					ret.push_back(Helper(h, manager));
				}
			}
			for (int count = item->getNumChildren() - 1; count >= 0; --count) {

				gather(item->getChild(count), ret, manager);
			}
		};

		std::vector<Helper> getHelpers()
		{
			std::vector<Helper> ret;
			core::IDataHierarchyManager* hierarchyManager = getHierarchyManager();
			if (hierarchyManager != nullptr) {
				auto t = hierarchyManager->transaction();
				std::for_each(t->hierarchyBegin(), t->hierarchyEnd(), 
							  [&](const core::IHierarchyItemConstPtr& item) { gather(item, ret, getVisualizerManager()); });
			} else {
				throw std::runtime_error("Unable to find source");
			}

			return ret;
		}
		
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
    class_<SimpleWorld>("SimpleWorld")
		.def("greet", &SimpleWorld::greet)
		.def("set", &SimpleWorld::set)
		.def("setN", &SimpleWorld::setN)
		.def("getN", &SimpleWorld::getN)
		.def("addFile", &SimpleWorld::addFile)
		.def("getHelpers", &SimpleWorld::getHelpers)
        ;
	class_<Helper>("Helper")
		.def("name", &Helper::name)
		.def("createVisualizer", &Helper::createVisualizer)
		;


	class_<std::vector<Helper>>("helpers_vector")
		.def(vector_indexing_suite<std::vector<Helper>>());
}
