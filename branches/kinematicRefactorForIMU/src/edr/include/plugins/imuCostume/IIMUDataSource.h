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
	//! Interfejs Ÿród³a danych obs³uguj¹cego kostiumy
	class IIMUDataSource
	{
	public:

		//! Typ statusu po³¹czenia z kostiumem i czujników
		enum ConnectionStatus
		{
			ONLINE,				//! Po³¹czenie OK
			OFFLINE,			//! Brak po³¹czenia
			CONNECTION_PROBLEMS,//! Problemy z po³¹czeniem - gubimy pakiety
			UNKNOWN				//! Nieznany stan - jeszcze siê nie próbowlaiœmy pod³anczaæ
		};

		//! Struktura opisuj¹ca status czujnika
		struct SensorDescription
		{
			utils::shared_ptr<std::atomic<ConnectionStatus>> status;
		};

		//! Struktura opisuj¹ca kostium
		struct CostumeDescription
		{
			//! Konfiguracja czujników
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			//! Obiekt obs³uguj¹cy komunikacjê z kostirumem w trybie raw (wyslij/odbierz surowe pakiety danych)
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			//! Status po³¹czenie z kostiumem
			utils::shared_ptr<std::atomic<ConnectionStatus>> status;
			//! Status poszczególnych czujników
			std::map<imuCostume::Costume::SensorID, SensorDescription> sensorsStatus;
			//! Strumieñ surowych danych
			RawDataStreamPtr rawDataStream;
		};

		//! Mapa identyfikatorów sensorów do algorytmów estymujacych ich otientacjê
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;

	public:
		//! Desturktor wirtualny
		virtual ~IIMUDataSource() {}
		//! Metoda odœwie¿a dostepne kostiumu
		//! \return Prawda jeœli uda³o siê odœwie¿yæ kostiumy
		virtual const bool refreshCostumes() = 0;
		//! \return Iloœæ dostepnych kostiumów
		virtual const unsigned int costumesCout() const = 0;
		//! \param idx Indeks kostiumu
		//! \return Czy dany kostium jest za³adowany
		virtual bool costumeLoaded(const unsigned int idx) const = 0;
		//! \return Iloœæ za³adowanych kostiumóe
		virtual unsigned int loadedCostumesCount() const = 0;
		//! \param idx Indeks kostiumu
		//! \return Opis kostiumu
		virtual const CostumeDescription & costumeDescription(const unsigned int idx) const = 0;
		//! Metoda ³aduj¹ca kostium do DM - tylko surowe dane + konwersja
		//! \param idx Indeks kostiumu
		virtual void loadRawCostume(const unsigned int idx) = 0;
		//! Metoda ³aduj¹ca kostium do DM - surowe dane + konwersja + estymacja ruchu po kalibracji
		//! \param idx Indeks kostiumu
		//! \param skeleton Szkielet skojarzony z kostiumem
		//! \param orientationAlgorithms Algorytmy estymacji orientacji czujników kostiumu
		//! \param motionEstimationAlgorithm Algorytm poprawiaj¹cy ogóln¹ estymacje ruchu dla ca³ego szkieletu
		virtual void loadCalibratedCostume(const unsigned int idx,
			kinematic::SkeletonConstPtr skeleton,
			const SensorsMapping & sensorsMapping,
			const IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustments,
			const OrientationEstimationAlgorithmsMapping & orientationAlgorithms,
			IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm) = 0;

		//! Metoda wy³¹dowywujê dane z DM dla kostiumu
		//! \param idx Indeks kostiumu
		virtual void unloadCostume(const unsigned int idx) = 0;
		//! \param idx Indeks kostiumu
		//! \return Dane domenowe kostiumu z DM
		virtual core::ConstVariantsList costumeData(const unsigned int idx) const = 0;

		//! \param algorithm Prototyp algorytmu estymacji orientacji czujnika
		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu kalibracji kostiumu
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithm * algorithm) = 0;
		//! \param skeleton Model szkieletu który mo¿na estymowaæ
		virtual void registerSkeletonModel(kinematic::SkeletonConstPtr skeleton) = 0;


		//! \return Lista prototypów algorytmów estymacji orientacji czujnika
		virtual std::list<IIMUOrientationEstimationAlgorithmConstPtr> orientationEstimationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów kalibracji kostiumu
		virtual std::list<IMUCostumeCalibrationAlgorithmConstPtr> calibrationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual std::list<IMUCostumeMotionEstimationAlgorithmConstPtr> motionEstimationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual std::list<kinematic::SkeletonConstPtr> skeletonModels() const = 0;
	};
}

#endif	// __HEADER_GUARD_IMU__IIMUDATASOURCE_H__