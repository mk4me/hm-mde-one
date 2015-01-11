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
	//! Klasa realizuj�ca filtracj� kompletnych danych IMU (pe�ne dane dla wszystkich czujnik�w IMU)
	class IMU_EXPORT CostumeCompleteDataFilter
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeCompleteDataFilter(CostumeCompleteDataFilter && Other);
		//! \param Other Obiekt kopiowany
		CostumeCompleteDataFilter(const CostumeCompleteDataFilter & Other);
		//! \param sensorsIDs Identyfikatory sensor�w IMU
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensor�w
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Desturktor
		~CostumeCompleteDataFilter();
		//! \param dataToFilter Dane do filtracji
		//! \return Prawda je�li dane s� kompletne
		bool operator()(const IMU::CostumeStream::value_type & dataToFilter) const;

	private:
		//! Zbi�r identyfikator�w sensor�w IMU
		imuCostume::Costume::SensorIDsSet sensorsIDs;
	};

	//! Klasa realizuj�ca konwersj� ze strumienia danych czujnik�w do kompletnych danych IMU
	class IMU_EXPORT CostumeIMUExtractor
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeIMUExtractor(CostumeIMUExtractor && Other);
		//! \param Other Obiekt kopiowany
		CostumeIMUExtractor(const CostumeIMUExtractor & Other);
		//! \param sensorsIDs Identyfikatory czujnik�w IMU
		CostumeIMUExtractor(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensor�w
		CostumeIMUExtractor(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Destruktor
		~CostumeIMUExtractor();
		//! \param dataToVerify Dane do veryfikacji pd k�tem mo�liwo�ci konwersji
		//! \return Prawda je�li dane mo�na konwertowa�
		bool verify(const IMU::CostumeStream::value_type & dataToVerify) const;
		//! \param convertedData [out] Dane po konwersji
		void extract(const IMU::CostumeStream::value_type & dataToConvert, IMU::SensorsStreamData & convertedData) const;

	private:
		//! Obiekt realizuj�cy weryfikacj�
		CostumeCompleteDataFilter completeDataFilter;
	};

	//! Klasa procesora strumienia estymuj�ca orientacje czujnik�w na bazie pomiar�w
	class IMU_EXPORT OrientationEstimator
	{
	public:
		//! \param orientationAlgorithms Algorytmy wstymuj�c� orientacje czujnik�w
		OrientationEstimator(const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping & orientationAlgorithms);
		//! Destruktor
		~OrientationEstimator();
		//! \param [out] data Dane do estymacji
		void operator()(IMU::SensorsStreamData & data) const;		

	private:
		//! Poprzedni czas pr�bki
		mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;
		//! Algorytmy estymuj�ce orientacje czujnik�w
		const IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping orientationAlgorithms;
	};

	//! Klasa pozwalaj�ca wypakowywa� dane z wektor�w
	class IMU_EXPORT ArrayExtractor
	{
	public:
		//! \param idx Index obiketu kt�ry chcemy wypakowywa� z wektora
		ArrayExtractor(const unsigned int idx);
		//! \param Other Inny kopiowany extractor
		ArrayExtractor(const ArrayExtractor & Other);
		//! Destruktor
		~ArrayExtractor();

		//! \tparam AT Wektor do weryfikacji
		template<typename AT>
		//! \param a Wektor z kt�rego chcemy wypakowa� dane
		//! \return Czy z wektora da si� wypakowa� dane o zadanym indeksie
		bool verify(const AT & a) const
		{
			return true;
		}

		//! \tparam AT Wektor do weryfikacji
		//! \tparam Ret Typ kt�ry chcemy wypakowa� z wektora
		template<typename AT, typename Ret>
		//! \param Wektor z kt�rego wypakowujemy dane
		//! \param ret [out] Obiekt docelowy dla wypakowanych danych
		void extract(const AT & a, Ret & ret) const
		{
			ret = a[idx];
		}

	private:
		//! Indeks spod kt�rego wybieramy dane z wektora
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