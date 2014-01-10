/********************************************************************
    created:  2014/01/09
    created:  9:1:2014   8:32
    filename: ImuCostume.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
#define HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__

#include <imucostumelib/Export.h>

#include <string>
#include <vector>
#include <osg/Vec3>
#include <osg/Quat>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace imuCostume
{

	//! Wewn�trzna implementacja kostiumu
	class CostumeImpl;

	//! Klasa obs�uguj�ca kostium
	class IMUCOSTUME_EXPORT Costume : boost::noncopyable
	{
	public:

		//! Status odebranych danych z kostiumu dla pojedynczego imu
		enum DataStatus
		{
			NODATA,	//! Brak danych
			DATA	//! Dane dotar�y
		};

		//! Struktura opisuj�ca surowe dane czujnika
		struct ImuRawData
		{
			//! Dane akcelerometru
			osg::Vec3 accelerometer;
			//! Dane �yroskopu
			osg::Vec3 gyroscope;
			//! Dane magnetometru
			osg::Vec3 magnetometer;
		};

		//! Struktura opisuj�ca nasze IMU - dostajemy jeszcze estymacj� orientacji
		struct ImuData : public ImuRawData
		{
			//! Orientacja
			osg::Quat orientation;
		};

		//! Ostatania paczka danych kostiumu
		typedef std::vector<ImuData> CostumeData;

		//! Status danych dla wszystkich czujnik�w IMU
		typedef std::vector<DataStatus> CostumeStatus;

		//! Struktura opisuj�ca stan danych i dane kostiumu
		struct CostumePacket
		{
			CostumeData data;
			CostumeStatus status;
		};

	public:
		//! Sta�a definiuj�ca maksumaln� ilo�� czujnik�w na kostiumie
		static const unsigned int MaxIMUsPerCostume = 32;

	public:
		//! \param ip Adres kostiumu
		Costume(const std::string & ip = std::string("127.0.0.1"),
			const unsigned int port = 1234);
		//! Destruktor
		~Costume();		

		//! \return Adres kostiumu
		const std::string & ip() const;
		//! \return Port na kt�rym mamy kostium
		const unsigned int port() const;
		//! \return Ilo�� czujnik�w IMU w kostiumie
		const unsigned int imusNumber() const;
		//! \return Opis odebranych danych kostiumu - odczyty IMU + stan odczyt�w
		const CostumePacket costumePacket() const;
		//! \return Czy dane gotowe do odbioru
		const bool ready() const;
		//! Metoda czytaj�ca pojedynczy pakiet danych z kostiumu
		//! \param timeout Czas jaki mamy na odbi�r paczki [s]
		void readPacket(const float timeout = 0.05);

	private:
		//! Wewn�trzna implementacja
		boost::shared_ptr<CostumeImpl> impl_;
	};

}

#endif	//	HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
