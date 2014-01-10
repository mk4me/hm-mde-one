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

	//! Wewnêtrzna implementacja kostiumu
	class CostumeImpl;

	//! Klasa obs³uguj¹ca kostium
	class IMUCOSTUME_EXPORT Costume : boost::noncopyable
	{
	public:

		//! Status odebranych danych z kostiumu dla pojedynczego imu
		enum DataStatus
		{
			NODATA,	//! Brak danych
			DATA	//! Dane dotar³y
		};

		//! Struktura opisuj¹ca surowe dane czujnika
		struct ImuRawData
		{
			//! Dane akcelerometru
			osg::Vec3 accelerometer;
			//! Dane ¿yroskopu
			osg::Vec3 gyroscope;
			//! Dane magnetometru
			osg::Vec3 magnetometer;
		};

		//! Struktura opisuj¹ca nasze IMU - dostajemy jeszcze estymacjê orientacji
		struct ImuData : public ImuRawData
		{
			//! Orientacja
			osg::Quat orientation;
		};

		//! Ostatania paczka danych kostiumu
		typedef std::vector<ImuData> CostumeData;

		//! Status danych dla wszystkich czujników IMU
		typedef std::vector<DataStatus> CostumeStatus;

		//! Struktura opisuj¹ca stan danych i dane kostiumu
		struct CostumePacket
		{
			CostumeData data;
			CostumeStatus status;
		};

	public:
		//! Sta³a definiuj¹ca maksumaln¹ iloœæ czujników na kostiumie
		static const unsigned int MaxIMUsPerCostume = 32;

	public:
		//! \param ip Adres kostiumu
		Costume(const std::string & ip = std::string("127.0.0.1"),
			const unsigned int port = 1234);
		//! Destruktor
		~Costume();		

		//! \return Adres kostiumu
		const std::string & ip() const;
		//! \return Port na którym mamy kostium
		const unsigned int port() const;
		//! \return Iloœæ czujników IMU w kostiumie
		const unsigned int imusNumber() const;
		//! \return Opis odebranych danych kostiumu - odczyty IMU + stan odczytów
		const CostumePacket costumePacket() const;
		//! \return Czy dane gotowe do odbioru
		const bool ready() const;
		//! Metoda czytaj¹ca pojedynczy pakiet danych z kostiumu
		//! \param timeout Czas jaki mamy na odbiór paczki [s]
		void readPacket(const float timeout = 0.05);

	private:
		//! Wewnêtrzna implementacja
		boost::shared_ptr<CostumeImpl> impl_;
	};

}

#endif	//	HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
