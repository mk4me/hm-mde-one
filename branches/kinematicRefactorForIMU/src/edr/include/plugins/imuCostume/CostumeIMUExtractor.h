/********************************************************************
	created:  2015/01/04	21:15:37
	filename: CostumeIMUExtractor.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__
#define __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__

#include <plugins/imuCostume/Export.h>
#include <imucostumelib/ImuCostume.h>
#include <plugins/imuCostume/IIMUDataSource.h>

namespace IMU
{
	//! Klasa realizuj¹ca filtracjê kompletnych danych IMU (pe³ne dane dla wszystkich czujników IMU)
	class IMU_EXPORT CostumeCompleteDataFilter
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeCompleteDataFilter(CostumeCompleteDataFilter && Other);
		//! \param Other Obiekt kopiowany
		CostumeCompleteDataFilter(const CostumeCompleteDataFilter & Other);
		//! \param sensorsIDs Identyfikatory sensorów IMU
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensorów
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Desturktor
		~CostumeCompleteDataFilter();
		//! \param dataToFilter Dane do filtracji
		//! \return Prawda jeœli dane s¹ kompletne
		bool operator()(const IMU::CostumeStream::value_type & dataToFilter) const;

	private:
		//! Zbiór identyfikatorów sensorów IMU
		imuCostume::Costume::SensorIDsSet sensorsIDs;
	};

	//! Klasa realizuj¹ca konwersjê ze strumienia danych czujników do kompletnych danych IMU
	class IMU_EXPORT CostumeIMUExtractor
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeIMUExtractor(CostumeIMUExtractor && Other);
		//! \param Other Obiekt kopiowany
		CostumeIMUExtractor(const CostumeIMUExtractor & Other);
		//! \param sensorsIDs Identyfikatory czujników IMU
		CostumeIMUExtractor(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensorów
		CostumeIMUExtractor(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Destruktor
		~CostumeIMUExtractor();
		//! \param dataToVerify Dane do veryfikacji pd k¹tem mo¿liwoœci konwersji
		//! \return Prawda jeœli dane mo¿na konwertowaæ
		bool verify(const IMU::CostumeStream::value_type & dataToVerify) const;
		//! \param convertedData [out] Dane po konwersji
		void extract(const IMU::CostumeStream::value_type & dataToConvert, IMU::SensorsStreamData & convertedData) const;

	private:
		//! Obiekt realizuj¹cy weryfikacjê
		CostumeCompleteDataFilter completeDataFilter;
	};

	//! Klasa procesora strumienia estymuj¹ca orientacje czujników na bazie pomiarów
	class IMU_EXPORT OrientationEstimator
	{
	public:
		//! \param orientationAlgorithms Algorytmy wstymuj¹cê orientacje czujników
		OrientationEstimator(const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping & orientationAlgorithms);
		//! Destruktor
		~OrientationEstimator();
		//! \param [out] data Dane do estymacji
		void operator()(IMU::SensorsStreamData & data) const;		

	private:
		//! Poprzedni czas próbki
		mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;
		//! Algorytmy estymuj¹ce orientacje czujników
		const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping orientationAlgorithms;
	};

	//! Klasa pozwalaj¹ca wypakowywaæ dane z wektorów
	class IMU_EXPORT ArrayExtractor
	{
	public:
		//! \param idx Index obiketu który chcemy wypakowywaæ z wektora
		ArrayExtractor(const unsigned int idx);
		//! \param Other Inny kopiowany extractor
		ArrayExtractor(const ArrayExtractor & Other);
		//! Destruktor
		~ArrayExtractor();

		//! \tparam AT Wektor do weryfikacji
		template<typename AT>
		//! \param a Wektor z którego chcemy wypakowaæ dane
		//! \return Czy z wektora da siê wypakowaæ dane o zadanym indeksie
		bool verify(const AT & a) const
		{
			return true;
		}

		//! \tparam AT Wektor do weryfikacji
		//! \tparam Ret Typ który chcemy wypakowaæ z wektora
		template<typename AT, typename Ret>
		//! \param Wektor z którego wypakowujemy dane
		//! \param ret [out] Obiekt docelowy dla wypakowanych danych
		void extract(const AT & a, Ret & ret) const
		{
			ret = a[idx];
		}

	private:
		//! Indeks spod którego wybieramy dane z wektora
		const unsigned int idx;
	};


	class IMU_EXPORT OrientationExtractor
	{
	public:
		OrientationExtractor(const unsigned int idx);
		OrientationExtractor(const OrientationExtractor & Other);
		~OrientationExtractor();

		bool verify(const IMU::SkeletonMotionState & a) const;

		void extract(const IMU::SkeletonMotionState & a, osg::Quat & ret) const;

	private:
		const unsigned int idx;
	};

	class IMU_EXPORT CANopenDataExtractor
	{
	public:

		CANopenDataExtractor();
		~CANopenDataExtractor();

		bool verify(const IMU::CANopenFramesStream::value_type & a);

		void extract(const IMU::CANopenFramesStream::value_type & a, IMU::CostumeStream::value_type & ret) const;
	};

	class ArrayStreamAdapter
	{
	public:

		template<typename Base, typename Dest>
		static threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor> * create(typename threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>::BaseStreamTypePtr baseStream, const unsigned int idx)
		{
			return new threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>(baseStream, ArrayExtractor(idx));
		}
	};

	class IMU_EXPORT ExtractCostumeMotion
	{
	public:
		ExtractCostumeMotion(
			kinematic::SkeletonConstPtr skeleton,
			const IMU::SensorsMapping & sensorsMapping,
			const IMU::DataIndexToJointMapping & dataMapping,
			const IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustments,
			IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm);

		~ExtractCostumeMotion();

		bool verify(const IMU::SensorsStreamData & input) const;

		void extract(const IMU::SensorsStreamData & input, IMU::SkeletonMotionState & output) const;

	private:
		kinematic::SkeletonConstPtr skeleton;
		IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
		const IMU::SensorsMapping sensorsMapping;
		const IMU::DataIndexToJointMapping dataMapping;
		mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;
		const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping orientationAlgorithms;
		const IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;
	};
}

#endif	// __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__