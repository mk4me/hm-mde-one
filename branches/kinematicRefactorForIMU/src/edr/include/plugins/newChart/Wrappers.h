/********************************************************************
	created:  2014/01/22
	created:  22:1:2014   9:34
	filename: Wrappers.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_NEW_CHART__WRAPPERS_H__
#define HEADER_GUARD_NEW_CHART__WRAPPERS_H__

#include <utils/PtrPolicyStd.h>
#include <utils/ObjectWrapper.h>
#include <utils/DataChannelDescriptors.h>
#include <threadingUtils/StreamData.h>

typedef utils::IChannelReader<float, float, true> ScalarChannelReader;
typedef threadingUtils::IStreamT<float> ScalarStream;

DEFINE_WRAPPER(ScalarChannelReader, utils::PtrPolicyStd, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(ScalarStream, utils::PtrPolicyStd, utils::ClonePolicyForbidden);

#endif	//	HEADER_GUARD_NEW_CHART__WRAPPERS_H__
