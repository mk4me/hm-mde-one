/********************************************************************
	created:  2014/12/03	12:35:25
	filename: IIMUDataSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IIMUDATASOURCE_H__
#define __HEADER_GUARD_IMU__IIMUDATASOURCE_H__

#include <atomic>
#include <corelib/Variant.h>
#include <plugins/imuCostume/Streams.h>
#include <plugins/imuCostume/IIMUOrientationEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <kinematiclib/SkeletonState.h>

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

		//! Struktura opisuj�ca status czujnika
		struct SensorDescription
		{
			utils::shared_ptr<std::atomic<ConnectionStatus>> status;
		};

		//! Struktura opisuj�ca kostium
		struct CostumeDescription
		{
			//! Konfiguracja czujnik�w
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			//! Obiekt obs�uguj�cy komunikacj� z kostirumem w trybie raw (wyslij/odbierz surowe pakiety danych)
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			//! Status po��czenie z kostiumem
			utils::shared_ptr<std::atomic<ConnectionStatus>> status;
			//! Status poszczeg�lnych czujnik�w
			std::map<imuCostume::Costume::SensorID, SensorDescription> sensorsStatus;
			//! Strumie� surowych danych
			RawDataStreamPtr rawDataStream;
		};

		//! Mapa identyfikator�w sensor�w do algorytm�w estymujacych ich otientacj�
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;

	public:
		//! Desturktor wirtualny
		virtual ~IIMUDataSource() {}
		//! Metoda od�wie�a dostepne kostiumu
		//! \return Prawda je�li uda�o si� od�wie�y� kostiumy
		virtual const bool refreshCostumes() = 0;
		//! \return Ilo�� dostepnych kostium�w
		virtual const unsigned int costumesCout() const = 0;
		//! \param idx Indeks kostiumu
		//! \return Czy dany kostium jest za�adowany
		virtual bool costumeLoaded(const unsigned int idx) const = 0;
		//! \return Ilo�� za�adowanych kostium�e
		virtual unsigned int loadedCostumesCount() const = 0;
		//! \param idx Indeks kostiumu
		//! \return Opis kostiumu
		virtual const CostumeDescription & costumeDescription(const unsigned int idx) const = 0;
		//! Metoda �aduj�ca kostium do DM - tylko surowe dane + konwersja
		//! \param idx Indeks kostiumu
		virtual void loadRawCostume(const unsigned int idx) = 0;
		//! Metoda �aduj�ca kostium do DM - surowe dane + konwersja + estymacja ruchu po kalibracji
		//! \param idx Indeks kostiumu
		//! \param skeleton Szkielet skojarzony z kostiumem
		//! \param orientationAlgorithms Algorytmy estymacji orientacji czujnik�w kostiumu
		//! \param motionEstimationAlgorithm Algorytm poprawiaj�cy og�ln� estymacje ruchu dla ca�ego szkieletu
		virtual void loadCalibratedCostume(const unsigned int idx,
			kinematic::SkeletonConstPtr skeleton,
			const SensorsMapping & sensorsMapping,
			const IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustments,
			const OrientationEstimationAlgorithmsMapping & orientationAlgorithms,
			IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm) = 0;

		//! Metoda wy��dowywuj� dane z DM dla kostiumu
		//! \param idx Indeks kostiumu
		virtual void unloadCostume(const unsigned int idx) = 0;
		//! \param idx Indeks kostiumu
		//! \return Dane domenowe kostiumu z DM
		virtual core::ConstVariantsList costumeData(const unsigned int idx) const = 0;

		//! \param algorithm Prototyp algorytmu estymacji orientacji czujnika
		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu kalibracji kostiumu
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithm * algorithm) = 0;
		//! \param skeleton Model szkieletu kt�ry mo�na estymowa�
		virtual void registerSkeletonModel(kinematic::SkeletonConstPtr skeleton) = 0;


		//! \return Lista prototyp�w algorytm�w estymacji orientacji czujnika
		virtual std::list<IIMUOrientationEstimationAlgorithmConstPtr> orientationEstimationAlgorithms() const = 0;
		//! \return Lista prototyp�w algorytm�w kalibracji kostiumu
		virtual std::list<IMUCostumeCalibrationAlgorithmConstPtr> calibrationAlgorithms() const = 0;
		//! \return Lista prototyp�w algorytm�w poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual std::list<IMUCostumeMotionEstimationAlgorithmConstPtr> motionEstimationAlgorithms() const = 0;
		//! \return Lista prototyp�w algorytm�w poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual std::list<kinematic::SkeletonConstPtr> skeletonModels() const = 0;
	};
}

#endif	// __HEADER_GUARD_IMU__IIMUDATASOURCE_H__