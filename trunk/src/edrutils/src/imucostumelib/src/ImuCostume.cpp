#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CostumeRawIO.h>

using namespace imuCostume;

//! Sta³a definiuj¹ca iloœæ czujników IMU na kostiumie
static const unsigned int IMUS_IN_COSTUME_COUNT = 18;
//! Sta³a definiuj¹ca iloœæ czujników insole na kostiumie
static const unsigned int INSOLS_IN_COSTUME_COUNT = 2;
//! Sta³a definiuj¹ca iloœæ sensorów w ramach jednego insole
static const unsigned int SENSORS_COUNT_IN_INSOL = 4;

//! Implementacja funkcjonalnoœci kostiumu
class Costume::CostumeImpl
{
public:
	CostumeImpl(CostumeRawIO * costume) : rawIO(costume)
	{
	}

	//! Destruktor
	~CostumeImpl()
	{
	}

	CostumeRawIO * costume() const
	{
		return rawIO;
	}

	//! \return Konfiguracja sensorów kostiumu
	const Costume::SensorsConfiguration sensorsConfiguration() const
	{
		//TODO - to powinien oferowaæ kostium
		Costume::SensorsConfiguration ret;

		//czujniki imu
		{
			Costume::SensorIDsSet imusIds;

			for (int i = 1; i < IMUS_IN_COSTUME_COUNT + 1; ++i){
				imusIds.insert(i);
			}

			ret.insert(Costume::SensorsConfiguration::value_type(Costume::IMU_SENSOR, imusIds));
		}

		//czujniki insole
		{
			Costume::SensorIDsSet insolsIds;

			for (int i = IMUS_IN_COSTUME_COUNT + 1; i < IMUS_IN_COSTUME_COUNT + INSOLS_IN_COSTUME_COUNT + 1; ++i){
				insolsIds.insert(i);
			}

			ret.insert(Costume::SensorsConfiguration::value_type(Costume::INSOLE_SENSOR, insolsIds));
		}

		return ret;
	}

	//! \param timeout Maksymalny czas oczekiwania na dane [ms] (wartoœc 0 oznacza blokowanie w nieskoñczonoœæ)
	//! \return Ramka danych kostiumu
	const Costume::Frame read(const unsigned int timeout) const
	{
		CostumeRawIO::Frame rawFrame = { 0 };
		Costume::Frame ret;

		uint16_t length = 0;
		if (rawIO->receive(rawFrame, length, timeout) == true){
			if (length == CostumeRawIO::MaxDataSize)
			{
				ret.status = Costume::Frame::COMPLETE_FRAME;
				// Chyba moje dane
				// Próbuje to rozpakowaæ
				// Jak wszystko ok to zaznaczam ¿e ready

				for (unsigned int i = 0; i < IMUS_IN_COSTUME_COUNT; ++i)
				{
					//il.ramek, dl. ramki
					//auto offs = 4 * 8 * i;
					const auto offs = i << 5;
					Costume::ImuData imuData;
					unpackIMU(rawFrame, offs, imuData);
					ret.imusData[i] = imuData;
				}

				for (unsigned int i = IMUS_IN_COSTUME_COUNT; i < IMUS_IN_COSTUME_COUNT + INSOLS_IN_COSTUME_COUNT; ++i)
				{
					const auto offs = i << 5;
					Costume::InsoleRawData insoleData;
					unpackINSOLE(rawFrame, offs, insoleData);
					ret.insolesData[i] = insoleData;
				}
			}
			else{
				ret.status = Costume::Frame::ERROR_FRAME;
			}
		}

		return ret;
	}

	//! \param listenTime Czas nas³uchiwania kostiumów [ms]
	//! \return Lista adresów dostêpnych kostiumów
	static const std::list<Costume::Address> availableCostumes(const unsigned int listenTime)
	{
		std::list<Costume::Address> ret;

		return ret;
	}

private:

	inline static const int16_t extractIntFromPacket(const CostumeRawIO::Frame & frame, unsigned int offset)
	{
		return int16_t((uint16_t(frame[offset + 1]) << 8) | uint16_t(frame[offset]));
	}

	inline static const int16_t extractFloatFromPacket(const CostumeRawIO::Frame & frame, unsigned int offset)
	{
		return float((uint16_t(frame[offset + 1]) << 8) | uint16_t(frame[offset]));
		//return reinterpret_cast<float>((uint16_t(frame[offset + 1]) << 8) | uint16_t(frame[offset]));
	}

	static const bool unpackIMU(const CostumeRawIO::Frame & frame, unsigned int offset,
		Costume::ImuData & imuData)
	{
		Costume::ImuData localData;
		{
			osg::Vec3 acc;

			acc.x() = extractIntFromPacket(frame, offset + 0);
			acc.y() = extractIntFromPacket(frame, offset + 2);
			acc.z() = extractIntFromPacket(frame, offset + 4);
			acc /= 1024.0f;

			localData.accelerometer = acc;
			offset += 8;
		}

		{
			osg::Vec3 mag;

			mag.x() = extractIntFromPacket(frame, offset + 0);
			mag.y() = extractIntFromPacket(frame, offset + 2);
			mag.z() = extractIntFromPacket(frame, offset + 4);
			mag /= 2048.0f;

			localData.magnetometer = mag;
			offset += 8;
		}

		{
			osg::Vec3 gyro;

			gyro.x() = extractIntFromPacket(frame, offset + 0);
			gyro.y() = extractIntFromPacket(frame, offset + 2);
			gyro.z() = extractIntFromPacket(frame, offset + 4);
			gyro /= 1024.0f;

			offset += 8;
			localData.gyroscope = gyro;
		}

		{
			osg::Quat orient;

			orient.w() = extractFloatFromPacket(frame, offset + 0);
			orient.x() = extractFloatFromPacket(frame, offset + 2);
			orient.y() = extractFloatFromPacket(frame, offset + 4);
			orient.z() = extractFloatFromPacket(frame, offset + 6);
			orient /= orient.length();

			localData.orientation = orient;
		}

		imuData = localData;

		return true;
	}

	static const bool unpackINSOLE(const CostumeRawIO::Frame & frame, unsigned int offset,
		Costume::InsoleRawData & insoleData)
	{
		Costume::InsoleRawData localData;
		localData.grfs.resize(SENSORS_COUNT_IN_INSOL);

		for (unsigned int i = 0; i < SENSORS_COUNT_IN_INSOL; ++i)
		{
			osg::Vec3 force;
			force.x() = extractIntFromPacket(frame, offset + 0);
			force.y() = extractIntFromPacket(frame, offset + 2);
			force.z() = extractIntFromPacket(frame, offset + 4);
			force /= 1024.0f;

			localData.grfs[i] = force;
			offset += 8;
		}

		insoleData = localData;

		return true;
	}

private:

	mutable CostumeRawIO * rawIO;
};

using namespace imuCostume;

Costume::Costume(CostumeRawIO * costume)
	: impl_(new CostumeImpl(costume))
{
}

Costume::~Costume()
{
}

const Costume::SensorsConfiguration Costume::sensorsConfiguration() const
{
	return impl_->sensorsConfiguration();
}

const Costume::Frame Costume::read(const unsigned int timeout) const
{
	return impl_->read(timeout);
}

const std::list<Costume::Address> Costume::availableCostumes(const unsigned int listenTime)
{
	return CostumeImpl::availableCostumes(listenTime);
}