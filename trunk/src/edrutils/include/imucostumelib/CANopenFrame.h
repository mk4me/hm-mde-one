/********************************************************************
	created:  2014/11/11	18:22:41
	filename: CANopenFrame.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__CANOPENFRAME_H__
#define __HEADER_GUARD_IMU_COSTUME__CANOPENFRAME_H__

#include <array>
#include <cinttypes>

namespace imuCostume
{
	//! Unia reprezentuj�ca ramk� CANopen
	//! UWAGA - wra�liwa na reprezentacj� danych!! (LittleEndian i BigEndian)
	union CANopenFrame
	{
	public:
		struct SizeLimits {
			//! Minimalny rozmiar ramki [B]
			static const uint8_t MinSize = 2;
			//! Maksymalny rozmiar ramki [B]
			static const uint8_t MaxSize = 10;
			//! Rozmiar pola identyfokatora wiadomo�ci [B]
			static const uint8_t COBIDSize = 2;
			//! Maxymalny rozmiar pola danych [B]
			static const uint8_t MaxDataSize = 8;
		};

	public:

		//! Typ opisuj�cy sekcj� danych protoko�u komunikacyjnego
		using Buffer = std::array<uint8_t, SizeLimits::MaxSize>;
		using Data = std::array<uint8_t, SizeLimits::MaxDataSize>;

		//! Identyfikator wiadomo�ci
		union COBID
		{
			//! Ca�y identyfikator
			uint16_t value;
			//! Identyfikator rozdzielony na poszczeg�lne bajty
			std::array<uint8_t, SizeLimits::COBIDSize> buffer;
			//! Identyfikator funkcji
			inline uint8_t functionID() const { return (buffer[1] >> 7) & 0x0F; };
			//! Identyfikator w�z�a
			inline uint8_t nodeID() const { return buffer[0] & 0x7F; };
		};

		//! Struktura opisuj�ca ramk� CANopen z podzia�em na COB-ID i dane
		struct FrameStructure
		{
			//! COBID ramki
			COBID cobID;
			//! Sekcja danych
			Data data;
		};

	public:

		//! Cala ramka CANopen jako tablica bajt�w
		Buffer buffer;
		//! Struktura ramki z podzia�em na pole COB-ID i dane
		FrameStructure structure;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__CANOPENFRAME_H__
