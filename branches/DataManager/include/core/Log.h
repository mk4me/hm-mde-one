/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   10:28
	filename: Log.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_LOG_H__
#define __HEADER_GUARD__CORE_LOG_H__

// tymczasowe rozwi¹zanie: u¿ywamy strumieni OSG
#include <core/Config.h>
#include <osg/Notify>

#define LOG_DEBUG(x) OSG_DEBUG<<x<<std::endl
#define LOG_ERROR(x) OSG_FATAL<<x<<std::endl
#define LOG_INFO(x) OSG_INFO<<x<<std::endl
#define LOG_WARNING(x) OSG_WARN<<x<<std::endl

#endif  // __HEADER_GUARD__CORE_LOG_H__