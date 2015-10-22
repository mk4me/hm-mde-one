/********************************************************************
	created:	2015/07/17
	created:	12:17
	filename: 	PythonPluginUtils.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PythonPluginUtils_H__
#define HEADER_GUARD_PYTHON__PythonPluginUtils_H__

#include <plugins/python/Export.h>

namespace core {
	class IServiceManager;
	class ISourceManager;
}

namespace  python {
	class PLUGIN_PYTHON_EXPORT PythonPluginUtils {
		public:
			virtual ~PythonPluginUtils() {}
		protected:
			core::IServiceManager* getServiceManager();
			core::ISourceManager* getSourceManager();
			core::IDataHierarchyManager* getHierarchyManager();
			core::IVisualizerManager* getVisualizerManager();
	};
	DEFINE_SMART_POINTERS(PythonPluginUtils);
}
#ifdef _DEBUG
	#define MDE_PYTHON_MODULE(name) BOOST_PYTHON_MODULE(name##d)
#else
	#define MDE_PYTHON_MODULE(name) BOOST_PYTHON_MODULE(name)
#endif

#endif // HEADER_GUARD 
