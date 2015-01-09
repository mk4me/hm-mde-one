/********************************************************************
	created:  2014/09/03	09:09:11
	filename: CostumeRawIO.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__
#define __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__

#include <imucostumelib/Export.h>
#include <cstdint>
#include <string>
#include <list>
#include <utils/SmartPtr.h>
#include <array>

namespace imuCostume
{
	//! Klasa realizuj�ca surow� komunikacj� z kostiumem po UDP,
	//! pozwalaj�ca wykry� dost�pne kostiumu i skonfigurowa� ich
	//! cz�stotliwo�� pr�bkowania danych
	class IMUCOSTUME_EXPORT CostumeRawIO
	{
	private:
		//! Forward declaration realizacji
		class CostumeRawIOImpl;	

	public:

		//! Rozmiar bufora na dane z kostiumu
		static const uint16_t MaxDataSize = 1024;

		//! Typ surowej ramki danych wysy�anej przez kostium
		typedef std::array<uint8_t, MaxDataSize> Buffer;		

		//! Struktura identyfikuj�ca kostium w sieci
		struct CostumeAddress
		{
			std::string ip;		//! Adres IP
			unsigned int port;	//! Port
		};	

	public:
		//! \param ip Adres kostiumu
		//! \param port Port na kt�rym kostium s�ucha
		CostumeRawIO(const std::string & ip, const unsigned int port = 1234);
		//! \param Other Kopiowany kostium
		CostumeRawIO(const CostumeRawIO & Other);
		//! Destruktor
		~CostumeRawIO();

		//! \return Adres kostiumu
		const std::string & ip() const;
		//! \return Port na kt�rym mamy kostium
		const unsigned int port() const;

		//! \param buffer [out] Miejsce na odebrane dane
		//! \param length [out] D�ugo�� odebranych danych [B]
		//! \param timeout Dopuszczalny czas oczekiwania na dane [ms] - 0 oznacza niesko�czono��
		//! \return Prawda je�li odebrano poprawn� ramke danych
		const bool receive(Buffer & buffer, uint16_t & length, const uint16_t timeout = 0);

		//! \param data Dane do wys�ania
		//! \param length Ile danych wys�a� z bufora [B]
		//! \param timeout Dopuszczalny czas oczekiwania na dane [ms] - 0 oznacza niesko�czono��
		//! \return Prawda je�li wys�ano dane
		const bool send(const void * data, const uint16_t length, const uint16_t timeout = 0);

		//! \return Lista dost�pnych kostium�w
		static std::list<CostumeAddress> listAvailableCostumes();

		//! \param delay Czas przerwy pomiedzy probkami [ms]
		//! \param timeout Czas na wys�anie [ms]
		void setSamplingDelay(const uint32_t delay, const uint16_t timeout = 0);

	private:
		//! Obiekt implementujacy funkcjonalno�� kostiumu
		utils::shared_ptr<CostumeRawIOImpl> impl;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__