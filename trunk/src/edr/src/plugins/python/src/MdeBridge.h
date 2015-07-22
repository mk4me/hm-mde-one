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

	//! Klasa ��czy MDE z Pythonem. Zapewnia wymian� danych mi�dzy tymi �rodowiskami
	class MdeBridge {
	public:
		virtual ~MdeBridge() {}
		
	public:
		//! Metoda inicjalizuj�ca, dostarcza potrzebnych manager�w, powinna zosta� wywo�ana przed pozosta�ymi metodami
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
