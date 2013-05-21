#include "PythonPCH.h"
#include "PythonLogic.h"
#include <QtGui/QMessageBox>
#include <boost/python.hpp>


using namespace boost::python;

PythonLogic::PythonLogic()
{
    Py_Initialize();

    mainModule = import("__main__");
    mainNamespace = mainModule.attr("__dict__");
}

PythonLogic::~PythonLogic()
{
    Py_Finalize();
}

struct World
    {
        void set(std::string msg) { this->msg = msg; }
        std::string greet() { return msg; }
        std::string msg;
    };

BOOST_PYTHON_MODULE(plugin_python)
{
    class_<World>("World")
        .def("greet", &World::greet)
        .def("set", &World::set)
        ;
}

void PythonLogic::run( const std::string& script)
{
    str code(script.c_str());
    try {
        exec(code, mainNamespace);
        try {
            int result = extract<int>(mainNamespace["result"]);
            QMessageBox::information(nullptr, "Result", QString("Result : %1").arg(result));
        } catch (...) {

        }
        
    } catch(std::exception& e) {
        int aaa = 1;
    } catch (...) {
        PyErr_Print();
        PyErr_Clear();
    }
}

boost::python::object& PythonLogic::getDict()
{
    return mainNamespace;
}


