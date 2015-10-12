#include <plugins/dfElements/DFProcessors.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>


void VectorDiff::process()
{
    c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();
    c3dlib::VectorChannelReaderInterfaceConstPtr signal2 = inPinB->getValue();

    if (signal1 && signal2) {
		c3dlib::VectorChannelPtr channel(new c3dlib::VectorChannel(signal1->size() / signal1->getLength()));
        channel->setName("Result");
        size_type count = (std::min)(signal1->size(), signal2->size());

        for (size_type i = 0; i < count; ++i) {
            auto val = signal1->value(i) - signal2->value(i);
            channel->addPoint(val);
        }
        outPinA->setValue(channel);
    } else {

    }
}

void VectorDiff::reset()
{

}

VectorDiff::VectorDiff()
{
    inPinA = new VectorInputPin(this);
    inPinB = new VectorInputPin(this);
    outPinA = new VectorOutputPin(this);
    addInputPin(inPinA);
    addInputPin(inPinB);
    addOutputPin(outPinA);
}

VectorAdder::VectorAdder()
{
    inPinA = new VectorInputPin(this);
    inPinB = new VectorInputPin(this);
    outPinA = new VectorOutputPin(this);
    addInputPin(inPinA);
    addInputPin(inPinB);
    addOutputPin(outPinA);
}

void VectorAdder::reset()
{

}

void VectorAdder::process()
{
    c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();
    c3dlib::VectorChannelReaderInterfaceConstPtr signal2 = inPinB->getValue();

    if (signal1 && signal2) {
		c3dlib::VectorChannelPtr channel(new c3dlib::VectorChannel(signal1->size() / signal1->getLength()));
        channel->setName("Result");
        size_type count = (std::min)(signal1->size(), signal2->size());

        for (size_type i = 0; i < count; ++i) {
            auto val = signal1->value(i) + signal2->value(i);
            channel->addPoint(val);
        }
        outPinA->setValue(channel);
    } else {

    }
}

Vector2Scalar::Vector2Scalar()
{
    inPinA = new VectorInputPin(this);
    outPinX = new ScalarOutputPin(this);
    outPinY = new ScalarOutputPin(this);
    outPinZ = new ScalarOutputPin(this);
    addInputPin(inPinA);
    addOutputPin(outPinX);
    addOutputPin(outPinY);
    addOutputPin(outPinZ);
}

void Vector2Scalar::reset()
{

}

void Vector2Scalar::process()
{
	c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();
    if (signal1) {
        float samplesPS = signal1->size() / signal1->getLength();
        c3dlib::ScalarChannelPtr channelX(new c3dlib::ScalarChannel(samplesPS)); channelX->setName("X");
        c3dlib::ScalarChannelPtr channelY(new c3dlib::ScalarChannel(samplesPS)); channelY->setName("Y");
        c3dlib::ScalarChannelPtr channelZ(new c3dlib::ScalarChannel(samplesPS)); channelZ->setName("Z");

        size_type count = signal1->size();

        for (size_type i = 0; i < count; ++i) {
            auto val = signal1->value(i);
            channelX->addPoint(val[0]);
            channelY->addPoint(val[1]);
            channelZ->addPoint(val[2]);
        }
        outPinX->setValue(channelX);
        outPinY->setValue(channelY);
        outPinZ->setValue(channelZ);
    } else {

    }
}

Scalar2Vector::Scalar2Vector()
{
    outPinA = new VectorOutputPin(this);
    inPinX = new ScalarInputPin(this);
    inPinY = new ScalarInputPin(this);
    inPinZ = new ScalarInputPin(this);
    addOutputPin(outPinA);
    addInputPin(inPinX);
    addInputPin(inPinY);
    addInputPin(inPinZ);
}

void Scalar2Vector::reset()
{

}

void Scalar2Vector::process()
{
    c3dlib::ScalarChannelReaderInterfaceConstPtr signal1 = inPinX->getValue();
    c3dlib::ScalarChannelReaderInterfaceConstPtr signal2 = inPinY->getValue();
    c3dlib::ScalarChannelReaderInterfaceConstPtr signal3 = inPinZ->getValue();
    if (signal1 && signal2 && signal3) {
        float samplesPS = signal1->size() / signal1->getLength();
		c3dlib::VectorChannelPtr channel(new c3dlib::VectorChannel(samplesPS));
        channel->setName("Result");

        size_type count = (std::min)(signal1->size(), signal2->size());
        count = (std::min)(count, signal3->size());

        for (size_type i = 0; i < count; ++i) {
            osg::Vec3 val(signal1->value(i), signal2->value(i), signal3->value(i));
            channel->addPoint(val);
        }
        outPinA->setValue(channel);
    } else {

    }
}

QWidget* ScalarTreshold::getConfigurationWidget()
{
    return widget;
}

void ScalarTreshold::refreshConfiguration()
{

}

ScalarTreshold::ScalarTreshold()
{
    QLayout* layout = new QHBoxLayout();
    QLabel* label = new QLabel(QObject::tr("Treshold: "));
    spinbox = new QDoubleSpinBox();
    layout->addWidget(label);
    layout->addWidget(spinbox);
    spinbox->setValue(1.0);
    widget = new QWidget();
    widget->setLayout(layout);
}
