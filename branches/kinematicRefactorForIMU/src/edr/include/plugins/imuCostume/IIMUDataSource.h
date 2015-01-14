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
			volatile ConnectionStatus status;
		};

		//! Struktura opisuj¹ca kostium
		struct CostumeDescription
		{
			//! Konfiguracja czujników
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			//! Obiekt obs³uguj¹cy komunikacjê z kostirumem w trybie raw (wyslij/odbierz surowe pakiety danych)
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			//! Status po³¹czenie z kostiumem
			volatile ConnectionStatus status;
			//! Status poszczególnych czujników
			std::map<imuCostume::Costume::SensorID, SensorDescription> sensorsStatus;
			//! Strumieñ surowych danych
			RawDataStreamPtr rawDataStream;
		};

		//! Mapa identyfikatorów sensorów do algorytmów estymujacych ich otientacjê
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmConstPtr> ConstOrientationEstimationAlgorithmsMapping;

		//! Mapa identyfikatorów sensorów do algorytmów estymujacych ich otientacjê
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;

		struct CostumeProfile;

		//! Profil kostiumu
		struct CostumeProfileInstance
		{
			//! \param profil Profil z którego tworzymy instancjê
			//! \return instancja profilu
			static CostumeProfileInstance create(const CostumeProfile & profile)
			{
				CostumeProfileInstance ret;

				ret.name = profile.name;
				ret.calibrationAlgorithm.reset(profile.calibrationAlgorithm->create());
				ret.motionEstimationAlgorithm.reset(profile.motionEstimationAlgorithm->create());
				ret.sensorsAdjustments = profile.sensorsAdjustments;
				ret.sensorsMapping = profile.sensorsMapping;
				ret.skeleton = profile.skeleton;
				for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
				{
					ret.sensorsOrientationEstimationAlgorithms.insert(OrientationEstimationAlgorithmsMapping::value_type(ea.first, IIMUOrientationEstimationAlgorithmPtr(ea.second->create())));
				}

				return ret;
			}

			//! Nazwa profilu
			std::string name;
			//! Szkielet
			kinematic::SkeletonConstPtr skeleton;
			//! Mapowanie sensorów
			SensorsMapping sensorsMapping;
			//! Algorytm kalibracji
			IMUCostumeCalibrationAlgorithmPtr calibrationAlgorithm;
			//! Poprawki sensorów
			IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
			//! Algorytmy estymacji orientacji sensorów			
			OrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
			//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchiê, ograniczneia stawów, ...)
			IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;
		};

		//! Profil kostiumu
		struct CostumeProfile
		{
			//! Nazwa profilu
			std::string name;
			//! Szkielet
			kinematic::SkeletonConstPtr skeleton;
			//! Mapowanie sensorów
			SensorsMapping sensorsMapping;
			//! Algorytm kalibracji
			IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithm;
			//! Poprawki sensorów
			IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
			//! Algorytmy estymacji orientacji sensorów			
			ConstOrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
			//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchiê, ograniczneia stawów, ...)
			IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithm;
		};
		
		//! Mapa algorytmów estymacji orientacji czujników
		typedef std::map<core::UniqueID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationAlgorithms;
		//! Mapa algorytmów kalibracyjnych kostium
		typedef std::map<core::UniqueID, IMUCostumeCalibrationAlgorithmConstPtr> CostumeCalibrationAlgorithms;
		//! Mapa algorytmów estymuj¹cych ruchu
		typedef std::map<core::UniqueID, IMUCostumeMotionEstimationAlgorithmConstPtr> CostumeMotionEstimationAlgorithms;
		//! Lista modeli (szkieletów, hierarchii)
		typedef std::list<kinematic::SkeletonConstPtr> SkeletonModels;
		//! Mapa profili kostiumów
		typedef std::map<std::string, CostumeProfile> CostumesProfiles;

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
		//! \param profileInstance Profil kostiumu
		virtual void loadCalibratedCostume(const unsigned int idx,
			const CostumeProfileInstance & profileInstance) = 0;

		//! Metoda wy³¹dowywujê dane z DM dla kostiumu
		//! \param idx Indeks kostiumu
		virtual void unloadCostume(const unsigned int idx) = 0;
		//! \param idx Indeks kostiumu
		//! \return Dane domenowe kostiumu z DM
		virtual core::ConstVariantsList costumeData(const unsigned int idx) const = 0;

		//! \param algorithm Prototyp algorytmu estymacji orientacji czujnika
		virtual void registerOrientationEstimationAlgorithm(const IIMUOrientationEstimationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu kalibracji kostiumu
		virtual void registerCostumeCalibrationAlgorithm(const IMUCostumeCalibrationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual void registerMotionEstimationAlgorithm(const IMUCostumeMotionEstimationAlgorithm * algorithm) = 0;
		//! \param skeleton Model szkieletu który mo¿na estymowaæ
		virtual void registerSkeletonModel(kinematic::SkeletonConstPtr skeleton) = 0;
		//! \param profile Profil kostiumu
		virtual void registerCostumeProfile(const CostumeProfile & profile) = 0;

		//! \return Lista prototypów algorytmów estymacji orientacji czujnika
		virtual OrientationEstimationAlgorithms orientationEstimationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów kalibracji kostiumu
		virtual CostumeCalibrationAlgorithms calibrationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual CostumeMotionEstimationAlgorithms motionEstimationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual SkeletonModels skeletonModels() const = 0;
		//! \return Profile kostiumu
		virtual CostumesProfiles costumesProfiles() const = 0;
	};
}

#endif	// __HEADER_GUARD_IMU__IIMUDATASOURCE_H__