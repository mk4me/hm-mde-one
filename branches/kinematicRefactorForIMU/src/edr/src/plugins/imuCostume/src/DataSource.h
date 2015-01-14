/********************************************************************
	created:  2013/11/15
	created:  15:11:2013   13:54
	filename: DataSource.h
	author:   Mateusz Janiak

	purpose:  �r�d�o danych obs�uguj�ce kostiumy z IMU
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

		//! Czy ko�czymy dzia�anie w�tku od�wie�aj�cego dane
		volatile bool finish;
		//! W�tek od�wie�aj�cy dane i status kostium�w
		core::Thread refreshThread;
		//! Obiekt synchronizuj�cy
		mutable std::recursive_mutex synch;		
		//! Manager danych w pami�ci
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
