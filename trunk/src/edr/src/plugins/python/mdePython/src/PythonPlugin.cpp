#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>
#include "plugins/newVdf/IDataFlowProvider.h"
#include "MdeBridge.h"



namespace py = boost::python;

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
		.def("close", &python::MdeBridge::close)
		.def("getHelpers", &python::MdeBridge::getHelpers)
		.def("log", &python::MdeBridge::log);


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