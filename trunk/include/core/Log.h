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
#include <osg/Notify>

#define LOG_ERROR OSG_FATAL
#define LOG_INFO  OSG_NOTICE

#endif  // __HEADER_GUARD__CORE_LOG_H__