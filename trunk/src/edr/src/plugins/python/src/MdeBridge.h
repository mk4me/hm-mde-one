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

namespace python {

	typedef std::pair<std::string, std::string> StringPair;
	typedef std::vector<StringPair> DataList;

	//! Klasa ³¹czy MDE z Pythonem. Zapewnia wymianê danych miêdzy tymi œrodowiskami
	class MdeBridge {
	public:
		virtual ~MdeBridge() {}
		
	public:
		//! Metoda inicjalizuj¹ca, dostarcza potrzebnych managerów, powinna zostaæ wywo³ana przed pozosta³ymi metodami
		//! \param sourceManager
		//! \param visualizerManager
		//! \param memoryDataManager
		//! \param streamDataManager
		//! \param fileDataManager
		void setManagers(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager,
						 core::IDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager,
						 core::IDataHierarchyManager * hierarchyDataManager);
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

	private:
		core::ISourceManager * sourceManager;
		core::IVisualizerManager * visualizerManager;
		core::IDataManager * memoryDataManager;
		core::IStreamDataManager * streamDataManager;
		core::IFileDataManager * fileDataManager;
		core::IDataHierarchyManager * hierarchyDataManager;
	};
	DEFINE_SMART_POINTERS(MdeBridge);
}

#endif // HEADER_GUARD 
