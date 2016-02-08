#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>
#include "CommunicationPython.h"
#include "boost/python/class.hpp"
#include "boost/python/scope.hpp"

namespace py = boost::python;

MDE_PYTHON_MODULE(plugin_communicationPython)
{
	{
		//QString name;										//! Nazwa konfiguracji (unikalna!!)
		//StorageConfiguration storageConfiguration;			//! Konfiguracja storage
		//RemoteConfiguration motionServicesConfiguration;	//! Konfiguracja us³ug ruchu
		//RemoteConfiguration motionDataConfiguration;		//! Konfiguracja danych ruchu
		//RemoteConfiguration medicalServicesConfiguration;	//! Konfiguracja us³ug medycznych
		//RemoteConfiguration medicalDataConfiguration;		//! Konfiguracja danych medycznych
		typedef python::CommunicationPython::ContextConf Conf;
		
		//py::scope in_human =                     // define example.Human and set
		//	py::class_<Human>("Human");            // scope to example.Human

		//python::class_<Human::emotion>("Emotion")    // define example.Human.Emotion
		//	.add_property("joy", &Human::emotion::joy)
		//	;
		py::scope conf = py::class_<Conf>("ContextConfiguration")
			.def_readwrite("name", &Conf::name);
		//typedef decltype(&Conf::storageConfiguration) SC;
		typedef hmdbCommunication::IHMDBSourceViewManager::StorageConfiguration SC;
		py::class_<SC>("storageConfiguration")
			.def_readwrite("path", &SC::path)
			.def_readwrite("password", &SC::password);
	}
	py::class_<python::CommunicationPython>("CommunicationPython")
		.def("login", &python::CommunicationPython::login)
		.def("loadConfig", &python::CommunicationPython::loadConf);


}