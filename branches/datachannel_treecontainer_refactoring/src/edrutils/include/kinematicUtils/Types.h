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
	//! Struktura opisuje kana� danych dla ruchu
	struct ChannelType
	{
		//! Typ wyliczeniowy kana�u
		enum Type
		{
			TX = 0,	//! Translacja wzd�� osi X
			TY = 1, //! Translacja wzd�� osi Y
			TZ = 2, //! Translacja wzd�� osi Z
			RX = 3, //! Rotacja wok� osi X
			RY = 4, //! Rotacja wok� osi Y
			RZ = 5, //! Rotacja wok� osi Z
			CUSTOM_CHANNEL_BASE = 6 //! Offset dla customowych kana��w
		};

		//! \param type Typ kana�u
		//! \return Nazwa kana�u w formie napisu
		static std::string toString(const Type type);
		//! \param name Nazwa kana�u w formie napisu
		//! \return Typ kana�u
		static Type fromString(const std::string & name);
	};

	//! Typ okre�laj�cy kana�
	typedef int Channel;
	//! Typ okre�laj�cy 3 kana�y
	typedef std::array<Channel, 3> ChannelTriplet;

	//! Struktura opisuj�ca kolejno�� rotacji osi
	struct AxisOrder
	{
		//! Typ wyliczeniowy opisuj�cy kolejno�� rotacji osi
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

		//! \param axisOrder Kolejno�� rotacji osi
		//! \return Napis okre�laj�cy kolejno�� osi
		static std::string toString(const AxisOrder::Type axisOrder);
		//! \param axisOrderName Napis okre�laj�cy kolejno�c rotacji osi 
		//! \return Kolejno�� rotacji osi
		static Type fromString(const std::string & axisOrderName);
		//! \param channelTriplet Tr�jka kana��w
		//! \return Kolejno�� rotacji osi
		static Type fromChannelTriplet(const ChannelTriplet & channelTriplet);
		//! \param axisOrder Kolejno�� rotacji osi
		//! \return Tr�jka z kana�ami
		static ChannelTriplet toChannelTriplet(const Type axisOrder);
	};
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__TYPES_H__