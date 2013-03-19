/********************************************************************
	created:	2013/01/07
	created:	7:1:2013   20:51
	filename: 	IDataSinkManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD___IDATASINKMANAGER_H__
#define HEADER_GUARD___IDATASINKMANAGER_H__

#include <plugins/newVdf/IDataSink.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>

namespace vdf{

    class IDataSinkManager : public utils::Observable<IDataSinkManager>
    {
    public:
        //! Lista Ÿróde³ danych.
        typedef std::vector<vdf::IDataSinkPtr> IDataSinks;
        //! 
        typedef boost::iterator_range<IDataSinks::const_iterator> DataSinksConstRange;

    public:
        virtual ~IDataSinkManager(void) {};
        virtual DataSinksConstRange enumPrototypes() const = 0;

    };

    typedef core::shared_ptr<IDataSinkManager> IDataSinkManagerPtr;
    typedef core::shared_ptr<const IDataSinkManager> IDataSinkManagerConstPtr;
    typedef core::weak_ptr<IDataSinkManager> IDataSinkManagerWeakPtr;
    typedef core::weak_ptr<const IDataSinkManager> IDataSinkManagerWeakConstPtr;

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  
