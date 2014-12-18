#include "C3DPCH.h"
#include <plugins/c3d/C3DChannels.h>

#include <stdexcept>
#include <utils/Debug.h>

#define DEFINE_CHANNEL(name)																\
	name##Channel::name##Channel(int samplesPerSec) : VectorChannel(samplesPerSec) {}		\
	        																				\
	name##Channel::name##Channel(const name##Channel& channel) : VectorChannel(channel) {}	\
																							\
	name##Channel::name##Channel(const c3dlib::C3DParser& data, int channelNo )				\
	: VectorChannel( data.getPointFrequency()) {											\
		  c3dlib::C3DParser::IPointConstPtr point = data.getPoint(channelNo);				\
		  int numSamples = data.getNumPointFrames();										\
		  for(int i = 0; i < numSamples; ++i) { addPoint(point->getValue(i)); }				\
		  setName(point->getLabel());														\
		  setValueBaseUnit(point->getUnit()); }												\
																							 \
	name##Channel* name##Channel::clone() const { return new name##Channel(*this); }

DEFINE_CHANNEL(Force);
DEFINE_CHANNEL(Moment);
DEFINE_CHANNEL(Angle);
DEFINE_CHANNEL(Power);

VectorToScalarAdaptor::VectorToScalarAdaptor(const VectorChannelReaderInterfaceConstPtr & vector, size_type idx, const std::string & name)
	: descriptor(*vector), vector(vector), index(idx), name(name)
{
	UTILS_ASSERT((idx >= 0 && idx < 3), "Bledny index dla adaptera skalarnego kanalu wektorowego");
}


VectorToScalarAdaptor::VectorToScalarAdaptor(const VectorToScalarAdaptor & adaptor)
	: descriptor(adaptor.descriptor), vector(adaptor.vector), index(adaptor.index),
	name(adaptor.name)
{

}

VectorToScalarAdaptor::~VectorToScalarAdaptor()
{

}

const std::string& VectorToScalarAdaptor::getName() const
{
	return name;
}

void VectorToScalarAdaptor::setName(const std::string& name)
{
	this->name = name;
}

VectorToScalarAdaptor * VectorToScalarAdaptor::clone() const
{
	return new VectorToScalarAdaptor(*this);
}

VectorToScalarAdaptor::time_type VectorToScalarAdaptor::getLength() const
{
	return vector->getLength();
}

VectorToScalarAdaptor::time_type VectorToScalarAdaptor::argument(size_type idx) const
{
	return vector->argument(idx);
}

VectorToScalarAdaptor::point_type_const_reference VectorToScalarAdaptor::value(size_type idx) const
{
	return vector->value(idx)[index];
}

VectorToScalarAdaptor::size_type VectorToScalarAdaptor::size() const
{
	return vector->size();
}

bool VectorToScalarAdaptor::empty() const
{
	return vector->empty();
}

float VectorToScalarAdaptor::getSamplesPerSecond() const
{
	return vector->getSamplesPerSecond();
}

float VectorToScalarAdaptor::getSampleDuration() const
{
	return vector->getSampleDuration();
}

const std::string& VectorToScalarAdaptor::getTimeBaseUnit() const
{
	return descriptor.getTimeBaseUnit();
}

float VectorToScalarAdaptor::getTimeScaleFactor() const
{
	return descriptor.getTimeScaleFactor();
}

const std::string& VectorToScalarAdaptor::getValueBaseUnit() const
{
	return descriptor.getValueBaseUnit();
}

float VectorToScalarAdaptor::getValueScaleFactor() const
{
	return descriptor.getValueScaleFactor();
}

void VectorToScalarAdaptor::setTimeBaseUnit(const std::string& timeBaseUnit)
{
	descriptor.setTimeBaseUnit(timeBaseUnit);
}

void VectorToScalarAdaptor::setTimeScaleFactor(float timeScaleFactor)
{
	descriptor.setTimeScaleFactor(timeScaleFactor);
}

void VectorToScalarAdaptor::setValueBaseUnit(const std::string& valueBaseUnit)
{
	descriptor.setValueBaseUnit(valueBaseUnit);
}

void VectorToScalarAdaptor::setValueScaleFactor(float valueScaleFactor)
{
	descriptor.setValueScaleFactor(valueScaleFactor);
}


ScalarWithTimeSegment::ScalarWithTimeSegment(const ScalarWithTimeSegment & adaptor)
	: descriptor(adaptor.descriptor), startIndex(adaptor.startIndex), endIndex(adaptor.endIndex)
{

}

ScalarWithTimeSegment::ScalarWithTimeSegment(const ScalarChannelReaderInterfaceConstPtr & channel, time_type start, time_type end)
	: descriptor(*channel),	reader(channel)
{
	startIndex = channel->getValueHelper(start).first;
	endIndex = channel->getValueHelper(end).second;
}

ScalarWithTimeSegment::~ScalarWithTimeSegment()
{

}

const std::string& ScalarWithTimeSegment::getName() const
{
	return reader->getName();
}

ScalarWithTimeSegment * ScalarWithTimeSegment::clone() const
{
	return new ScalarWithTimeSegment(*this);
}

ScalarWithTimeSegment::time_type ScalarWithTimeSegment::getLength() const
{
	return static_cast<time_type>(100);
}

ScalarWithTimeSegment::size_type ScalarWithTimeSegment::transformIndex(size_type idx) const
{
	return startIndex + idx;
	float x = static_cast<float>(idx) / size();
	return static_cast<size_type>(startIndex * (1.0f - x) + endIndex * x);
}

ScalarWithTimeSegment::time_type ScalarWithTimeSegment::argument(size_type idx) const
{
	return static_cast<time_type>(idx * 100) / size();
}

ScalarWithTimeSegment::point_type_const_reference ScalarWithTimeSegment::value(size_type idx) const
{
	return reader->value(transformIndex(idx));
}

ScalarWithTimeSegment::size_type ScalarWithTimeSegment::size() const
{
	return endIndex - startIndex;
}

bool ScalarWithTimeSegment::empty() const
{
	return reader->empty();
}

float ScalarWithTimeSegment::getSamplesPerSecond() const
{
	return size() / getLength();
}

float ScalarWithTimeSegment::getSampleDuration() const
{
	if (size()) {
		return 1.0 / getSamplesPerSecond();
	}

	throw core::runtime_error("empty channel");
}

const std::string& ScalarWithTimeSegment::getTimeBaseUnit() const
{
	return descriptor.getTimeBaseUnit();
}

float ScalarWithTimeSegment::getTimeScaleFactor() const
{
	return descriptor.getTimeScaleFactor();
}

const std::string& ScalarWithTimeSegment::getValueBaseUnit() const
{
	return descriptor.getValueBaseUnit();
}

float ScalarWithTimeSegment::getValueScaleFactor() const
{
	return descriptor.getValueScaleFactor();
}

void ScalarWithTimeSegment::setTimeBaseUnit(const std::string& timeBaseUnit)
{
	descriptor.setTimeBaseUnit(timeBaseUnit);
}

void ScalarWithTimeSegment::setTimeScaleFactor(float timeScaleFactor)
{
	descriptor.setTimeScaleFactor(timeScaleFactor);
}

void ScalarWithTimeSegment::setValueBaseUnit(const std::string& valueBaseUnit)
{
	descriptor.setValueBaseUnit(valueBaseUnit);
}

void ScalarWithTimeSegment::setValueScaleFactor(float valueScaleFactor)
{
	descriptor.setValueScaleFactor(valueScaleFactor);
}

void ScalarChannelNormalizer::operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
	const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
	const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
{
	//uzupe³nij brakuj¹ce prboki
	if(myChannel.size() < observedChannel.size()){
		for(auto idx = myChannel.size(); idx < observedChannel.size(); ++idx){
			modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
		}
	}

	auto i = 0;

	//min i max
	auto minVal = observedChannel.value(0);
	auto maxVal = minVal;

	++i;

	for( ; i != observedChannel.size(); ++i){
		if(observedChannel.value(i) < minVal){
			minVal = observedChannel.value(i);
		}else if(observedChannel.value(i) > maxVal){
			maxVal = observedChannel.value(i);
		}
	}

	ScalarChannelReaderInterface::point_type diff = maxVal - minVal;

	if(diff != 0){
		//aktualizacja próbek
		for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); ++idx){
			modifierInterface.setIndexData(idx, (observedChannel.value(idx) - minVal) / diff);
		}
	}
}

C3DAnalogChannel::C3DAnalogChannel( const c3dlib::C3DParser& c3dData, int channelNo ) :
	ScalarChannel( static_cast<int>(c3dData.getNumberAnalogSamplePerFrame() * c3dData.getPointFrequency()) )
	{
		if ( c3dData.getNumAnalogFrames() == 0) {
			throw core::runtime_error("Incorrect number of frames.");
		}

		c3dlib::C3DParser::IAnalogConstPtr analog = c3dData.getAnalog(channelNo);

		int numSamples = c3dData.getNumAnalogFrames();
		for (int i = 0; i < numSamples; ++i) {
			addPoint(static_cast<float>(analog->getValue(i)));
		}

		setName(analog->getLabel());
		setValueBaseUnit(analog->getUnit());
		setTimeBaseUnit("s");
	}
	
C3DAnalogChannel::C3DAnalogChannel( int samplesPerSec ) : ScalarChannel(samplesPerSec)
{

}

C3DAnalogChannel::C3DAnalogChannel( const C3DAnalogChannel& channel ) : ScalarChannel(channel)
{

}

EMGChannel::EMGChannel(int samplesPerSec) : C3DAnalogChannel(samplesPerSec)
{

}

EMGChannel::EMGChannel(const EMGChannel& channel) : C3DAnalogChannel(channel)
{

}

EMGChannel::EMGChannel(const c3dlib::C3DParser& data, int channelNo) : C3DAnalogChannel(data, channelNo)
{

}

EMGChannel* EMGChannel::clone() const
{
	return new EMGChannel(*this);
}

GRFChannel::GRFChannel(int samplesPerSec, float treshold) : VectorChannel(samplesPerSec) ,
	type(Unknown),
	treshold(treshold)
{

}

GRFChannel::GRFChannel(const GRFChannel& channel) : VectorChannel(channel),
	type(channel.type)
{

}

GRFChannel::GRFChannel( const c3dlib::C3DParser& data, int channelNo, float treshold) :
	VectorChannel(static_cast<int>(data.getNumberAnalogSamplePerFrame() * data.getPointFrequency())),
	type(Unknown),
	dataStart(-1.0f),
	dataEnd(-1.0f)
{
	if ( data.getNumAnalogFrames() == 0) {
		throw core::runtime_error("Incorrect number of frames.");
	}

	float treshold2 = treshold * treshold;

	c3dlib::C3DParser::IAnalogConstPtr x = data.getAnalog(3 * channelNo + 0);
	c3dlib::C3DParser::IAnalogConstPtr y = data.getAnalog(3 * channelNo + 1);
	c3dlib::C3DParser::IAnalogConstPtr z = data.getAnalog(3 * channelNo + 2);

	bool dataStartSet = false;
	bool dataEndSet = false;

	int startIndex, endIndex;

	int numSamples = data.getNumAnalogFrames();
	for (int i = 0; i < numSamples; ++i) {
		osg::Vec3 val = osg::Vec3(x->getValue(i), y->getValue(i), z->getValue(i));
		if (val.length2() > treshold2) {
			if (!dataStartSet) {
				dataStartSet = true;
				startIndex = i;
			}
		} else {
			if (dataStartSet && !dataEndSet) {
				endIndex = i;
				dataEndSet = true;
			}
		}
		addPoint(val);
	}

	std::string name = x->getLabel();
	auto it = std::remove(name.begin(), name.end(), 'x');
	if (it == name.end()) {
		it = std::remove(name.begin(), name.end(), 'X');
	}

	if (it != name.end()) {
		name.erase(it);
	}

	//std::string name = x->getLabel().erase( remove( str.begin(), str.end(), 'x' ), str.end() );
	setName(name);
	setValueBaseUnit(x->getUnit());
	//? w pliku c3d nie pojawia siê cos takiego jak jednostka czasu, dlatego mo¿na przyj¹æ sekundy
	setTimeBaseUnit("s");

	if (dataStartSet) {
		dataStart = (startIndex / static_cast<float>(numSamples)) * getLength();
	}
	if (dataEndSet) {
		dataEnd = (endIndex / static_cast<float>(numSamples)) * getLength();
	}

	switch (channelNo) {
	case 0:		type = F1;		break;
	case 1:		type = M1;		break;
	case 2:		type = F2;		break;
	case 3:		type = M2;		break;
	default:	type = Unknown; break;
	}
}

GRFChannel* GRFChannel::clone() const
{
	return new GRFChannel(*this);
}

GRFChannel::Type GRFChannel::getType() const
{
	return type;
}

bool GRFChannel::hasStartEndData() const
{
	return dataStart >= 0.0f && dataEnd >= 0.0f;
}

float GRFChannel::getDataStartTime() const
{
	UTILS_ASSERT(dataStart >= 0.0f); return dataStart;
}

float GRFChannel::getDataEndTime() const
{
	UTILS_ASSERT(dataEnd >= 0.0f); return dataEnd;
}

MarkerChannel::MarkerChannel(int samplesPerSec) : VectorChannel(samplesPerSec)
{

}

MarkerChannel::MarkerChannel(const MarkerChannel& channel) : VectorChannel(channel)
{

}

MarkerChannel::MarkerChannel(const c3dlib::C3DParser& data, int channelNo ) :
	VectorChannel( data.getPointFrequency())
{
	c3dlib::C3DParser::IPointConstPtr point = data.getPoint(channelNo);

	int numSamples = data.getNumPointFrames();

	for (int i = 0; i < numSamples; ++i) {
		osg::Vec3 val = point->getValue(i);
		addPoint(val);
	}
	setName(point->getLabel());
	setValueBaseUnit(point->getUnit());
	setTimeBaseUnit("s");
}

MarkerChannel * MarkerChannel::clone() const
{
	return new MarkerChannel(*this);
}

