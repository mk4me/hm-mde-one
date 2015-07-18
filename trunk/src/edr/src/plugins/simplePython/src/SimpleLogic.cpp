#include "SimpleLogic.h"
#include <boost/python.hpp>
#include "corelib/ISourceManager.h"
#include "corelib/PluginCommon.h"
#include "plugins/subject/ISubjectService.h"
#include "corelib/IServiceManager.h"
#include "plugins/python/PythonPluginUtils.h"

#include "plugins/fileSource/IFileSource.h"

using namespace boost::python;



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

BOOST_PYTHON_MODULE(plugin_simplePythond)
{
    class_<SimpleWorld>("SimpleWorld")
		.def("greet", &SimpleWorld::greet)
		.def("set", &SimpleWorld::set)
		.def("setN", &SimpleWorld::setN)
		.def("getN", &SimpleWorld::getN)
		.def("addFile", &SimpleWorld::addFile)
        ;

	
}
