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
#include <plugins/kinematic/Wrappers.h>

namespace IMU
{
	//! Klasa usuwaj�ca czas z danych
	class TimeRemoverExtractor
	{
	public:
		//! \tparam Src Typ �r�d�owy
		template<typename Src>
		inline static bool verify(const Src & timeData)
		{
			return true;
		}
		//! \tparam Src Typ �r�d�owy
		template<typename Src>
		inline static void extract(const Src & timeData, typename Src::second_type & data)
		{
			data = timeData.second;
		}
	};

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
	private:

		struct ImuSensorData
		{
			osg::Vec3d acc;
			osg::Vec3d gyro;
			osg::Vec3d mag;
			osg::Quat orient;
			int status;
		};

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
		//aktualne dane do wysy�k dalej w przetwarzanie
		mutable std::map<imuCostume::Costume::SensorID, ImuSensorData> currentData;
	};

	//! Klasa procesora strumienia estymuj�ca orientacje czujnik�w na bazie pomiar�w
	class IMU_EXPORT OrientationEstimator
	{
	public:
		//! \param orientationAlgorithms Algorytmy wstymuj�c� orientacje czujnik�w
		OrientationEstimator(IMU::CostumeProfilePtr profile);
		//! Destruktor
		~OrientationEstimator();
		//! \param [out] data Dane do estymacji
		void operator()(IMU::SensorsStreamData & data) const;		

	private:
		//! Profil z algorytmami kt�re estymuj� orientacje czujnik�w
		IMU::CostumeProfilePtr profile;

		mutable std::map<imuCostume::Costume::SensorID, uint32_t> lastUpdateTime;
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

	//! Klasa pozwalaj�ca wypakowywa� dane z wektor�w
	class IMU_EXPORT CompoundArrayExtractor
	{
	public:
		//! \param idx Index obiketu kt�ry chcemy wypakowywa� z wektora
		CompoundArrayExtractor(const unsigned int idx);
		//! \param Other Inny kopiowany extractor
		CompoundArrayExtractor(const CompoundArrayExtractor & Other);
		//! Destruktor
		~CompoundArrayExtractor();

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
			ret.first = a.first;
			ret.second = a.second[idx];
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

		bool verify(const IMU::MotionStream::value_type & a) const;

		void extract(const IMU::MotionStream::value_type & a, osg::Quat & ret) const;

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

		template<typename Base, typename Dest = decltype(std::declval<Base>().operator[](0))>
		static threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor> * create(typename threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>::BaseStreamTypePtr baseStream, const unsigned int idx)
		{
			return new threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>(baseStream, ArrayExtractor(idx));
		}
	};	

	class IMU_EXPORT ExtractCostumeMotion
	{
	public:
		ExtractCostumeMotion(
			IMU::CostumeProfilePtr profile,
			const IMU::DataIndexToJointMapping & dataMapping);

		~ExtractCostumeMotion();

		bool verify(const IMU::SensorsStreamData & input) const;

		void extract(const IMU::SensorsStreamData & input, IMU::MotionStream::value_type & output) const;

	private:
		IMU::CostumeProfilePtr profile;
		mutable kinematic::SkeletonState skeletonState;				
		const IMU::DataIndexToJointMapping dataMapping;
		mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;		
	};

	class IMU_EXPORT KinematicStreamExtractor
	{
	public:
		KinematicStreamExtractor(kinematic::SkeletonState && skeletonState);

		~KinematicStreamExtractor();

		bool verify(const IMU::MotionStream::value_type & input) const;

		void extract(const IMU::MotionStream::value_type & input, IMU::SkeletonStateStream::value_type & output) const;

		const kinematic::SkeletonState & skeletonState() const;
		float currentTime() const;

	private:

		mutable kinematic::SkeletonState skeletonState_;
		mutable float currentTime_;
	};

	class IMU_EXPORT RawToCANopenExtractor
	{
	public:

		RawToCANopenExtractor();
		~RawToCANopenExtractor();

		bool verify(const IMU::RawDataStream::value_type & val) const;

		void extract(const IMU::RawDataStream::value_type & in, IMU::CANopenFramesStream::value_type & out) const;
	};

	class IMUAccExtractor
	{
	public:
		bool verify(const IMU::IMUStream::value_type & a)
		{
			return true;
		}
		
		void extract(const IMU::IMUStream::value_type & a, Vec3Stream::value_type & ret) const
		{
			ret.first = a.first;
			ret.second = a.second.accelerometer;
		}
	};

	class IMUMagExtractor
	{
	public:
		bool verify(const IMU::IMUStream::value_type & a)
		{
			return true;
		}

		void extract(const IMU::IMUStream::value_type & a, Vec3Stream::value_type & ret) const
		{
			ret.first = a.first;
			ret.second = a.second.magnetometer;
		}
	};

	class IMUGyroExtractor
	{
	public:
		bool verify(const IMU::IMUStream::value_type & a)
		{
			return true;
		}

		void extract(const IMU::IMUStream::value_type & a, Vec3Stream::value_type & ret) const
		{
			ret.first = a.first;
			ret.second = a.second.gyroscope;
		}
	};

	class IMUOrientExtractor
	{
	public:
		bool verify(const IMU::IMUStream::value_type & a)
		{
			return true;
		}

		void extract(const IMU::IMUStream::value_type & a, QuatStream::value_type & ret) const
		{
			ret.first = a.first;
			ret.second = a.second.orientation;
		}
	};

	class IMUExtractor
	{
	public:

		IMUExtractor(const imuCostume::Costume::SensorID id) : id(id) {}
		~IMUExtractor() {}

		bool verify(const IMU::SensorsStreamData & a)
		{
			return a.sensorsData.find(id) != a.sensorsData.end();
		}

		void extract(const IMU::SensorsStreamData & a, IMU::IMUStream::value_type & ret) const
		{
			ret.first = a.timestamp;
			ret.second = a.sensorsData.find(id)->second;
		}

	private:

		const imuCostume::Costume::SensorID id;
	};
}

#endif	// __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__