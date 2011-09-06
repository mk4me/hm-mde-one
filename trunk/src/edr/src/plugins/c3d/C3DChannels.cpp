//#include "C3DPCH.h"
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
