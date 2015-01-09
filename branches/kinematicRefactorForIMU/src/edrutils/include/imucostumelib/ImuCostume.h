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
#include <list>
#include <set>
#include <map>
#include <osg/Vec3>
#include <osg/Quat>
#include <utils/SmartPtr.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>

namespace imuCostume
{
	//! Klasa obs�uguj�ca kostium
	class IMUCOSTUME_EXPORT Costume
	{
	public:

		//! Typ identyfikatora sensora
		typedef uint8_t SensorID;

		//! Typ wyliczeniowy obs�ugiwanych czujnik�w przez kostium
		enum SensorType
		{
			UNKNOWN = -1,	//! Nieznany typ
			IMU = 1,		//! Czujnik IMU
			INSOLE = 2		//! Czujnik INSOLE
		};

		//! Generyczny sensor - baza dla pozosta�ych
		class IMUCOSTUME_EXPORT GenericSensor
		{
		protected:
			//! \param id Identyfikator CANopen sensora
			//! \param type Typ sensora
			GenericSensor(const SensorID id, const int type);

		public:
			//! Destruktor wirtualny
			virtual ~GenericSensor();
			//! \return Identyfikator czujnika
			const SensorID id() const;
			//! \return Typ czujnika
			const int type() const;

		private:
			//! Identyfikator w�z�a
			const SensorID id_;
			//! Typ czujnika
			const int type_;
		};

		typedef utils::shared_ptr<GenericSensor> SensorDataPtr;

		//! Klasa opisuj�ca dane czujnik�w IMU
		class IMUCOSTUME_EXPORT IMUSensor : public GenericSensor
		{
		public:

			//! Flagi statusu danych - czy poszczeg�lne dane s� dostepne do odczytu w aktualnej ramce
			enum DataStatus
			{
				ACC_DATA = 0x01,	//! Akcelerometr
				GYRO_DATA = 0x02,	//! Gyroskop
				MAG_DATA = 0x04,	//! Magnetometer
				ORIENT_DATA = 0x08	//! Orientacja
			};

		public:

			//! \param id Identyfikator CANopen sensora
			//! \param type Typ sensora
			//! \param accelerometer
			//! \param magnetometer
			//! \param gyroscope
			//! \param orientation
			IMUSensor(const SensorID id, const int dataStatus,
				const osg::Vec3 & accelerometer, const osg::Vec3 & magnetometer,
				const osg::Vec3 & gyroscope, const osg::Quat & orientation);

			//! Destruktor wirtualny
			virtual ~IMUSensor();

			//! \return Dane akcelerometru 
			const osg::Vec3 & accelerometer() const;
			//! \return Dane magnetometru 
			const osg::Vec3 & magnetometer() const;
			//! \return Dane �yroskopu 
			const osg::Vec3 & gyroscope() const;
			//! \return Estymowana orientacja na czujniku
			const osg::Quat & orientation() const;
			//! \return Status danych
			const int dataStatus() const;

		private:
			const int dataStatus_;
			const osg::Vec3 accelerometer_;
			const osg::Vec3 magnetometer_;
			const osg::Vec3 gyroscope_;
			const osg::Quat orientation_;
		};

		//! Klasa opisuj�ca dane czujnik�w INSOLE
		class IMUCOSTUME_EXPORT INSOLESensor : public GenericSensor
		{
		public:

			//! Struktura danych czujnika
			struct INSOLEData
			{
				//! Identyfikator punktu nacisku
				unsigned int id;
				//! Wektor si�y
				osg::Vec3 grf;
			};

			//! Lista punkt�w nacisku
			typedef std::list<INSOLEData> INSOLESData;

		public:

			//! \param id Identyfikator sensora
			//! \param insolesData Dane punkt�w naciskowych
			INSOLESensor(const SensorID id, const INSOLESData & insolesData);

			//! Destruktor wirtualny
			virtual ~INSOLESensor();
			//! \return Dane punkt�w naciskowych
			const INSOLESData & insolesData() const;

		private:
			//! Dane punkt�w naciskowych
			const INSOLESData insolesData_;
		};		

		//! Zbi�r identyfikator�w sensor�w
		typedef std::set<SensorID> SensorIDsSet;

		//! Konfiguracja sensor�w kostiumu
		typedef std::map<SensorType, SensorIDsSet> SensorsConfiguration;

		//! Struktura opisuj�ca rozpakowane dane sensor�w
		struct IMUCOSTUME_EXPORT Data
		{
			//! Znacznik czasu ramki
			CostumeCANopenIO::Timestamp timestamp;
			//! Dane sensor�w
			std::list<SensorDataPtr> sensorsData;
		};

	public:

		//! \param data Wska�nik bufora danych do analizy z surowymi ramkami
		//! \param length Rozmiar bufora z danymi [B]
		//! \return Konfiguracja sensor�w kostiumu
		static SensorsConfiguration sensorsConfiguration(const void * data, const uint16_t length);
		//! \param data Surowe dane CANopen
		//! \return Dane sensor�w
		static std::list<SensorDataPtr> extractSensorsData(const std::list<CANopenFrame> & data);
		//! \param data Wska�nik bufora danych do analizy z surowymi ramkami
		//! \param length Rozmiar bufora z danymi [B]
		//! \return Dane sensor�w
		static std::list<SensorDataPtr> extractSensorsData(const void * buffer, const uint16_t length);
		//! Metoda przygotowuje kostium do dzia�ania - wy��cza komunikaty heartbeat,
		//! przestawia TPDO w trym asynchroniczny (zrzut z danych niezale�nie od dost�pno�ci pozosta�ych
		//! czujnik�w + brak zrzucania warto�ci domy�lnych)
		//! \param sendBuffer Bufor danych do wysy�ki
		static void prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer);
		//! To samo co poprzednia metoda ale konfigurujemy ju� tylko specyficzne sensory a nie wszystkie
		static void prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer,
			const SensorsConfiguration & sensorsConfiguration);
		//! \param data Dane rozpakowane z protoko�u (w formie ramek CANopen)
		//! \return Rozpakowane dane sensor�w
		static Data convert(const CostumeCANopenIO::Data & data);
	};
}

#endif	//	HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
