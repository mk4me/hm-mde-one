#include "CorePCH.h"
#include <boost/algorithm/string.hpp>
#include <core/C3DChannels.h>

static const double scale = 0.004;

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

C3DAnalogChannel::C3DAnalogChannel( int samplesPerSec ) :
ScalarChannel(samplesPerSec)
{}

C3DAnalogChannel::C3DAnalogChannel( const C3DAnalogChannel& channel ) :
ScalarChannel(channel)
{}

C3DAnalogChannel::C3DAnalogChannel( const C3D_Data& c3dData, int channelNo ) :
ScalarChannel( c3dData.getHeader()->getNumberOfAnalogSamplesPerFrame()* c3dData.getHeader()->getFrameRate() )
{
    // TODO: kto do diab³a tworzy strukturê "Data" w globalnym namespace?!
    const ::Data* data = c3dData.getData();
    const ::MarkerHeader* header = c3dData.getHeader();

    if ( data->getNumberOfFrames() * header->getNumberOfAnalogSamplesPerFrame() == 0 ) {
        throw std::runtime_error("Incorrect number of frames.");
    }

    // wczytanie próbek
    // liczba wszystkich próbek... próbki dla kana³ów umieszczone s¹ obok siebie, a z nich komponowane s¹ ramki
    int numSamples = data->getNumberOfFrames() * header->getNumberOfAnalogSamplesPerFrame() * data->getAnalogChannelsNumber();
    const float* sample = data->getAnalog() + channelNo;
    const float* lastSample = data->getAnalog() + numSamples;
    int delta = data->getAnalogChannelsNumber();
    for ( ; sample < lastSample; sample += delta ) {
        addPoint(*sample);
    }

    const ::GroupData* analogGroup = c3dData.getParameters()->getGroup("ANALOG");

    //setScale(analogGroup->getParameter("SCALE")->getDataAsFloatArray()[channelNo]);
    //setOffset(analogGroup->getParameter("OFFSET")->getDataAsFloatArray()[channelNo]);
    //setDescription(analogGroup->getParameter("DESCRIPTIONS")->getDataAsStringsArray(channelNo));

    setName(boost::trim_copy(analogGroup->getParameter("LABELS")->getDataAsStringsArray(channelNo)));
    setYUnit(analogGroup->getParameter("UNITS")->getDataAsStringsArray(channelNo));
    setXUnit("s");
    normalize();
}



MarkerChannel::MarkerChannel( C3D_Data& c3dData, int channelNo ) :
VectorChannel( c3dData.getHeader()->getFrameRate())
{
    ::Data* data = c3dData.getData();
    ::MarkerHeader* header = c3dData.getHeader();
    
    int framesNo = data->getNumberOfFrames();
    int markersNo = data->getHeader()->getNumberOfC3DPoints();
    int firstFrame = data->getHeader()->getFirstDataFrame();
    int lastFrame = data->getHeader()->getLastDataFrame();

    osg::Vec3 v;
    float* table = data->getData();
    for(int i=0; i < framesNo; i++) {
        v[0] = table[i * markersNo * 4 + channelNo * 4 + 0] * scale;
        v[1] = table[i * markersNo * 4 + channelNo * 4 + 1] * scale;
        v[2] = table[i * markersNo * 4 + channelNo * 4 + 2] * scale;
        addPoint(v);
    }
    const ::GroupData* videoGroup = c3dData.getParameters()->getGroup("POINT");
    setName(boost::trim_copy(videoGroup->getParameter("LABELS")->getDataAsStringsArray(channelNo)));
    // wczytanie próbek
    // liczba wszystkich próbek... próbki dla kana³ów umieszczone s¹ obok siebie, a z nich komponowane s¹ ramki
    //int numSamples = data->getNumberOfFrames() * header->getNumberOfAnalogSamplesPerFrame() * data->getAnalogChannelsNumber();
    //const float* sample = data->getAnalog() + channelNo;
    //const float* lastSample = data->getAnalog() + numSamples;
    //int delta = data->getAnalogChannelsNumber();
    //for ( ; sample < lastSample; sample += delta ) {
    //    addPoint(*sample);
    //}

    //const ::GroupData* analogGroup = c3dData.getParameters()->getGroup("ANALOG");

    ////setScale(analogGroup->getParameter("SCALE")->getDataAsFloatArray()[channelNo]);
    ////setOffset(analogGroup->getParameter("OFFSET")->getDataAsFloatArray()[channelNo]);
    ////setDescription(analogGroup->getParameter("DESCRIPTIONS")->getDataAsStringsArray(channelNo));

    //setName(boost::trim_copy(analogGroup->getParameter("LABELS")->getDataAsStringsArray(channelNo)));
    //setYUnit(analogGroup->getParameter("UNITS")->getDataAsStringsArray(channelNo));
    //setXUnit("s");
    //normalize();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////