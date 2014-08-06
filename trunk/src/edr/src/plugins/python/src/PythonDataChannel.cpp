#include "PythonPCH.h"
#include "PythonDataChannel.h"

namespace py = boost::python;

python::PythonDataChannel::PythonDataChannel() :
	frequency(0.0f)
{

}

python::PythonDataChannel::~PythonDataChannel()
{

}

python::PythonDataChannel python::PythonDataChannel::convert(VectorChannelConstPtr channel)
{
	UTILS_ASSERT(channel);
	PythonDataChannel c;
	auto count = channel->size();

	for (auto i = 0; i < count; ++i) {
		c.data.push_back(channel->value(i));
	}
	c.name = channel->getName();
	c.frequency = channel->getSamplesPerSecond();
	return c;
}

VectorChannelPtr python::PythonDataChannel::convert(const PythonDataChannel& obj)
{
	VectorChannelPtr channel = utils::make_shared<VectorChannel>(obj.getFrequency());
	channel->setName("Result");
	auto data = obj.getData();
	int count = data.size();
	for (int i = 0; i < count; ++i) {
		channel->addPoint(data[i]);
	}
	return channel;
}

std::vector<osg::Vec3> python::PythonDataChannel::getData() const
{
	return data;
}

float python::PythonDataChannel::getFrequency() const
{
	return frequency;
}

void python::PythonDataChannel::setFrequency(float frq)
{
	this->frequency = frq;
}

std::string python::PythonDataChannel::getName() const
{
	return name;
}

void python::PythonDataChannel::setData(const std::vector<osg::Vec3>& data)
{
	this->data = data;
}

void python::PythonDataChannel::setName(const std::string& name)
{
	this->name = name;
}


