/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	DFProcessors.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS__HMMPROCESSORS_H__
#define HEADER_GUARD_DFELEMENTS__HMMPROCESSORS_H__

#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/UniversalOutputPin.h>
#include <plugins/dfElements/DFPins.h>
#include <QtWidgets/QDoubleSpinBox>
#include <dflib/Node.h>
#include <dflib/IDFNode.h>
#include <plugins/newVdf/INodeConfiguration.h>

class VectorDiff : public df::ProcessingNode, public df::IDFProcessor
{
public:
    VectorDiff();

public:
    virtual void reset();
    virtual void process();

private:
    VectorInputPin* inPinA;
    VectorInputPin* inPinB;
    VectorOutputPin* outPinA;
    std::string name;
};

class VectorAdder : public df::ProcessingNode, public df::IDFProcessor
{
public:
    VectorAdder();

public:
    virtual void reset();
    virtual void process();

private:
    VectorInputPin* inPinA;
    VectorInputPin* inPinB;
    VectorOutputPin* outPinA;
    std::string name;
};

template <class InputPinT, class OutputPinT, class InputPtr, class OutputPtr>
class UniversalChannelProcessor : public df::ProcessingNode, public df::IDFProcessor
{
public:
    UniversalChannelProcessor()
    {
        inPinA = new InputPinT(this);
        outPinA = new OutputPinT(this);
        addInputPin(inPinA);
        addOutputPin(outPinA);
    }

public:
    virtual void reset() {}
    typedef typename OutputPtr::element_type::point_type SimpleOutT;
    typedef typename InputPtr::element_type::point_type SimpleInT;
    virtual SimpleOutT processSingleValue(const SimpleInT& val) = 0;
    virtual void process()
    {
        InputPtr signal1 = inPinA->getValue();

        if (signal1) {
            OutputPtr channel(new typename OutputPtr::element_type(signal1->size() / signal1->getLength()));
            channel->setName("Result");
            size_type count = signal1->size();

            for (size_type i = 0; i < count; ++i) {
                channel->addPoint(processSingleValue(signal1->value(i)));
            }
            outPinA->setValue(channel);
        } else {

        }
    }

private:
    InputPinT * inPinA;
    OutputPinT * outPinA;
    std::string name;
};


template <class InputPinT, class OutputPinT, class InputPtr, class OutputPtr>
class UniversalChannel2Processor : public df::ProcessingNode, public df::IDFProcessor
{
public:
    UniversalChannel2Processor()
    {
        inPinA = new InputPinT(this);
        inPinB = new InputPinT(this);
        outPinA = new OutputPinT(this);
        addInputPin(inPinA);
        addInputPin(inPinB);
        addOutputPin(outPinA);
    }

public:
    virtual void reset() {}
    typedef typename OutputPtr::element_type::point_type SimpleOutT;
    typedef typename InputPtr::element_type::point_type SimpleInT;
    virtual SimpleOutT processSingleValue(const SimpleInT& v1, const SimpleInT& v2) = 0;
    virtual void process()
    {
        InputPtr signal1 = inPinA->getValue();
        InputPtr signal2 = inPinB->getValue();

        if (signal1 && signal2) {
            OutputPtr channel(new typename OutputPtr::element_type(signal1->size() / signal1->getLength()));
            channel->setName("Result");
            size_type count = (std::min)(signal1->size(), signal2->size());

            for (size_type i = 0; i < count; ++i) {
                auto val = processSingleValue(signal1->value(i), signal2->value(i));
                channel->addPoint(val);
            }
            outPinA->setValue(channel);
        } else {

        }
    }

private:
   InputPinT * inPinA;
   InputPinT * inPinB;
   OutputPinT * outPinA;
   std::string name;
};

typedef UniversalChannelProcessor<ScalarInputPin, ScalarOutputPin, c3dlib::ScalarChannelReaderInterfaceConstPtr, c3dlib::ScalarChannelPtr> UniversalScalar;
typedef UniversalChannel2Processor<ScalarInputPin, ScalarOutputPin, c3dlib::ScalarChannelReaderInterfaceConstPtr, c3dlib::ScalarChannelPtr> Universal2Scalar;
typedef UniversalChannelProcessor<VectorInputPin, VectorOutputPin, c3dlib::VectorChannelReaderInterfaceConstPtr, c3dlib::VectorChannelPtr> UniversalVector;
typedef UniversalChannel2Processor<VectorInputPin, VectorOutputPin, c3dlib::VectorChannelReaderInterfaceConstPtr, c3dlib::VectorChannelPtr> Universal2Vector;

class ScalarDiff : public Universal2Scalar
{
public:
    virtual float processSingleValue(const float& v1, const float& v2) 
    {
        return v1 - v2;
    }
};

class ScalarAdd : public Universal2Scalar
{
public:
    virtual float processSingleValue(const float& v1, const float& v2) 
    {
        return v1 + v2;
    }
};

class ScalarTreshold : public UniversalScalar, public vdf::INodeConfiguration
{
public:
    ScalarTreshold();
    virtual float processSingleValue(const float& v1) 
    {
        float treshold = static_cast<float>(spinbox->value());
        return (std::min)(v1, treshold);
    }

    virtual QWidget* getConfigurationWidget();
    virtual void refreshConfiguration();
private:
    QWidget* widget;
    QDoubleSpinBox* spinbox;
};

class ScalarMul : public Universal2Scalar
{
public:
    virtual float processSingleValue(const float& v1, const float& v2) 
    {
        return v1 + v2;
    }
};

class ScalarInverter : public UniversalScalar
{
public:
    virtual float processSingleValue(const float& v1) 
    {
        return -v1;
    }
};

class ScalarDummy : public UniversalScalar
{
public:
    virtual float processSingleValue(const float& v1) 
    {
        return v1;
    }
};

class VectorMul : public Universal2Vector
{
public:
    virtual osg::Vec3 processSingleValue(const osg::Vec3& v1, const osg::Vec3& v2) 
    {
        return osg::Vec3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
    }
};

class Vector2Scalar : public df::ProcessingNode, public df::IDFProcessor
{
public:
    Vector2Scalar();

public:
    virtual void reset();
    virtual void process();

private:
    VectorInputPin * inPinA;
    ScalarOutputPin * outPinX;
    ScalarOutputPin * outPinY;
    ScalarOutputPin * outPinZ;
    std::string name;
};

class Scalar2Vector : public df::ProcessingNode, public df::IDFProcessor
{
public:
    Scalar2Vector();

public:
    virtual void reset();
    virtual void process();

private:
    VectorOutputPin * outPinA;
    ScalarInputPin * inPinX;
    ScalarInputPin * inPinY;
    ScalarInputPin * inPinZ;
    std::string name;
};

#endif
