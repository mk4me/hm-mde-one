/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   9:02
    filename: IDataProcessorManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAPROCESSORMANAGER_H__
#define HEADER_GUARD___IDATAPROCESSORMANAGER_H__

#include <plugins/newVdf/IDataProcessor.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>

namespace vdf{

    class IDataProcessorManager :  public utils::Observable<IDataProcessorManager>
    {
    public:
        //! Lista elementów przetwarzaj¹cych.
        typedef std::vector<vdf::IDataProcessorPtr> IDataProcessors;
        //! 
        typedef boost::iterator_range<IDataProcessors::const_iterator> IDataProcessorsConstRange;
    public:
        virtual ~IDataProcessorManager(void) {};
        virtual IDataProcessorsConstRange enumPrototypes() const = 0;

    };

    typedef core::shared_ptr<IDataProcessorManager> IDataProcessorManagerPtr;
    typedef core::shared_ptr<const IDataProcessorManager> IDataProcessorManagerConstPtr;
    typedef core::weak_ptr<IDataProcessorManager> IDataProcessorManagerWeakPtr;
    typedef core::weak_ptr<const IDataProcessorManager> IDataProcessorManagerWeakConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif //HEADER_GUARD___ISERVICEMANAGER_H__
