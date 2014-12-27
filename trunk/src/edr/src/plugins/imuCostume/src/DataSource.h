/********************************************************************
	created:  2013/11/15
	created:  15:11:2013   13:54
	filename: DataSource.h
	author:   Mateusz Janiak

	purpose:  èrÛd≥o danych obs≥ugujπce kostiumy z IMU
	*********************************************************************/
#ifndef HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
#define HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__

#include <plugins/imuCostume/IIMUDataSource.h>
#include <plugins/imuCostume/Wrappers.h>
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


namespace IMU
{
	class IMUCostumeDataSource : public plugin::ISource, public IIMUDataSource
	{
		UNIQUE_ID("{441BB894-1019-4382-97EE-F18A511A49CB}");
		CLASS_DESCRIPTION("IMUCostume", "IMU Costume Data Source");

	private:

		struct SensorData
		{
			utils::shared_ptr<std::atomic<ConnectionStatus>> status;
			utils::shared_ptr<IMUStream> dataStream;
			std::vector<utils::shared_ptr<Vec3Stream>> vec3dStreams;
			utils::shared_ptr<QuatStream> orientationStream;
			std::list<utils::shared_ptr<ScalarStream>> scalarStreams;
		};

		struct CostumeData
		{
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			utils::shared_ptr<std::atomic<ConnectionStatus>> status;
			std::map<imuCostume::Costume::SensorID, SensorData> sensorsData;
			RawDataStreamPtr rawDataStream;
			CANopenFramesStreamPtr CANopenStream;	
			CostumeStreamPtr costumeStream;

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

		virtual const imuCostume::Costume::SensorsConfiguration & costumeConfiguration(const unsigned int idx) const override;
		virtual const imuCostume::CostumeRawIO::CostumeAddress costumeAddress(const unsigned int idx) const override;
		virtual const ConnectionStatus costumeStatus(const unsigned int idx) const override;
		virtual const RawDataStreamPtr costumeRawDataStream(const unsigned int idx) const override;
		virtual const core::VariantsList costumeData(const unsigned int idx) const override;

		virtual void loadCostume(const unsigned int idx) override;
		virtual void unloadCostume(const unsigned int idx) override;
		virtual const bool costumeLoaded(const unsigned int idx) const override;
		virtual const unsigned int costumesLoadedCount() const override;

		virtual void loadAllCostumes() override;
		virtual void unloadAllCostumes() override;

		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithm * algorithm) override;
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithm * algorithm) override;
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithm * algorithm) override;

		virtual std::list<IIMUOrientationEstimationAlgorithmConstPtr> orientationEstimationAlgorithms() const override;
		virtual std::list<IMUCostumeCalibrationAlgorithmConstPtr> calibrationAlgorithms() const override;
		virtual std::list<IMUCostumeMotionEstimationAlgorithmConstPtr> motionEstimationAlgorithms() const override;

	private:

		static void configureCostume(CostumeData & cd);
		static void refreshCostumeSensorsConfiguration(CostumeData & cd);

		static std::string sensorParameterName(const unsigned int idx);
		static std::string vectorParameterName(const unsigned int idx);

		void refreshData();

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

	private:
		//! Czy odúwieøamy dane
		std::atomic<bool> refreshData_;
		//! Obiekt synchronizujπcy
		mutable std::recursive_mutex synch;
		//! Obiekt synchronizujπcy aktualizacjÍ danych
		std::recursive_mutex updateSynch;
		//! Manager danych w pamiÍci
		core::IMemoryDataManager * memoryDM;
		//! Watek odswiezajacy dane
		core::ThreadPool::Thread refreshThread;
		//! Korzen drzewa dla analiz
		core::HierarchyItemPtr root;
		//! Dane strumieniowe
		core::HierarchyItemPtr streamItems;
		//! Dane nagrane
		core::HierarchyItemPtr recordedItems;

		std::map<std::string, CostumeData> costumesData;

		std::list<IIMUOrientationEstimationAlgorithmConstPtr> orientationEstimationAlgorithms_;
		std::list<IMUCostumeCalibrationAlgorithmConstPtr> calibrationAlgorithms_;
		std::list<IMUCostumeMotionEstimationAlgorithmConstPtr> motionEstimationAlgorithms_;
	};
}

#endif	//	HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
