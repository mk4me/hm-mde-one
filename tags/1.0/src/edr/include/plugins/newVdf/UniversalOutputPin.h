/********************************************************************
	created:	2013/10/02
	created:	2:10:2013   10:34
	filename: 	UniversalOutputPin.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__UNIVERSALOUTPUTPIN_H__
#define HEADER_GUARD_NEWVDF__UNIVERSALOUTPUTPIN_H__

#include <plugins/newVdf/Export.h>
#include <dflib/IDFPin.h>
#include <utils/ObjectWrapper.h>
#include <dflib/IDFPin.h>
#include <dflib/Pin.h>

namespace vdf {

class PLUGIN_NEWVDF_EXPORT UniversalOutputPinBase : public df::OutputPin, public df::IDFOutput
{
public:
    UniversalOutputPinBase(df::ISourceNode * node);

    typedef utils::ObjectWrapperConstPtr ConstPtr;

    const ConstPtr getWrapper() const;

    void setWrapper(ConstPtr val);

    virtual void reset();

private:
    ConstPtr val;
};

class PLUGIN_NEWVDF_EXPORT UniversalOutputPin : public UniversalOutputPinBase
{
public:
    UniversalOutputPin(df::ISourceNode* node, const utils::TypeInfo& info);
    virtual ~UniversalOutputPin();

    utils::TypeInfo getInfo() const;

protected:
    utils::TypeInfo info;
};

template <class T>
class UniversalOutputPinT : public UniversalOutputPin
{
public:
    typedef typename utils::ObjectWrapperT<T>::Ptr Ptr;
    typedef typename utils::ObjectWrapperT<T>::ConstPtr ConstPtr;
public:
    UniversalOutputPinT (df::ISourceNode* node) : UniversalOutputPin(node, typeid(T)) {}
    virtual ~UniversalOutputPinT() {}

    ConstPtr getValue() { return getWrapper()->get(); }
    void setValue(Ptr item)
    {
        utils::ObjectWrapperPtr wrapper = utils::ObjectWrapper::create<T>();
        wrapper->set(item);
        setWrapper(wrapper);
    }
};

}
#endif
