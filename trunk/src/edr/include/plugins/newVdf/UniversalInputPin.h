/********************************************************************
	created:	2013/10/02
	created:	2:10:2013   10:31
	filename: 	UniversalInputPin.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__UNIVERSALINPUTPIN_H__
#define HEADER_GUARD_NEWVDF__UNIVERSALINPUTPIN_H__

#include <utils/ObjectWrapper.h>
#include <utils/TypeInfo.h>
#include <dflib/IDFPin.h>
#include <dflib/Pin.h>
#include <plugins/newVdf/Export.h>

namespace vdf {

//! Klasa potrafi obs³u¿yæ piny, które implementuj¹ interfejs IMDEOutputPin
class PLUGIN_NEWVDF_EXPORT UniversalInputPinBase : public df::InputPin, public df::IDFInput
{
public:
    //! 
    //! \param node 
    UniversalInputPinBase( df::ISinkNode * node);

public:
    //! 
    virtual void reset();
    //! 
    //! \param pin 
    void copyData( const df::IDFOutput * pin );
    //! 
    //! \param pin 
    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;
    //! 
    utils::ObjectWrapperConstPtr getWrapper() const;
    //! 
    //! \param val 
    void setWrapper(utils::ObjectWrapperConstPtr val);

private:
    utils::ObjectWrapperConstPtr wrapper;
};


class PLUGIN_NEWVDF_EXPORT UniversalInputPin : public UniversalInputPinBase
{
public:
    UniversalInputPin(df::ISinkNode* node, const utils::TypeInfo& info);
    virtual ~UniversalInputPin();

    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;
    utils::TypeInfo getInfo() const;

protected:
    utils::TypeInfo info;
};

template <class T>
class UniversalInputPinT : public UniversalInputPin
{
public:
    typedef typename utils::ObjectWrapperT<T>::Ptr Ptr;
    typedef typename utils::ObjectWrapperT<T>::ConstPtr ConstPtr;
public:
    UniversalInputPinT(df::ISinkNode * node) : UniversalInputPin(node, typeid(T)) {}
    virtual ~UniversalInputPinT() {}

    ConstPtr getValue() { return getWrapper()->get(); }
    void setValue(Ptr item)
    {
        utils::ObjectWrapperPtr wrapper = utils::ObjectWrapper::create<T>();
        wrapper->set(item);
        setWrapper(wrapper);
    }

private:
    utils::TypeInfo info;
};

}

#endif
