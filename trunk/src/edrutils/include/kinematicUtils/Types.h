/********************************************************************
	created:  2014/12/11	08:48:23
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATICUTILS__TYPES_H__
#define __HEADER_GUARD_KINEMATICUTILS__TYPES_H__

#include <array>
#include <string>

namespace kinematicUtils
{
	struct ChannelType
	{
		enum Type
		{
			TX,
			TY,
			TZ,
			RX,
			RY,
			RZ,
			CUSTOM_CHANNEL_BASE = 100
		};

		static std::string toString(const Type type);
		static Type fromString(const std::string & name);
	};

	typedef int Channel;

	typedef std::array<Channel, 3> ChannelTriplet;

	struct AxisOrder
	{
		enum Type
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

		static std::string toString(const AxisOrder::Type axisOrder);

		static Type fromString(const std::string & axisOrderName);

		static Type fromChannelTriplet(const ChannelTriplet & channelTriplet);

		static ChannelTriplet toChannelTriplet(const Type axisOrder);
	};
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__TYPES_H__