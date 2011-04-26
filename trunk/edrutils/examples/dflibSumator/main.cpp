#include <dfmlib/DFModel.h>
#include <dfmlib/DFSourceNode.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/DFPin.h>

#include <set>
#include <sstream>
#include <iomanip>

#include <iostream>

#include <boost/shared_ptr.hpp>



template<class Value>
class SmartPin : public dflm::DFPin
{
private:
    typedef boost::shared_ptr<Value> ValuePtr;
    typedef SmartPin<Value> MyType;

public:
    SmartPin(const std::string & pinName = std::string(),
        bool required = false, const dflm::Pin::REQ_PINS_SET & requiredPins = dflm::Pin::REQ_PINS_SET())
        : DFPin(pinName, required, requiredPins)
    {

    }

    ~SmartPin() {}

    const Value& getValue() const
    {
        return *val;
    }

    void setValue(const Value & val)
    {
        this->val.reset(new Value(val));
    }

protected:

    virtual bool onUpdate()
    {
        std::cout << getParent()->getNodeName() + "->" + getPinName() << "\tPo update." << std::endl;
        return true;
    }

    virtual void copyDataFromPin(dflm::DFPinPtr pin)
    {
        boost::shared_ptr<MyType> smartPin(boost::dynamic_pointer_cast<MyType>(pin));

        if(smartPin == nullptr) {
            std::cout << getParent()->getNodeName() + "->" + getPinName() << "\tDane od nieobslugiwanego pinu" << std::endl;
            return;
        }

        val = smartPin->val;

        std::cout << getParent()->getNodeName() + "->" + getPinName() << "\tSkopiowane dane:\t" << *val << std::endl;
    }

private:
    ValuePtr val;

};

template<class Value>
class SummarryNode : public dflm::DFNode
{
private:
    typedef SmartPin<Value> MySmartPin;
    typedef boost::shared_ptr<MySmartPin> MySmartPinPtr;

public:
    SummarryNode(const std::string & nodeName = std::string(), bool processingAllowed = true,
        bool propagatingAllowed = true) : DFNode(nodeName, processingAllowed, propagatingAllowed), val(0)
    {

    }

protected:

    virtual void setOutputData()
    {
        for(auto it = getOutPins().begin(); it != getOutPins().end(); it++){
            MySmartPinPtr myPin = boost::dynamic_pointer_cast<MySmartPin>(*it);
            if(myPin != nullptr){
                myPin->setValue(val);
            }
        }
    }

    virtual void processData()
    {
        val = 0;
        for(auto it = getInPins().begin(); it != getInPins().end(); it++){
            MySmartPinPtr myPin = boost::dynamic_pointer_cast<MySmartPin>(*it);
            if(myPin != nullptr){
                val += myPin->getValue();
            }
        }

        std::cout << getNodeName() << "\tWezel sumujacy wyliczyl wartosc:\t" << val << std::endl;
    }

private:

    Value val;
};

template<class Value>
class SourceNode : public dflm::DFSourceNode
{
private:
    typedef SmartPin<Value> MySmartPin;
    typedef boost::shared_ptr<MySmartPin> MySmartPinPtr;

public:
    SourceNode(Value v, const std::string & nodeName = std::string(), bool processingAllowed = true,
        bool propagatingAllowed = true) : DFNode(nodeName), DFSourceNode(nodeName, processingAllowed, propagatingAllowed), val(v), count(1)
    {
        
    }

    virtual bool hasMoreData() const
    {
        return count > 0;
    }

protected:

    virtual void setOutputData()
    {
        for(auto it = getOutPins().begin(); it != getOutPins().end(); it++){
            MySmartPinPtr myPin = boost::dynamic_pointer_cast<MySmartPin>(*it);
            if(myPin != nullptr){
                myPin->setValue(val);
            }
        }

        count --;
    }

private:

    int count;  
    Value val;
};

typedef SourceNode<int> IntSourceNode;
typedef SummarryNode<int> IntSummaryNode;
typedef SmartPin<int> IntSmartPin;
typedef boost::shared_ptr<IntSmartPin> IntSMartPinPtr;

dflm::NPtr buildSimpleSource(const std::string & name, int emitVal);

dflm::NPtr buildSummer(const std::string & name);

int main(int argc, char** argv) {

    dflm::DFMPtr model(new dflm::DFModel());

    dflm::NPtr sourceA(buildSimpleSource("Simple source 5", 5));
    dflm::NPtr sourceB(buildSimpleSource("Simple source 3", 3));
    dflm::NPtr sourceC(buildSimpleSource("Simple source 7", 7));
    dflm::NPtr sourceD(buildSimpleSource("Simple source 1", 1));
    
    dflm::NPtr summerA(buildSummer("Summer A"));
    dflm::NPtr summerB(buildSummer("Summer B"));
    dflm::NPtr summerC(buildSummer("Summer C"));
    dflm::NPtr summerD(buildSummer("Summer D"));


    model->addNode(sourceA);
    model->addNode(sourceB);
    model->addNode(sourceC);
    model->addNode(sourceD);
    model->addNode(summerA);
    model->addNode(summerB);
    model->addNode(summerC);
    model->addNode(summerD);

    model->connect(*(sourceA->getOutPins().begin()), *(summerA->getInPins().begin()));
    model->connect(*(sourceB->getOutPins().begin()), *(++(summerA->getInPins().begin())));

    model->connect(*(sourceB->getOutPins().begin()), *(summerC->getInPins().begin()));
    model->connect(*(sourceC->getOutPins().begin()), *(++(summerC->getInPins().begin())));

    model->connect(*(sourceC->getOutPins().begin()), *(summerB->getInPins().begin()));
    model->connect(*(sourceD->getOutPins().begin()), *(++(summerB->getInPins().begin())));

    model->connect(*(summerA->getOutPins().begin()), *(summerD->getInPins().begin()));
    model->connect(*(summerC->getOutPins().begin()), *(++(summerD->getInPins().begin())));

    model->setEnable(true);


	return 0;
}



dflm::NPtr buildSimpleSource(const std::string & name, int emitVal)
{
    dflm::NPtr ret(new IntSourceNode(emitVal, name));
    
    ret->addOutPin(IntSMartPinPtr(new IntSmartPin("Out Pin A", true)));

    return ret;
}

dflm::NPtr buildSummer(const std::string & name)
{
    dflm::NPtr ret(new IntSummaryNode(name));

    ret->addInPin(IntSMartPinPtr(new IntSmartPin("In Pin A", true)));
    ret->addInPin(IntSMartPinPtr(new IntSmartPin("In Pin B", true)));
    ret->addOutPin(IntSMartPinPtr(new IntSmartPin("Out Pin A - sum", true)));

    return ret;
}