/********************************************************************
	created:  2013/11/15
	created:  15:11:2013   13:54
	filename: DataSource.h
	author:   Mateusz Janiak

	purpose:  �r�d�o danych obs�uguj�ce kostiumy z IMU
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

		//! Inicjalizacja �r�d�a. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich �r�de�.
		virtual void init(core::IMemoryDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM);

		//! Zobacz podobn� metode dla IService
		//! P�na inicjalizacja �r�d�a, nast�puje po wczytaniu i inicjalizacji wszystkich innych �r�de�
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny spos�b zwalniac zasoby, maj�c na uwadze �e niekoniecznie wszystkie us�ugi i zasoby pobrane z zewn�trz s� jeszcze dost�pne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, kt�re uruchomi� serwis, mo�e tez zwalniac pamie� przydzielon� dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezale�niaj�cy dana us�ug� od pozosta�ych us�ug i przygotowuj�cy ja na usuni�cie
		virtual void finalize();

		//! Metoda aktualizuj�ca pochodzi z w�tku UI! Powinny tu by� realizowane lekkie operacje od�wie�ania widget�w!!
		//! Jako parametr dostajemy przyrost czasu jaki mina� od poprzedniego wywo�ania
		virtual void update(double deltaTime);

		//! �r�d�o nie musi mie� wizualnej reprezentacji.
		//! \return Widget tworzony przez �r�d�o b�d� NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontroluj�cy zachowanie us�ugi/us�ug zale�nych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to �r�d�o
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
		//! Czy od�wie�amy dane
		std::atomic<bool> refreshData_;
		//! Obiekt synchronizuj�cy
		mutable std::recursive_mutex synch;
		//! Obiekt synchronizuj�cy aktualizacj� danych
		std::recursive_mutex updateSynch;
		//! Manager danych w pami�ci
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
