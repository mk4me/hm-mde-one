/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   19:08
	filename: 	DFPins.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS__HMMPINS_H__
#define HEADER_GUARD_DFELEMENTS__HMMPINS_H__

#include <plugins/newVdf/NewVdfService.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/dfElements/Export.h>
#include <plugins/newVdf/IDataFlowProvider.h>
#include <utils/ObjectWrapper.h>
#include <utils/TypeInfo.h>
#include <dflib/IDFPin.h>

class PLUGIN_DFELEMENTS_EXPORT UniversalOutputPinBase : public df::OutputPin, public df::IDFOutput
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

class PLUGIN_DFELEMENTS_EXPORT UniversalOutputPin : public UniversalOutputPinBase
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

//! Klasa potrafi obs³u¿yæ piny, które implementuj¹ interfejs IMDEOutputPin
class PLUGIN_DFELEMENTS_EXPORT UniversalInputPinBase : public df::InputPin, public df::IDFInput
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

class PLUGIN_DFELEMENTS_EXPORT UniversalInputPin : public UniversalInputPinBase
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

typedef UniversalInputPinT<VectorChannelReaderInterface> VectorInputPin;
typedef UniversalOutputPinT<VectorChannelReaderInterface> VectorOutputPin;

typedef UniversalInputPinT<ScalarChannelReaderInterface> ScalarInputPin;
typedef UniversalOutputPinT<ScalarChannelReaderInterface> ScalarOutputPin;


//class PLUGIN_DFELEMENTS_EXPORT VectorOutputPin : public df::OutputPin, public df::IDFOutput, public vdf::IMDEOutputPin
//{
//	MDE_OUTPUT_PIN(VectorChannelReaderInterface);
//public:
//    VectorOutputPin(df::ISourceNode * node);
//
//    const VectorChannelReaderInterfaceConstPtr value() const;
//    void value(VectorChannelReaderInterfaceConstPtr val);
//
//    virtual void reset();
//
//private:
//    VectorChannelReaderInterfaceConstPtr val;
//
//};
//
//class PLUGIN_DFELEMENTS_EXPORT VectorInputPin : public df::InputPin, public df::IDFInput
//{
//public:
//    VectorInputPin(df::ISinkNode * node);
//
//public:
//    virtual void copyData(const df::IDFOutput * pin);
//    virtual void reset();
//    const VectorChannelReaderInterfaceConstPtr value() const;
//
//    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;
//
//private:
//    VectorChannelReaderInterfaceConstPtr val;
//};
//
//
//class PLUGIN_DFELEMENTS_EXPORT ScalarOutputPin : public df::OutputPin, public df::IDFOutput, public vdf::IMDEOutputPin
//{
//    MDE_OUTPUT_PIN(ScalarChannelReaderInterface);
//
//public:
//    ScalarOutputPin(df::ISourceNode * node) : df::OutputPin(node) {}
//
//    const ScalarChannelReaderInterfaceConstPtr value() const { return val; }
//    void value(ScalarChannelReaderInterfaceConstPtr val) { this->val = val; }
//
//    virtual void reset() { val = ScalarChannelReaderInterfaceConstPtr(); }
//
//private:
//    ScalarChannelReaderInterfaceConstPtr val;
//};
//
//class PLUGIN_DFELEMENTS_EXPORT ScalarInputPin : public df::InputPin, public df::IDFInput
//{
//public:
//    ScalarInputPin(df::ISinkNode * node) : df::InputPin(node) {}
//
//public:
//    virtual void copyData(const df::IDFOutput * pin) { val = dynamic_cast<const ScalarOutputPin*>(pin)->value(); }
//    virtual void reset() { val = ScalarChannelReaderInterfaceConstPtr(); }
//    const ScalarChannelReaderInterfaceConstPtr value() const { return val; }
//
//    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;
//
//private:
//    ScalarChannelReaderInterfaceConstPtr val;
//};
//
//class PLUGIN_DFELEMENTS_EXPORT ChannelInputPin : public df::InputPin, public df::IDFInput
//{
//public:
//    ChannelInputPin(df::ISinkNode * node);
//
//public:
//    virtual void copyData(const df::IDFOutput * pin);
//    virtual void reset();
//    //const ScalarChannelReaderInterfaceConstPtr value() const;
//    ScalarChannelReaderInterfaceConstPtr getScalar() const { return scalar; }
//    VectorChannelReaderInterfaceConstPtr getVector() const { return vector; }
//
//    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;
//
//private:
//    ScalarChannelReaderInterfaceConstPtr scalar;
//    VectorChannelReaderInterfaceConstPtr vector;
//};

#endif
