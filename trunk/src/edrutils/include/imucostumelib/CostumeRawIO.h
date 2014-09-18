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
#include <utils/SmartPtr.h>
#include <boost/array.hpp>

namespace imuCostume
{
	class IMUCOSTUME_EXPORT CostumeRawIO
	{
	private:

		class CostumeRawIOImpl;

	public:

		//! Rozmiar bufora na dane z kostiumu
		static const uint16_t MaxDataSize = 1024;

		//! Typ surowej ramki danych wysy³anej przez kostium
		typedef boost::array<int8_t, MaxDataSize> Frame;

	public:
		//! \param ip Adres kostiumu
		//! \param port Port na którym kostium s³ucha
		CostumeRawIO(const std::string & ip, const unsigned int port = 1234);
		//! Destruktor
		~CostumeRawIO();

		//! \return Adres kostiumu
		const std::string ip() const;
		//! \return Port na którym mamy kostium
		const unsigned int port() const;

		//! \param frame [out] Miejsce na odebrane dane
		//! \param length [out] D³ugoœæ odebranych danych [B]
		//! \param timeout Dopuszczalny czas oczekiwania na dane [ms] - 0 oznacza nieskoñczonoœæ
		//! \return Prawda jeœli odebrano poprawn¹ ramke danych
		const bool receive(Frame & frame, uint16_t & length, const unsigned int timeout);

		//! \param data Dane do wys³ania
		//! \param length Ile danych wys³aæ z bufora [B]
		//! \param timeout Dopuszczalny czas oczekiwania na dane [ms] - 0 oznacza nieskoñczonoœæ
		//! \return Prawda jeœli wys³ano dane
		const bool send(const void * data, const uint16_t length, const unsigned int timeout);

	private:
		//! Obiekt implementujacy funkcjonalnoœæ kostiumu
		utils::shared_ptr<CostumeRawIOImpl> impl;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__