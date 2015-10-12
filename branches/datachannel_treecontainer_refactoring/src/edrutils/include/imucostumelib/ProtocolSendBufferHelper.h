/********************************************************************
	created:  2014/11/19	10:52:11
	filename: ProtocolSendBufferHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__PROTOCOLSENDBUFFERHELPER_H__
#define __HEADER_GUARD_IMU_COSTUME__PROTOCOLSENDBUFFERHELPER_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CANopenFrame.h>
#include <vector>

namespace imuCostume
{
	//! Klasa pomocnicza przy tworzniu bufora do wys�ania na kostium
	class IMUCOSTUME_EXPORT ProtocolSendBufferHelper
	{
	public:

		//! Struktura opisuj�ca dynamiczny bufor danych
		struct IMUCOSTUME_EXPORT Buffer
		{
			//! \param bufor Dynamicznie zaalokowane dane
			utils::shared_array<uint8_t> buffer;
			//! \param length Rozmiar bufora danych
			uint16_t length;

			Buffer(Buffer && Other);
			Buffer() = default;
			Buffer(const Buffer& other) = default;

			Buffer& operator=(Buffer&& Other);
			Buffer& operator=(const Buffer& other) = default;
		};

	public:

		//! Konstruktor domy�lny
		ProtocolSendBufferHelper();
		//! Konstruktor kopiuj�cy
		//! \param Other Obiekt kopiowany
		ProtocolSendBufferHelper(const ProtocolSendBufferHelper & Other);
		//! Konstruktor przenosz�cy w�asno��
		//! \param Other Obiekt przejmowany
		ProtocolSendBufferHelper(ProtocolSendBufferHelper && Other);

		//! \tpara Typ iteratora pozwalaj�cego wy�uska� CANopenFrame
		//! \param first Iterator pierwszego elementu
		//! \param last Iterator za ostatnim elementem
		template<class Iter>
		ProtocolSendBufferHelper(Iter first, Iter last)
		{
			while (first != last){
				add(*first);
				++first;
			}
		}

		//! \param frame Ramka do spakowania przed wys�aniem
		void add(const CANopenFrame & frame);
		//! \param frame Ramka do spakowania przed wys�aniem
		//! \param length D�ugo�c ramki
		void add(const CANopenFrame & frame, const uint8_t length);
		//! \return Czy zbi�r ramek pusty
		const bool empty() const;
		//! \return Upakowane ramki CANopen, gotowe do wsy�ania w jednym pakiecie,
		//! powoduje ich usuni�cie z lokalnej listy
		Buffer createBuffer();
		//! \param frame [out] Ramka danych rozszerzana o d�ugo� pola danych
		//! \param length D�ugo�c ca�ej ramki [B]
		static void updateFrameLength(CANopenFrame & frame, const uint8_t length = 10);

	private:
		//! Lista ramek do spakowania przed wys�aniem
		std::list<CANopenFrame> frames;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__PROTOCOLSENDBUFFERHELPER_H__
