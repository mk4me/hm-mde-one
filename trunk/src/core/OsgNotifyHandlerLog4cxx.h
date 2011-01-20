/********************************************************************
	created:  2011/01/20
	created:  20:1:2011   12:58
	filename: OsgNotifyHandlerLog4cxx.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__OSGNOTIFYHANDLERLOG4CXX_H__
#define __HEADER_GUARD__OSGNOTIFYHANDLERLOG4CXX_H__

#include <osg/Notify>
#include <core/Log.h>

#ifdef CORELIB_ENABLE_LOGGING

#include <log4cxx/logger.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class OsgNotifyHandlerLog4cxx : public osg::NotifyHandler
{
private:
    //! Faktyczny logger.
    log4cxx::LoggerPtr logger;

public:
    //! \param logger Faktyczny logger.
    OsgNotifyHandlerLog4cxx(log4cxx::LoggerPtr logger);
    //! \param severity
    //! \param message
    void notify(osg::NotifySeverity severity, const char *message);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif

#endif  // __HEADER_GUARD__OSGNOTIFYHANDLERLOG4CXX_H__