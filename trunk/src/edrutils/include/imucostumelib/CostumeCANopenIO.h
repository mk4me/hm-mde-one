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

namespace imuCostume
{
	struct IMUCOSTUME_EXPORT CostumeCANopenIO
	{
	public:

		//! Typ opisuj�cy ramke CANopen
		struct Frame
		{
			//! Minimalny rozmiar ramki [B]
			static const int8_t MinFrameSize = 2;
			//! Maksymalny rozmiar ramki [B]
			static const int8_t MaxFrameSize = 10;
			//! Rozmiar pola identyfokatora wiadomo�ci [B]
			static const int8_t MessageIDFieldSize = 2;
			//! Maxymalny rozmiar pola danych [B]
			static const int8_t MaxDataFieldSize = 8;

			//! Identyfikator wiadomo�ci
			union MessageID
			{
				//! Ca�y identyfikator
				int16_t id;
				//! Identyfikator rozdzielony na poszczeg�lne bajty
				int8_t bytes[MessageIDFieldSize];
			};

			//! Opis struktury ramki
			union Structure
			{
				//! Pe�na ramka
				int8_t frame[MaxFrameSize];
				//! Sekcja identyfikatora wiadomo�ci
				MessageID messageID;
				//! Sekcja danych
				int8_t data[MaxDataFieldSize];
			};

			//! Struktura ramki
			Structure structure;
		};

	public:
		//! \param seqNumber Numer zawarty w komunikacie, powinien by� r�wnie� zawarty w odpowiadaj�cej wiadomo�ci zwrotnej
		//! \param canOpenFrame Dane do wys�ania - ramka powinna by� poprawn� dla CANopen
		//! \param length Rozmiar ramki CANopen [B]
		//! \param timeout Timeout dla wysy�ania danych
		//! \param costume Kostium dla kt�rego wysy�amy dane na CANopen
		//! \return Czy transmisja si� powiod�a (tylko transfer danych do kostiumu a nie na CANopen)
		static const bool send(const int8_t seqNumber, const Frame & canOpenFrame,
			const uint8_t length, const unsigned int timeout, CostumeRawIO & costume);
		//! \param seqNumber Numer jaki powinien znale�� si� w odbieranym komunikacie
		//! \param canOpenFrame [out] Bufor dla odbieranych danych z CANopen
		//! \param length [out] Rozmiar odebranych danych [B]
		//! \param timeout Timeout dla odbioru danych
		//! \param costume Kostium dla kt�rego wysy�amy dane na CANopen
		//! \return Czy transmisja si� powiod�a (tylko transfer danych z kostiumu)
		static const bool receive(const int8_t seqNumber, Frame & canOpenFrame,
			unsigned int & length, const unsigned int timeout, CostumeRawIO & costume);
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__