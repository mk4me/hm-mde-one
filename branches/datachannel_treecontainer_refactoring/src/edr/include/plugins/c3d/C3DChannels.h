/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   14:06
	filename: C3DChannels.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_C3D__C3DCHANNELS_H__
#define __HEADER_GUARD_C3D__C3DCHANNELS_H__

#include <c3dlib/C3DTypes.h>
#include <plugins/newChart/Wrappers.h>

DEFINE_WRAPPER(c3dlib::VectorChannelReaderInterface, utils::PtrPolicyStd, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER_INHERITANCE(c3dlib::VectorChannel, c3dlib::VectorChannelReaderInterface);
DEFINE_WRAPPER_INHERITANCE(c3dlib::MarkerChannel, c3dlib::VectorChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::ForceChannel, c3dlib::VectorChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::MomentChannel,c3dlib::VectorChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::AngleChannel, c3dlib::VectorChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::PowerChannel, c3dlib::VectorChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::ScalarChannel, c3dlib::ScalarChannelReader);
DEFINE_WRAPPER_INHERITANCE(c3dlib::VectorToScalarAdaptor, c3dlib::ScalarChannelReader);
DEFINE_WRAPPER_INHERITANCE(c3dlib::C3DAnalogChannel, c3dlib::ScalarChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::EMGChannel, c3dlib::C3DAnalogChannel);
DEFINE_WRAPPER_INHERITANCE(c3dlib::GRFChannel, c3dlib::VectorChannel);

#endif  // __HEADER_GUARD_CORE__C3DCHANNELS_H__
