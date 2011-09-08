#include <plugins/c3d/C3DChannels.h>


C3DAnalogChannel::C3DAnalogChannel( int samplesPerSec ) :
ScalarChannel(samplesPerSec)
{}

C3DAnalogChannel::C3DAnalogChannel( const C3DAnalogChannel& channel ) :
ScalarChannel(channel)
{}

C3DAnalogChannel::C3DAnalogChannel( const c3dlib::C3DParser& c3dData, int channelNo ) :
ScalarChannel( static_cast<int>(c3dData.getNumberAnalogSamplePerFrame() * c3dData.getPointFrequency()) )
{
    if ( c3dData.getNumAnalogFrames() == 0) {
        throw std::runtime_error("Incorrect number of frames.");
    }

	c3dlib::C3DParser::IAnalogConstPtr analog = c3dData.getAnalog(channelNo);

	int numSamples = c3dData.getNumAnalogFrames();
	for (int i = 0; i < numSamples; i++) {
		addPoint(static_cast<float>(analog->getValue(i)));
	}
		
    setName(analog->getLabel());
    setValueBaseUnit(analog->getUnit());
}



MarkerChannel::MarkerChannel(const c3dlib::C3DParser& data, int channelNo ) :
VectorChannel( data.getPointFrequency())
{
	c3dlib::C3DParser::IPointConstPtr point = data.getPoint(channelNo);

	int numSamples = data.getNumPointFrames();

	for (int i = 0; i < numSamples; i++) {
		addPoint(point->getValue(i));
	}
	setName(point->getLabel());
}

GRFChannel::GRFChannel( const c3dlib::C3DParser& data, int channelNo ) :
	VectorChannel(static_cast<int>(data.getNumberAnalogSamplePerFrame() * data.getPointFrequency())),
	type(Unknown)
{
	if ( data.getNumAnalogFrames() == 0) {
		throw std::runtime_error("Incorrect number of frames.");
	}

	c3dlib::C3DParser::IAnalogConstPtr x = data.getAnalog(3 * channelNo + 0);
	c3dlib::C3DParser::IAnalogConstPtr y = data.getAnalog(3 * channelNo + 1);
	c3dlib::C3DParser::IAnalogConstPtr z = data.getAnalog(3 * channelNo + 2);


	int numSamples = data.getNumAnalogFrames();
	for (int i = 0; i < numSamples; i++) {
		addPoint(osg::Vec3(x->getValue(i), y->getValue(i), z->getValue(i)));
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

	switch (channelNo) {
		case 0:		type = F1;		break;
		case 1:		type = M1;		break;
		case 2:		type = F2;		break;
		case 3:		type = M2;		break;
		default:	type = Unknown; break;
	}
}
