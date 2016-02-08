/********************************************************************
	created:	2015/10/20
	created:	12:49
	filename: 	CommunicationPython.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CommunicationPython__CommunicationPython_H__
#define HEADER_GUARD_CommunicationPython__CommunicationPython_H__

#include <utils/SmartPtr.h>
#include <plugins/python/python/PythonPluginUtils.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>

namespace python {
	class CommunicationPython : private python::PythonPluginUtils
	{
	public:
		typedef hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration ContextConf;
		void login(const ContextConf& conf);
		ContextConf loadConf(const QString& path);
	};
}

#endif
