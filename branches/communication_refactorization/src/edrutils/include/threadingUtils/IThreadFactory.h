/********************************************************************
	created:  2012/12/11
	created:  11:12:2012   15:00
	filename: IThreadFactory.h
	author:   Mateusz Janiak

	purpose:  Interfejs fabryki w¹tków - Qt, OpenThreads, natywna
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__ITHREADFACTORY_H__
#define HEADER_GUARD_THREADINGUTILS__ITHREADFACTORY_H__

#include <utils/SmartPtr.h>
#include <threadingUtils/IThread.h>

namespace threadingUtils {
	class IThreadFactory
	{
	public:
		//! Wirtualny destruktor
		virtual ~IThreadFactory() {}
		//! \return WskaŸnik do nowoutworoznego w¹tku
		virtual IThreadPtr createThread() = 0;
	};

	DEFINE_SMART_POINTERS(IThreadFactory)
}

#endif	//	HEADER_GUARD_THREADINGUTILS__ITHREADFACTORY_H__
