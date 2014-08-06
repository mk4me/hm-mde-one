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
#include <corelib/IMemoryDataManager.h>
#include <corelib/IStreamDataManager.h>
#include <corelib/IFileDataManager.h>

namespace python {

	typedef std::pair<std::string, std::string> StringPair;
	typedef std::vector<StringPair> DataList;

	class MdeBridge {
	public:
		virtual ~MdeBridge() {}
		
	public:
		void setManagers(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager,
			core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager);

		PythonDataChannel getVectorChannel(const std::string& sessionDesc, const std::string& dataDesc);
		void addVectorChannel(const PythonDataChannel& channel);

		DataList listLoadedVectors();

		PythonDataChannel createVectorChannel();

	private:
		core::ISourceManager * sourceManager;
		core::IVisualizerManager * visualizerManager;
		core::IMemoryDataManager * memoryDataManager;
		core::IStreamDataManager * streamDataManager;
		core::IFileDataManager * fileDataManager;
	};
	DEFINE_SMART_POINTERS(MdeBridge);
}

#endif // HEADER_GUARD 
