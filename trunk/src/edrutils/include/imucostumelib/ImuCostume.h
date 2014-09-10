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

	//! Klasa obs�uguj�ca kostium
	class IMUCOSTUME_EXPORT Costume : boost::noncopyable
	{
	private:
		//! Wewn�trzna implementacja kostiumu
		class CostumeImpl;

	public:		

		//! Struktura opisuj�ca adres kostiumu w sieci
		struct Address
		{
			//! Adres IP
			std::string ip;
			//! Port
			unsigned int port;
		};

		//! Typ wyliczeniowy obs�ugiwanych czujnik�w przez kostium
		enum SensorType
		{
			IMU_SENSOR,		//! Czujnik IMU
			INSOLE_SENSOR	//! Czujnik INSOLE
		};

		//! Struktura opisuj�ca surowe dane czujnika IMU
		struct ImuRawData
		{
			//! Dane akcelerometru
			osg::Vec3 accelerometer;
			//! Dane �yroskopu
			osg::Vec3 gyroscope;
			//! Dane magnetometru
			osg::Vec3 magnetometer;
		};

		//! Struktura opisuj�ca surowe dane czujnika Insole
		struct InsoleRawData
		{
			//! Dane si�y nacisku na pod�o�e
			std::vector<osg::Vec3> grfs;
		};

		//! Struktura opisuj�ca nasze IMU - dostajemy jeszcze estymacj� orientacji
		struct ImuData : public ImuRawData
		{
			//! Orientacja
			osg::Quat orientation;
		};

		//! Typ identyfikatora sensora
		typedef int8_t SensorID;

		//! Zbi�r identyfikator�w sensor�w
		typedef std::set<SensorID> SensorIDsSet;

		//! Konfiguracja sensor�w kostiumu
		typedef std::map<SensorType, SensorIDsSet> SensorsConfiguration;
		
		//! Struktura opisuj�ca ramk� rozpakowanych danych kostiumu
		struct Frame
		{
			//! Typ identyfikatora czasu ramki
			typedef unsigned long long int Timestamp;

			//! Status odebranych danych z kostiumu dla pojedynczego imu
			enum Status
			{
				NO_FRAME,			//! Brak danych
				COMPLETE_FRAME,		//! Dane dotar�y
				INCOMPLETE_FRAME,	//! Dane niekompletne
				ERROR_FRAME			//! B��dna ramka
			};

			//! Stempel czasowy ramki
			Timestamp timestamp;
			//! Status ramki
			Status status;
			//! Dane z czujnik�w imu
			std::map<SensorID, ImuData> imusData;
			//! Dane z czujnik�w insole
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

		//! \return Konfiguracja sensor�w kostiumu
		const SensorsConfiguration sensorsConfiguration() const;
		//! \param timeout Maksymalny czas oczekiwania na dane [ms] (warto�c 0 oznacza blokowanie w niesko�czono��)
		//! \return Ramka danych kostiumu
		const Frame read(const unsigned int timeout) const;
		//! \param listenTime Czas nas�uchiwania kostium�w [ms]
		//! \return Lista adres�w dost�pnych kostium�w
		static const std::list<Address> availableCostumes(const unsigned int listenTime);

	private:
		//! Wewn�trzna implementacja
		utils::shared_ptr<CostumeImpl> impl_;
	};
}

#endif	//	HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
