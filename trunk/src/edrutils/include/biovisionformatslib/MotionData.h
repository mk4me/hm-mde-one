/********************************************************************
	created:  2014/12/10	22:57:09
	filename: MotionData.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_BIOVISIONFORMATS__MOTIONDATA_H__
#define __HEADER_GUARD_BIOVISIONFORMATS__MOTIONDATA_H__

#include <vector>

namespace biovision
{
	struct MotionData
	{
		typedef std::vector<float> ChannelData;
		typedef std::vector<ChannelData> FrameJointData;
		typedef std::vector<FrameJointData> Frames;

		float frameTime;
		Frames frames;
	};
}

#endif	// __HEADER_GUARD_BIOVISIONFORMATS__MOTIONDATA_H__