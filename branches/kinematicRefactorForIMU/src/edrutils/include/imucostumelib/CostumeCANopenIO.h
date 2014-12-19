/********************************************************************
	created:  2014/09/03	18:39:36
	filename: CostumeCANopenIO.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__
#define __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CANopenFrame.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <list>

namespace imuCostume
{
	//! Klasa obs�uguj�ca protoku� kostiumu (odczyt danych z CANopen i zapis danych na CANopen)
	struct IMUCOSTUME_EXPORT CostumeCANopenIO
	{
	public:

		//! Struktura opisuj�ca struktur� nag��wka protoko�u
		struct HeaderStructure
		{
			//! Typ ramki
			uint8_t type;
			//! Numer sekwencyjny ramki
			uint8_t sequenceNumber;
		};

		//! Unia realizuj�ca r�ne reprezentacje nag��wka
		union Header
		{
			//! Forma bufora
			std::array<uint8_t, 2> buffer;
			//! Forma poszczeg�lnych p�l
			HeaderStructure structure;
			//! Zwi�z�a reprezentacja
			uint16_t value;
		};

		//! Struktura opisuj�ca struktur� ca�ej ramki
		struct FrameStructure
		{
			//! Nag��wek
			Header header;
			//! Sekcja danych
			std::array<uint8_t, CostumeRawIO::MaxDataSize - 2> data;
		};

		//! Unia realizuj�ca r�ne reprezentacje ramki
		union Frame
		{
			//! Forma bufora
			CostumeRawIO::Buffer buffer;
			//! Forma poszczeg�lnych p�l
			FrameStructure structure;
		};

		//! Typ identyfikatora czasu ramki
		typedef uint32_t Timestamp;

		//! Struktura opisuj�ca wypakowane dane z protoko�u
		struct IMUCOSTUME_EXPORT Data
		{
			//! Znacnik czasowy
			Timestamp timestamp;
			//! Ramki CANopen
			std::list<CANopenFrame> CANopenFrames;
		};

	public:

		//! \param seqNumber Numer zawarty w komunikacie, powinien by� r�wnie� zawarty w odpowiadaj�cej wiadomo�ci zwrotnej
		//! \param data Dane do wys�ania - ramka powinna by� poprawn� zgodnie ze specyfikacj� protoko�u komunikacyjnego
		//! \param length Rozmiar ramki CANopen [B]
		//! \param timeout Timeout dla wysy�ania danych
		//! \param costume Kostium dla kt�rego wysy�amy dane na CANopen
		//! \return Czy transmisja si� powiod�a (tylko transfer danych do kostiumu a nie na CANopen)
		static const bool send(const uint8_t seqNumber, const void * data,
			const uint16_t length, const uint16_t timeout, CostumeRawIO & costume);

		//! \param seqNumber Numer zawarty w komunikacie, powinien by� r�wnie� zawarty w odpowiadaj�cej wiadomo�ci zwrotnej
		//! \param data Dane do wys�ania - ramka powinna by� poprawn� zgodnie ze specyfikacj� protoko�u komunikacyjnego
		//! \param length Rozmiar ramki CANopen [B]
		//! \return Czy odebrany komunikat jest potwierdzeniem odbioru
		static const bool verifyReceiveConfirmation(const uint8_t seqNumber,
			const void * data, const uint16_t length);

		//! \param data Dane wyci�gni�te z naszego prtoko�u
		//! \param length D�ugo�� danych [B]
		//! \return Wypakowane dane
		static Data extractData(const void * data, const uint16_t length);

		//! Metoda przygotowuje kostium do dzia�ania (w��cza wszystkie sensory w tryb operational przez NMT)
		//! \param sendBuffer Bufor w kt�rym umieszczamy sformatowane ramki CANopen zgodnie z protoko�em
		static void prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer);
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__