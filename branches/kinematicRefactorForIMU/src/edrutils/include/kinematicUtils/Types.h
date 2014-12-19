/********************************************************************
	created:  2014/12/11	08:48:23
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATICUTILS__TYPES_H__
#define __HEADER_GUARD_KINEMATICUTILS__TYPES_H__

#include <array>

namespace kinematicUtils
{
	enum ChannelType
	{
		TX,
		TY,
		TZ,
		RX,
		RY,
		RZ,
		CUSTOM_CHANNEL_BASE = 100
	};

	typedef int Channel;

	typedef std::array<Channel, 3> ChannelTriplet;

	enum AxisOrder
	{
		XYX,
		XYZ,
		XZX,
		XZY,
		YXY,
		YXZ,
		YZX,
		YZY,
		ZXY,
		ZXZ,		
		ZYX,
		ZYZ
	};
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__TYPES_H__