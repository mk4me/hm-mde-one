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
			volatile ConnectionStatus status;
		};

		//! Struktura opisuj�ca kostium
		struct CostumeDescription
		{
			//! Konfiguracja czujnik�w
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			//! Obiekt obs�uguj�cy komunikacj� z kostirumem w trybie raw (wyslij/odbierz surowe pakiety danych)
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			//! Status po��czenie z kostiumem
			volatile ConnectionStatus status;
			//! Status poszczeg�lnych czujnik�w
			std::map<imuCostume::Costume::SensorID, SensorDescription> sensorsStatus;
			//! Strumie� surowych danych
			RawDataStreamPtr rawDataStream;
		};

		//! Mapa identyfikator�w sensor�w do algorytm�w estymujacych ich otientacj�
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmConstPtr> ConstOrientationEstimationAlgorithmsMapping;

		//! Mapa identyfikator�w sensor�w do algorytm�w estymujacych ich otientacj�
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;

		struct CostumeProfile;

		//! Profil kostiumu
		struct CostumeProfileInstance
		{
			//! \param profil Profil z kt�rego tworzymy instancj�
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
			//! Mapowanie sensor�w
			SensorsMapping sensorsMapping;
			//! Algorytm kalibracji
			IMUCostumeCalibrationAlgorithmPtr calibrationAlgorithm;
			//! Poprawki sensor�w
			IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
			//! Algorytmy estymacji orientacji sensor�w			
			OrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
			//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchi�, ograniczneia staw�w, ...)
			IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;
		};

		//! Profil kostiumu
		struct CostumeProfile
		{
			//! Nazwa profilu
			std::string name;
			//! Szkielet
			kinematic::SkeletonConstPtr skeleton;
			//! Mapowanie sensor�w
			SensorsMapping sensorsMapping;
			//! Algorytm kalibracji
			IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithm;
			//! Poprawki sensor�w
			IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
			//! Algorytmy estymacji orientacji sensor�w			
			ConstOrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
			//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchi�, ograniczneia staw�w, ...)
			IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithm;
		};
		
		//! Mapa algorytm�w estymacji orientacji czujnik�w
		typedef std::map<core::UniqueID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationAlgorithms;
		//! Mapa algorytm�w kalibracyjnych kostium
		typedef std::map<core::UniqueID, IMUCostumeCalibrationAlgorithmConstPtr> CostumeCalibrationAlgorithms;
		//! Mapa algorytm�w estymuj�cych ruchu
		typedef std::map<core::UniqueID, IMUCostumeMotionEstimationAlgorithmConstPtr> CostumeMotionEstimationAlgorithms;
		//! Lista modeli (szkielet�w, hierarchii)
		typedef std::list<kinematic::SkeletonConstPtr> SkeletonModels;
		//! Mapa profili kostium�w
		typedef std::map<std::string, CostumeProfile> CostumesProfiles;

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
		//! \param profileInstance Profil kostiumu
		virtual void loadCalibratedCostume(const unsigned int idx,
			const CostumeProfileInstance & profileInstance) = 0;

		//! Metoda wy��dowywuj� dane z DM dla kostiumu
		//! \param idx Indeks kostiumu
		virtual void unloadCostume(const unsigned int idx) = 0;
		//! \param idx Indeks kostiumu
		//! \return Dane domenowe kostiumu z DM
		virtual core::ConstVariantsList costumeData(const unsigned int idx) const = 0;

		//! \param algorithm Prototyp algorytmu estymacji orientacji czujnika
		virtual void registerOrientationEstimationAlgorithm(const IIMUOrientationEstimationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu kalibracji kostiumu
		virtual void registerCostumeCalibrationAlgorithm(const IMUCostumeCalibrationAlgorithm * algorithm) = 0;
		//! \param algorithm Prototyp algorytmu poprawy estymacji ruchu kostiumu dla ca�ego szkieletu
		virtual void registerMotionEstimationAlgorithm(const IMUCostumeMotionEstimationAlgorithm * algorithm) = 0;
		//! \param skeleton Model szkieletu kt�ry mo�na estymowa�
		virtual void registerSkeletonModel(kinematic::SkeletonConstPtr skeleton) = 0;
		//! \param profile Profil kostiumu
		virtual void registerCostumeProfile(const CostumeProfile & profile) = 0;

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