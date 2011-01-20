#include "CorePCH.h"
#include "OsgNotifyHandlerLog4cxx.h"

#ifdef CORELIB_ENABLE_LOGGING

#include <boost/algorithm/string.hpp>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! \param logger Faktyczny logger.
OsgNotifyHandlerLog4cxx::OsgNotifyHandlerLog4cxx(log4cxx::LoggerPtr logger) : 
logger(logger)
{}

//! \param severity
//! \param message
void OsgNotifyHandlerLog4cxx::notify(osg::NotifySeverity severity, const char *message)
{
    // z wiadomoœci usuwamy zbêdne znaki (osg zawsze na koniec daje now¹ liniê)
    std::string msg = message;
    boost::trim(msg);
    switch ( severity ) {
        case osg::ALWAYS:
            LOG4CXX_LOG(logger, log4cxx::Level::getAll(), msg)
                break;
        case osg::FATAL:
            LOG4CXX_ERROR(logger, msg);
            break;
        case osg::WARN:
            LOG4CXX_WARN(logger, msg);
            break;
        case osg::NOTICE:
        case osg::INFO:
            LOG4CXX_INFO(logger, msg);
            break;
        case osg::DEBUG_INFO:
        case osg::DEBUG_FP:
            LOG4CXX_DEBUG(logger, msg);
            break;
        default:
            UTILS_ASSERT(false, "Nieznany poziom osg. Wiadomoœæ: %s", message);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif