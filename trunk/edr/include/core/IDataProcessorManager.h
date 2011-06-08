/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   9:02
    filename: IDataProcessorManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAPROCESSORMANAGER_H__
#define HEADER_GUARD___IDATAPROCESSORMANAGER_H__

#include <core/IDataProcessor.h>
#include <map>
#include <vector>
#include <utils/Debug.h>

namespace core{

    class IDataProcessorManager
    {
    public:
        virtual ~IDataProcessorManager(void) {};

    };

    typedef shared_ptr<IDataProcessorManager> IDataProcessorManagerPtr;
    typedef shared_ptr<const IDataProcessorManager> IDataProcessorManagerConstPtr;
    typedef weak_ptr<IDataProcessorManager> IDataProcessorManagerWeakPtr;
    typedef weak_ptr<const IDataProcessorManager> IDataProcessorManagerWeakConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif //HEADER_GUARD___ISERVICEMANAGER_H__
