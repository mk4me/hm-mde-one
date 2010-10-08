/********************************************************************
	created:  2010/10/07
	created:  7:10:2010   11:45
	filename: ITimelineClient.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__ITIMELINECLIENT_H__
#define __HEADER_GUARD__ITIMELINECLIENT_H__

#include <vector>
#include <boost/weak_ptr.hpp>
#include "Stream.h"
#include "Model.h"

/**
 *	Interfejs klienta timeline'a.
 */
class ITimelineClient
{
public:
    //! Pusty polimorficzny destruktor.
    virtual ~ITimelineClient() {}
    //! \return Lista strumieni.
    virtual timeline::Model::Streams getStreams() = 0;
};


#endif  // __HEADER_GUARD__ITIMELINECLIENT_H__