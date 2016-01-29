/********************************************************************
	created:	2014/07/31
	created:	11:27
	filename: 	MdeBridge.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__MdeBridge_H__
#define HEADER_GUARD_PYTHON__MdeBridge_H__

#include "PythonDataChannel.h"
#include <corelib/ISourceManager.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/IDataManager.h>
#include <corelib/IStreamDataManager.h>
#include <corelib/IFileDataManager.h>
#include "plugins/python/python/PythonPluginUtils.h"
#include "corelib/HierarchyHelper.h"

namespace python {

	typedef std::pair<std::string, std::string> StringPair;
	typedef std::vector<StringPair> DataList;


	struct Helper
	{
		Helper() {}
		Helper(core::HierarchyHelperConstWeakPtr helper, core::IVisualizerManager* vm) : helper(helper), manager(vm) {}
		std::string name() { return "nazwa"; }
		void createVisualizer();

		bool operator==(const Helper& h)
		{
			return this->helper.lock() == h.helper.lock();
		}

		core::IVisualizerManager* manager;
		core::HierarchyHelperConstWeakPtr helper;
	};

	//! Klasa ³¹czy MDE z Pythonem. Zapewnia wymianê danych miêdzy tymi œrodowiskami
	class MdeBridge:  public PythonPluginUtils
	{
	public:
		virtual ~MdeBridge() {}
		
	public:
		//! 
		//! \param sessionDesc
		//! \param dataDesc
		//! \return 
		PythonDataChannel getVectorChannel(const std::string& sessionDesc, const std::string& dataDesc);
		//! 
		//! \param channel
		//! \return 
		void addVectorChannel(const PythonDataChannel& channel);
		//! 
		//! \return 
		DataList listLoadedVectors();

		PythonDataChannel createVectorChannel();

		std::vector<Helper> getHelpers();

		void addFile(const std::string& file);

	};
	DEFINE_SMART_POINTERS(MdeBridge);
}

#endif // HEADER_GUARD 
