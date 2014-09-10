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
#include <list>
#include <set>
#include <map>
#include <osg/Vec3>
#include <osg/Quat>
#include <utils/SmartPtr.h>
#include <boost/noncopyable.hpp>

namespace imuCostume
{
	class CostumeRawIO;

	//! Klasa obs³uguj¹ca kostium
	class IMUCOSTUME_EXPORT Costume : boost::noncopyable
	{
	private:
		//! Wewnêtrzna implementacja kostiumu
		class CostumeImpl;

	public:		

		//! Struktura opisuj¹ca adres kostiumu w sieci
		struct Address
		{
			//! Adres IP
			std::string ip;
			//! Port
			unsigned int port;
		};

		//! Typ wyliczeniowy obs³ugiwanych czujników przez kostium
		enum SensorType
		{
			IMU_SENSOR,		//! Czujnik IMU
			INSOLE_SENSOR	//! Czujnik INSOLE
		};

		//! Struktura opisuj¹ca surowe dane czujnika IMU
		struct ImuRawData
		{
			//! Dane akcelerometru
			osg::Vec3 accelerometer;
			//! Dane ¿yroskopu
			osg::Vec3 gyroscope;
			//! Dane magnetometru
			osg::Vec3 magnetometer;
		};

		//! Struktura opisuj¹ca surowe dane czujnika Insole
		struct InsoleRawData
		{
			//! Dane si³y nacisku na pod³o¿e
			std::vector<osg::Vec3> grfs;
		};

		//! Struktura opisuj¹ca nasze IMU - dostajemy jeszcze estymacjê orientacji
		struct ImuData : public ImuRawData
		{
			//! Orientacja
			osg::Quat orientation;
		};

		//! Typ identyfikatora sensora
		typedef int8_t SensorID;

		//! Zbiór identyfikatorów sensorów
		typedef std::set<SensorID> SensorIDsSet;

		//! Konfiguracja sensorów kostiumu
		typedef std::map<SensorType, SensorIDsSet> SensorsConfiguration;
		
		//! Struktura opisuj¹ca ramkê rozpakowanych danych kostiumu
		struct Frame
		{
			//! Typ identyfikatora czasu ramki
			typedef unsigned long long int Timestamp;

			//! Status odebranych danych z kostiumu dla pojedynczego imu
			enum Status
			{
				NO_FRAME,			//! Brak danych
				COMPLETE_FRAME,		//! Dane dotar³y
				INCOMPLETE_FRAME,	//! Dane niekompletne
				ERROR_FRAME			//! B³êdna ramka
			};

			//! Stempel czasowy ramki
			Timestamp timestamp;
			//! Status ramki
			Status status;
			//! Dane z czujników imu
			std::map<SensorID, ImuData> imusData;
			//! Dane z czujników insole
			std::map<SensorID, InsoleRawData> insolesData;
		};		

	public:
		//! \param ip Adres kostiumu
		//! \param port Port kostiumu
		Costume(CostumeRawIO * costume);
		//! Destruktor
		~Costume();

		//! \return Obiekt do niskopoziomowej komunikacji z kostiumem
		CostumeRawIO * costume() const;

		//! \return Konfiguracja sensorów kostiumu
		const SensorsConfiguration sensorsConfiguration() const;
		//! \param timeout Maksymalny czas oczekiwania na dane [ms] (wartoœc 0 oznacza blokowanie w nieskoñczonoœæ)
		//! \return Ramka danych kostiumu
		const Frame read(const unsigned int timeout) const;
		//! \param listenTime Czas nas³uchiwania kostiumów [ms]
		//! \return Lista adresów dostêpnych kostiumów
		static const std::list<Address> availableCostumes(const unsigned int listenTime);

	private:
		//! Wewnêtrzna implementacja
		utils::shared_ptr<CostumeImpl> impl_;
	};
}

#endif	//	HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
