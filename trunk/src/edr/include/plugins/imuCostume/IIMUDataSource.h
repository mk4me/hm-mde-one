/********************************************************************
	created:  2014/12/03	12:35:25
	filename: IIMUDataSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IIMUDATASOURCE_H__
#define __HEADER_GUARD_IMU__IIMUDATASOURCE_H__

#include <atomic>
#include <corelib/IIdentifiable.h>
#include <corelib/Variant.h>
#include <plugins/imuCostume/Streams.h>
#include <plugins/imuCostume/IIMUOrientationEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <kinematiclib/SkeletonState.h>
#include <plugins/imuCostume/CostumeProfile.h>
#include <plugins/imuCostume/CostumeProfile.h>

namespace IMU
{
	//! Interfejs �r�d�a danych obs�uguj�cego kostiumy
	class IIMUDataSource
	{
	public:

		//! Typ statusu po��czenia z kostiumem i czujnik�w
		enum ConnectionStatus
		{
			ONLINE,				//! Po��czenie OK
			OFFLINE,			//! Brak po��czenia
			CONNECTION_PROBLEMS,//! Problemy z po��czeniem - gubimy pakiety
			UNKNOWN				//! Nieznany stan - jeszcze si� nie pr�bowlai�my pod�ancza�
		};	

		//! Typ opisuj�cy identyfikator kostiumy
		typedef imuCostume::CostumeRawIO::CostumeAddress CostumeID;

		//! Zapisywane dane sensora
		struct RecordedSensorData : SensorData
		{
			//! Estymowana orientacja
			osg::Quat estimatedOrientation;
		};

		//! Zapisywane dane kostiumu
		struct RecordedCostumeData
		{
			//! Czas
			imuCostume::CostumeCANopenIO::Timestamp timestamp;
			//! Dane sensor�w
			std::map<imuCostume::Costume::SensorID, RecordedSensorData> sensorsData;
			//! Dane szkieletu
			kinematic::SkeletonState::RigidCompleteState skeletonData;
		};

		//! Mapa pakiet�w wg kostium�w
		typedef std::map<CostumeID, RecordedCostumeData> CostumesDataFrame;
		//! Strumie� danych pakiet�w kostium�w
		typedef threadingUtils::StreamBufferT<CostumesDataFrame> CostumesRecordingDataBuffer;

		typedef std::map<imuCostume::CostumeRawIO::CostumeAddress, imuCostume::Costume::SensorIDsSet> CostumesToRecord;

		//! Struktura opisuj�ca zapis danych z kostium�w
		struct RecordingConfiguration
		{
			//! Strumie� bufor�w z danymi kostium�w
			CostumesRecordingDataBuffer costumesDataBuffer;
			//! Zbi�r nagrywanych kostium�w
			CostumesToRecord costumesToRecord;
		};

		DEFINE_SMART_POINTERS(RecordingConfiguration);

		//! Struktura opisuj�ca status po��czenia kostiumu i czujnik�w
		struct CostumeStatus
		{
			//! Status po��czenie z kostiumem
			volatile ConnectionStatus status;
			//! Status poszczeg�lnych czujnik�w
			std::map<imuCostume::Costume::SensorID, ConnectionStatus> sensorsStatus;
		};

		//! Mapa status�w kostium�w i ich sensor�w
		typedef std::map<CostumeID, CostumeStatus> CostumesStatus;

		//! Struktura opisuj�ca kostium
		struct CostumeDescription
		{
			//! Konfiguracja czujnik�w
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			//! Obiekt obs�uguj�cy komunikacj� z kostirumem w trybie raw (wyslij/odbierz surowe pakiety danych)
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			//! Strumie� surowych danych
			RawDataStreamPtr rawDataStream;
			//! Profil kosiumu
			CostumeProfilePtr profile;
		};

		//! Mapa status�w kostium�w i ich sensor�w
		typedef std::map<CostumeID, CostumeDescription> CostumesDescriptions;

		//! Struktura opisuj�ca szczeg�y kostiumu
		struct CostumeDetails
		{
			CostumeStatus statusDetails;
			CostumeDescription descriptionDetails;
		};

		//! Mapa pe�nej informajci o kostiumach
		typedef std::map<CostumeID, CostumeDetails> CostumesDetails;

		//! Mapa identyfikator�w sensor�w do algorytm�w estymujacych ich otientacj�
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationPrototypeAlgorithmsMapping;

		//! Mapa identyfikator�w sensor�w do algorytm�w estymujacych ich otientacj�
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;
		
		//! Mapa algorytm�w estymacji orientacji czujnik�w
		typedef std::map<core::UniqueID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationAlgorithms;
		//! Mapa algorytm�w kalibracyjnych kostium
		typedef std::map<core::UniqueID, IMUCostumeCalibrationAlgorithmConstPtr> CostumeCalibrationAlgorithms;
		//! Mapa algorytm�w estymuj�cych ruchu
		typedef std::map<core::UniqueID, IMUCostumeMotionEstimationAlgorithmConstPtr> CostumeMotionEstimationAlgorithms;
		//! Lista modeli (szkielet�w, hierarchii)
		typedef std::map<core::UniqueID, SkeletonConstPtr> SkeletonModels;
		//! Mapa profili kostium�w
		typedef std::map<std::string, CostumeProfileConstPtr> CostumesProfiles;

	public:
		//! Desturktor wirtualny
		virtual ~IIMUDataSource() {}

		//! \param id Indeks kostiumu
		//! \return Czy dany kostium jest za�adowany
		virtual bool addCostume(const CostumeID & id) = 0;
		//! Metoda od�wie�a dostepne kostiumu
		//! \return Prawda je�li uda�o si� od�wie�y� kostiumy
		virtual const bool refreshCostumes() = 0;
		//! \return Ilo�� dostepnych kostium�w
		virtual const unsigned int costumesCout() const = 0;
		//! \return Ilo�� dostepnych kostium�w
		virtual const bool costumesEmpty() const = 0;
		//! \param id Indeks kostiumu
		//! \return Czy dany kostium jest za�adowany
		virtual bool costumeLoaded(const CostumeID & id) const = 0;
		//! \return Ilo�� za�adowanych kostium�e
		virtual unsigned int loadedCostumesCount() const = 0;
		//! \param id Identyfikator kosstiumu
		//! \param samplesCount ilo�� pr�b pozyskania danych do wyciagniecia konfiguracji kostiumu
		//! \return Czy uda�o si� chocia� z jednej pr�bki wyci�gna� konfiguracj�
		virtual bool refreshCostumeSensorsConfiguration(const CostumeID & id, const uint8_t samplesCount) = 0;
		//! \param id Indeks kostiumu
		//! \return Opis kostiumu
		virtual CostumeDescription costumeDescription(const CostumeID & id) const = 0;

		//! \param id Indeks kostiumu
		virtual void resetCostumeStatus(const CostumeID & id) = 0;
		//! \param costumeID Indeks kostiumu
		//! \param sensorID Indeks kostiumu
		virtual void resetSensorStatus(const CostumeID & costumeID,
			const imuCostume::Costume::SensorID sensorID) = 0;
		//! \param id Indeks kostiumu
		//! \return Opis kostiumu
		virtual CostumeStatus costumeStatus(const CostumeID & id) const = 0;
		//! \param id Indeks kostiumu
		//! \return Opis kostiumu
		virtual CostumeDetails costumeDetails(const CostumeID & id) const = 0;
		//! \return Opis kostiumu
		virtual CostumesDescriptions costumesDescriptions() const = 0;
		//! \return Opis kostiumu
		virtual CostumesStatus costumesStatus() const = 0;
		//! \return Opis kostiumu
		virtual CostumesDetails costumesDetails() const = 0;
		//! Metoda �aduj�ca kostium do DM - tylko surowe dane + konwersja
		//! \param id Indeks kostiumu
		virtual void loadRawCostume(const CostumeID & id) = 0;
		//! Metoda �aduj�ca kostium do DM - surowe dane + konwersja + estymacja ruchu po kalibracji
		//! \param id Indeks kostiumu
		//! \param profileInstance Profil kostiumu
		virtual void loadCalibratedCostume(const CostumeID & id,
			CostumeProfilePtr profile) = 0;

		//! Metoda wy��dowywuj� dane z DM dla kostiumu
		//! \param id Indeks kostiumu
		virtual void unloadCostume(const CostumeID & id) = 0;
		//! \param id Indeks kostiumu
		//! \return Dane domenowe kostiumu z DM
		virtual core::ConstVariantsList costumeData(const CostumeID & id) const = 0;

		//! \param algorithm Prototyp algorytmu estymacji orientacji czujnika
		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithmPtr algorithm) = 0;
		//! \param algorithm Prototyp algorytmu kalibracji kostiumu
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithmPtr algorithm) = 0;
		//! \param algorithm Prototyp algorytmu poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithmPtr algorithm) = 0;
		//! \param skeleton Model szkieletu kt�ry mo�na estymowa�
		virtual void registerSkeletonModel(SkeletonPtr skeleton) = 0;
		//! \param profile Profil kostiumu
		virtual void registerCostumeProfile(CostumeProfilePtr profile) = 0;

		virtual void startRecording(RecordingConfigurationPtr recording) = 0;
		virtual void stopRecording(RecordingConfigurationPtr recording) = 0;

		//! \return Lista prototyp�w algorytm�w estymacji orientacji czujnika
		virtual OrientationEstimationAlgorithms orientationEstimationAlgorithms() const = 0;
		//! \return Lista prototyp�w algorytm�w kalibracji kostiumu
		virtual CostumeCalibrationAlgorithms calibrationAlgorithms() const = 0;
		//! \return Lista prototyp�w algorytm�w poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual CostumeMotionEstimationAlgorithms motionEstimationAlgorithms() const = 0;
		//! \return Lista prototyp�w algorytm�w poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual SkeletonModels skeletonModels() const = 0;
		//! \return Profile kostiumu
		virtual CostumesProfiles costumesProfiles() const = 0;
	};
}

#endif	// __HEADER_GUARD_IMU__IIMUDATASOURCE_H__