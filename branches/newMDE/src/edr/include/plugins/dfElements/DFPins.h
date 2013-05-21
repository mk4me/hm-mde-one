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
//class HmmMainWindow;

template <class T>
class PLUGIN_DFELEMENTS_EXPORT UniversalOutputPin : public df::OutputPin, public df::IDFOutput, public vdf::IMDEOutputPin
{
    MDE_OUTPUT_PIN(T);
public:
    UniversalOutputPin(df::ISourceNode * node) : df::OutputPin(node) {}

    typedef utils::ObjectWrapperT<T> Wrapper;
    typedef typename Wrapper::ConstPtr ConstPtr;

    const ConstPtr value() const
    {
        return val;
    }

    void value(ConstPtr val) 
    {
        this->val = val;
    }

    virtual void reset() {
        val = ConstPtr();
    }

private:
    ConstPtr val;
};

class PLUGIN_DFELEMENTS_EXPORT VectorOutputPin : public df::OutputPin, public df::IDFOutput, public vdf::IMDEOutputPin
{
	MDE_OUTPUT_PIN(VectorChannelReaderInterface);
public:
    VectorOutputPin(df::ISourceNode * node);

    const VectorChannelReaderInterfaceConstPtr value() const;
    void value(VectorChannelReaderInterfaceConstPtr val);

    virtual void reset();

private:
    VectorChannelReaderInterfaceConstPtr val;

};

class PLUGIN_DFELEMENTS_EXPORT VectorInputPin : public df::InputPin, public df::IDFInput
{
public:
    VectorInputPin(df::ISinkNode * node);

public:
    virtual void copyData(const df::IDFOutput * pin);
    virtual void reset();
    const VectorChannelReaderInterfaceConstPtr value() const;

    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;

private:
    VectorChannelReaderInterfaceConstPtr val;
};


class PLUGIN_DFELEMENTS_EXPORT ScalarOutputPin : public df::OutputPin, public df::IDFOutput, public vdf::IMDEOutputPin
{
    MDE_OUTPUT_PIN(ScalarChannelReaderInterface);

public:
    ScalarOutputPin(df::ISourceNode * node) : df::OutputPin(node) {}

    const ScalarChannelReaderInterfaceConstPtr value() const { return val; }
    void value(ScalarChannelReaderInterfaceConstPtr val) { this->val = val; }

    virtual void reset() { val = ScalarChannelReaderInterfaceConstPtr(); }

private:
    ScalarChannelReaderInterfaceConstPtr val;
};

class PLUGIN_DFELEMENTS_EXPORT ScalarInputPin : public df::InputPin, public df::IDFInput
{
public:
    ScalarInputPin(df::ISinkNode * node) : df::InputPin(node) {}

public:
    virtual void copyData(const df::IDFOutput * pin) { val = dynamic_cast<const ScalarOutputPin*>(pin)->value(); }
    virtual void reset() { val = ScalarChannelReaderInterfaceConstPtr(); }
    const ScalarChannelReaderInterfaceConstPtr value() const { return val; }

    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;

private:
    ScalarChannelReaderInterfaceConstPtr val;
};

class PLUGIN_DFELEMENTS_EXPORT ChannelInputPin : public df::InputPin, public df::IDFInput
{
public:
    ChannelInputPin(df::ISinkNode * node);

public:
    virtual void copyData(const df::IDFOutput * pin);
    virtual void reset();
    //const ScalarChannelReaderInterfaceConstPtr value() const;
    ScalarChannelReaderInterfaceConstPtr getScalar() const { return scalar; }
    VectorChannelReaderInterfaceConstPtr getVector() const { return vector; }

    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;

private:
    ScalarChannelReaderInterfaceConstPtr scalar;
    VectorChannelReaderInterfaceConstPtr vector;
};


//class ChannelOutputPin : public df::InputPin, public df::IDFInput
//{
//public:
//    ChannelOutputPin(df::ISinkNode * node);
//
//public:
//    virtual void reset()
//    {
//        scalar = ScalarChannelReaderInterfaceConstPtr(); 
//        vector = VectorChannelReaderInterfaceConstPtr();
//    }
//    //const ScalarChannelReaderInterfaceConstPtr value() const;
//    VectorChannelReaderInterfaceConstPtr getVector() const { return vector; }
//    void setVector(VectorChannelReaderInterfaceConstPtr val) { vector = val; }
//    ScalarChannelReaderInterfaceConstPtr getScalar() const { return scalar; }
//    void setScalar(ScalarChannelReaderInterfaceConstPtr val) { scalar = val; }
//
//private:
//    ScalarChannelReaderInterfaceConstPtr scalar;
//    VectorChannelReaderInterfaceConstPtr vector;
//   
//};

#endif
