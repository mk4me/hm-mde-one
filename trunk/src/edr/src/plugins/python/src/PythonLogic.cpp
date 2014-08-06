#include "PythonPCH.h"
#include "PythonLogic.h"
#include <QtWidgets/QMessageBox>
#include <boost/python.hpp>
#include <osg/Vec3>
#include "MdeBridge.h"


using namespace boost::python;

python::PythonStdIoRedirect::ContainerType python::PythonStdIoRedirect::m_outputs;

python::PythonLogic::PythonLogic(MdeBridgeConstPtr bridge) : 
	bridge(bridge)
{
    Py_Initialize();

    mainModule = import("__main__");
	mainNamespace = createGlobal();
	mainNamespace["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
		.def("write", &PythonStdIoRedirect::Write);
	boost::python::import("sys").attr("stderr") = python_stdio_redirector;
	boost::python::import("sys").attr("stdout") = python_stdio_redirector;
	
	

	boost::python::class_<python::MdeBridge>("MdeBridge")
		.def("createVectorChannel", &python::MdeBridge::createVectorChannel)
		.def("getVectorChannel", &python::MdeBridge::getVectorChannel)
		.def("addVectorChannel", &python::MdeBridge::addVectorChannel)
		.def("listLoadedVectors", &python::MdeBridge::listLoadedVectors);

	mainNamespace["mde"] = ptr(bridge.get());
}

python::PythonLogic::~PythonLogic()
{
    Py_Finalize();
}

struct World
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

BOOST_PYTHON_MODULE(plugin_python)
{
    class_<World>("World")
        .def("greet", &World::greet)
        .def("set", &World::set)
		.def("setN", &World::setN)
		.def("getN", &World::getN)
        ;

	using namespace python;
	typedef osg::Vec3::value_type vval;
	typedef float (osg::Vec3::*get_const)() const;
	typedef void (osg::Vec3::*set3)(vval x, vval y, vval z);

	class_<osg::Vec3>("Vec3", init<vval, vval, vval>())
		.def("x", get_const(&osg::Vec3::x))
		.def("y", get_const(&osg::Vec3::y))
		.def("z", get_const(&osg::Vec3::z))
		.def("set", set3(&osg::Vec3::set))
		;

	class_<StringPair>("StringPair")
		.def_readwrite("first", &StringPair::first)
		.def_readwrite("second", &StringPair::second);

	class_<DataList>("DataList")
		.def(vector_indexing_suite<DataList>());

	class_<PythonDataChannel>("PythonDataChannel")
		.def("getData", &PythonDataChannel::getData)
		.def("getFrequency", &PythonDataChannel::getFrequency)
		.def("getName", &PythonDataChannel::getName)
		.def("setData", &PythonDataChannel::setData)
		.def("setFrequency", &PythonDataChannel::setFrequency)
		.def("setName", &PythonDataChannel::setName)
		;


	class_<std::vector<osg::Vec3>>("v3vector")
		.def(vector_indexing_suite<std::vector<osg::Vec3>>());
}

std::string python::PythonLogic::run(const std::string& script)
{
	str code(script.c_str());
    try {
		//object cpp_module((handle<>(PyImport_ImportModule("WorldMod"))));
		//mainNamespace["WorldMod"] = cpp_module;
		//
		//int testInt = 100;
		//mainNamespace["input1"] = testInt;
		//PyRun_SimpleString("import cStringIO");
		//PyRun_SimpleString("import sys");
		//PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");
		
        exec(code, mainNamespace);
       /* try {
            float result = extract<float>(mainNamespace["result"]);
            QMessageBox::information(nullptr, "Result", QString("Result : %1").arg(result));
        } catch (...) {

        }*/
        
	}
	catch (std::exception& e) {
		int aaa = 1;
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
		//boost::python::object sys(
		//	boost::python::handle<>(PyImport_ImportModule("sys"))
		//	);
		//boost::python::object err = sys.attr("stderr");
		//std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());
		//QMessageBox::critical(nullptr, "Error", QString::fromStdString(err_text));
	} catch (...) {
        PyErr_Print();
        //PyErr_Clear();
    }

	return python_stdio_redirector.GetOutput();
}

boost::python::object& python::PythonLogic::getDict()
{
    return mainNamespace;
}

std::pair<std::string, python::PythonLogic::object> python::PythonLogic::runStatement(const std::string& script, object global /*= object()*/, object local /*= object()*/)
{
	str code(script.c_str());

	//global["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
	//	.def("write", &PythonStdIoRedirect::Write);
	object ret;
	try {
		//ret = exec_statement(code, global, local);
		PyRun_SimpleString(script.c_str());
	} catch (...) {
		PyErr_Print();
	}
	return std::make_pair(python_stdio_redirector.GetOutput(), ret);
}

python::PythonLogic::object python::PythonLogic::createGlobal()
{
	boost::python::object global = mainModule.attr("__dict__");
	//global["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
	//	.def("write", &PythonStdIoRedirect::Write);
	return global;
}



void python::PythonStdIoRedirect::Write(std::string const& str)
{
	if (m_outputs.capacity() < 100)
		m_outputs.resize(100);
	m_outputs.push_back(str);
}

std::string python::PythonStdIoRedirect::GetOutput()
{
	std::string ret;
	std::stringstream ss;
	for (boost::circular_buffer<std::string>::const_iterator it = m_outputs.begin();
		it != m_outputs.end();
		it++)
	{
		ss << *it;
	}
	m_outputs.clear();
	ret = ss.str();
	return ret;
}
