/********************************************************************
    created:  2013/01/17
    created:  17:1:2013   16:47
    filename: StreamDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___STREAMDATAMANAGER_H__
#define HEADER_GUARD___STREAMDATAMANAGER_H__

#include <core/IStreamDataManager.h>

namespace core {

	class StreamDataManager : public IStreamDataManager, public IStreamManagerReader
	{

	};

}

#endif	//	HEADER_GUARD___STREAMDATAMANAGER_H__
