/********************************************************************
	created:  2013/11/15
	created:  15:11:2013   13:54
	filename: DataSource.h
	author:   Mateusz Janiak

	purpose:  èrÛd≥o danych obs≥ugujπce kostiumy z IMU
	*********************************************************************/
#ifndef HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
#define HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__

#include <corelib/ISource.h>
#include <osg/Vec3>
#include <mutex>
#include <list>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <corelib/HierarchyItem.h>
#include <corelib/ThreadPool.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/imuCostume/CostumeIMUExtractor.h>
#include <utils/SamplesStatus.h>

typedef threadingUtils::StreamAdapterT<IMU::CostumeStream::value_type, IMU::SensorsStreamData, IMU::CostumeIMUExtractor> ExtractedCostumeStreamAdapter;

namespace IMU
{
	DEFINE_SMART_POINTERS(ExtractedCostumeStreamAdapter);

	class IMUCostumeDataSource : public plugin::ISource, public IIMUDataSource
	{
		UNIQUE_ID("{441BB894-1019-4382-97EE-F18A511A49CB}");
		CLASS_DESCRIPTION("IMUCostume", "IMU Costume Data Source");

	private:

		struct ExtendedCostumeDescription : public CostumeDescription
		{
			ExtendedCostumeDescription();
			~ExtendedCostumeDescription();

			utils::SamplesStatus samplesStatus;
			std::map<imuCostume::Costume::SensorID, utils::SamplesStatus> sensorsSamplesStatus;
		};

		struct SensorData
		{
			utils::shared_ptr<IMUStream> dataStream;
			std::vector<utils::shared_ptr<Vec3Stream>> vec3dStreams;
			utils::shared_ptr<QuatStream> orientationStream;
			std::list<utils::shared_ptr<ScalarStream>> scalarStreams;
		};

		struct CostumeData
		{
			std::map<imuCostume::Costume::SensorID, SensorData> sensorsData;
			CANopenFramesStreamPtr CANopenStream;
			CostumeStreamPtr costumeStream;
			ExtractedCostumeStreamAdapterPtr completeImuStream;

			CostumeSkeletonMotionPtr skeletonMotion;

			core::HierarchyDataItemPtr hierarchyRootItem;
			core::VariantsList domainData;			
		};

	public:
		//! Konstruktor
		IMUCostumeDataSource();
		//! Destruktor wirtualny
		virtual ~IMUCostumeDataSource();

		//! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
		virtual void init(core::IMemoryDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM);

		//! Zobacz podobnπ metode dla IService
		//! PÛüna inicjalizacja ürÛd≥a, nastÍpuje po wczytaniu i inicjalizacji wszystkich innych ürÛde≥
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny sposÛb zwalniac zasoby, majπc na uwadze øe niekoniecznie wszystkie us≥ugi i zasoby pobrane z zewnπtrz sπ jeszcze dostÍpne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, ktÛre uruchomi≥ serwis, moøe tez zwalniac pamieÊ przydzielonπ dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezaleøniajπcy dana us≥ugÍ od pozosta≥ych us≥ug i przygotowujπcy ja na usuniÍcie
		virtual void finalize();

		//! Metoda aktualizujπca pochodzi z wπtku UI! Powinny tu byÊ realizowane lekkie operacje odúwieøania widgetÛw!!
		//! Jako parametr dostajemy przyrost czasu jaki mina≥ od poprzedniego wywo≥ania
		virtual void update(double deltaTime);

		//! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
		//! \return Widget tworzony przez ürÛd≥o bπdü NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontrolujπcy zachowanie us≥ugi/us≥ug zaleønych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczajπcy opcji zwiπzanych z us≥ugπ/us≥ugami zaleønymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to ürÛd≥o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;

		virtual const bool refreshCostumes() override;

		virtual const unsigned int costumesCout() const override;

		virtual const CostumeDescription & costumeDescription(const unsigned int idx) const override;

		virtual void loadRawCostume(const unsigned int idx) override;

		virtual void loadCalibratedCostume(const unsigned int idx,
			const CostumeProfileInstance & profileInstance) override;

		virtual void unloadCostume(const unsigned int idx);

		virtual unsigned int loadedCostumesCount() const override;

		virtual bool costumeLoaded(const unsigned int idx) const override;

		virtual core::ConstVariantsList costumeData(const unsigned int idx) const override;

		virtual void registerOrientationEstimationAlgorithm(const IIMUOrientationEstimationAlgorithm * algorithm) override;
		virtual void registerCostumeCalibrationAlgorithm(const IMUCostumeCalibrationAlgorithm * algorithm) override;
		virtual void registerMotionEstimationAlgorithm(const IMUCostumeMotionEstimationAlgorithm * algorithm) override;
		virtual void registerSkeletonModel(kinematic::SkeletonConstPtr skeleton) override;
		virtual void registerCostumeProfile(const CostumeProfile & profile) override;
		
		virtual OrientationEstimationAlgorithms orientationEstimationAlgorithms() const override;
		virtual CostumeCalibrationAlgorithms calibrationAlgorithms() const override;
		virtual CostumeMotionEstimationAlgorithms motionEstimationAlgorithms() const override;
		virtual SkeletonModels skeletonModels() const override;
		virtual CostumesProfiles costumesProfiles() const override;

	private:

		void fillRawCostumeData(CostumeData & cData, const CostumeDescription & cd);

		static void configureCostume(CostumeDescription & cd);
		static void refreshCostumeSensorsConfiguration(CostumeDescription & cd);

		static std::string sensorParameterName(const unsigned int idx);
		static std::string vectorParameterName(const unsigned int idx);

		void innerLoadCostume(const unsigned int idx);
		void innerUnloadCostume(const unsigned int idx);		

		static core::HierarchyItemPtr createRootItem();
		static core::HierarchyItemPtr createStreamItem();
		static core::HierarchyItemPtr createRecordItem();
		static core::HierarchyItemPtr createIMUsItem();

		void generateCostumeItem(const unsigned int idx,
			const core::VariantsList & data,
			core::HierarchyItemPtr parent);

		void tryCreateRootItem();

		void tryCreateStreamItem();
		void tryCreateRecordedItem();

		void resfreshCostumesData();

	private:

		//! Czy koÒczymy dzia≥anie wπtku odúwieøajπcego dane
		volatile bool finish;
		//! Wπtek odúwieøajπcy dane i status kostiumÛw
		core::Thread refreshThread;
		//! Obiekt synchronizujπcy
		mutable std::recursive_mutex synch;		
		//! Manager danych w pamiÍci
		core::IMemoryDataManager * memoryDM;
		//! Korzen drzewa dla analiz
		core::HierarchyItemPtr root;
		//! Dane strumieniowe
		core::HierarchyItemPtr streamItems;
		//! Dane nagrane
		core::HierarchyItemPtr recordedItems;

		std::map<std::string, CostumeData> costumesData;
		std::map<std::string, ExtendedCostumeDescription> costumesDescription;

		OrientationEstimationAlgorithms orientationEstimationAlgorithms_;
		CostumeCalibrationAlgorithms calibrationAlgorithms_;
		CostumeMotionEstimationAlgorithms motionEstimationAlgorithms_;
		SkeletonModels skeletonModels_;
		CostumesProfiles costumesProfiles_;
	};
}

#endif	//	HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
