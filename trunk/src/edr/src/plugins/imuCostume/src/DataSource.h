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
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <utils/SamplesStatus.h>
#include "CostumeParser.h"

typedef threadingUtils::StreamAdapterT<IMU::CostumeStream::value_type, IMU::SensorsStreamData, IMU::CostumeIMUExtractor> ExtractedCostumeStreamAdapter;

namespace IMU
{
	DEFINE_SMART_POINTERS(ExtractedCostumeStreamAdapter);

	class IMUCostumeDataSource : public plugin::ISource, public IIMUDataSource
	{
		UNIQUE_ID("{441BB894-1019-4382-97EE-F18A511A49CB}");
		CLASS_DESCRIPTION("IMUCostume", "IMU Costume Data Source");

	private:

		struct SensorData
		{
			utils::shared_ptr<utils::SamplesStatus> samplesStatus;
			core::VariantsList domainData;
		};

		typedef std::map<imuCostume::Costume::SensorID, SensorData> SensorsData;

		struct CostumeData : public CostumeDescription
		{
			utils::shared_ptr<utils::SamplesStatus> samplesStatus;
			SensorsData sensorsData;
			CANopenFramesStreamPtr CANopenStream;
			CostumeStreamPtr costumeStream;
			ExtractedCostumeStreamAdapterPtr completeImuStream;

			CostumeSkeletonMotionPtr skeletonMotion;

			core::HierarchyItemPtr hierarchyRootItem;
			core::VariantsList domainData;		
		};

		typedef std::map<CostumeID, CostumeData> CostumesData;

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

		virtual const bool costumesEmpty() const override;

		virtual bool refreshCostumeSensorsConfiguration(const CostumeID & id, const uint8_t samplesCount) override;
		
		virtual void resetCostumeStatus(const CostumeID & id) override;

		virtual void resetSensorStatus(const CostumeID & costumeID,
			const imuCostume::Costume::SensorID sensorID) override;

		virtual CostumeStatus costumeStatus(const CostumeID & id) const override;

		virtual CostumeDetails costumeDetails(const CostumeID & id) const override;

		virtual CostumesDescriptions costumesDescriptions() const override;

		virtual CostumesStatus costumesStatus() const override;

		virtual CostumesDetails costumesDetails() const override;

		virtual CostumeDescription costumeDescription(const CostumeID & id) const override;

		virtual void loadRawCostume(const CostumeID & id) override;

		virtual void loadCalibratedCostume(const CostumeID & id,
			CostumeProfilePtr profile) override;

		virtual void unloadCostume(const CostumeID & id);

		virtual unsigned int loadedCostumesCount() const override;

		virtual bool costumeLoaded(const CostumeID & id) const override;

		virtual core::ConstVariantsList costumeData(const CostumeID & id) const override;

		virtual void startRecording(RecordingConfigurationPtr recording) override;
		virtual void stopRecording(RecordingConfigurationPtr recording) override;

		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithmPtr algorithm) override;
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithmPtr algorithm) override;
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithmPtr algorithm) override;
		virtual void registerSkeletonModel(SkeletonPtr skeleton) override;
		virtual void registerCostumeProfile(CostumeProfilePtr profile) override;
		
		virtual OrientationEstimationAlgorithms orientationEstimationAlgorithms() const override;
		virtual CostumeCalibrationAlgorithms calibrationAlgorithms() const override;
		virtual CostumeMotionEstimationAlgorithms motionEstimationAlgorithms() const override;
		virtual SkeletonModels skeletonModels() const override;
		virtual CostumesProfiles costumesProfiles() const override;

		hmdbCommunication::IHMDBShallowCopyContextPtr selectUploadContext() const;

		void uploadSession(const core::Filesystem::Path & configuration,
			const core::Filesystem::FilesList & recordings);

	private:

		static CostumeStatus innerCreateCostumeStatus(const IMUCostumeDataSource::CostumeData & cData);

		core::HierarchyItemPtr fillRawCostumeData(CostumeData & cData);

		static void configureCostume(CostumeDescription & cd);
		static bool innerRefreshCostumeSensorsConfiguration(CostumeData & data, const uint8_t MaxSamplesCount);

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

		void tryRecord(std::set<imuCostume::CostumeRawIO::CostumeAddress> &costumes);

		void updateSensorsStatus(const imuCostume::CostumeCANopenIO::Frame &frame, const uint16_t length, const CostumeData &cd);


		static void unpackSensorsStream(SensorsStreamPtr stream,
			SensorsData & sensorsData,
			core::IHierarchyItemPtr root,
			core::VariantsList & domainData,
			CostumeProfileConstPtr profile);

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

		CostumesData costumesData;
		std::set<RecordingConfigurationPtr> recordings;

		OrientationEstimationAlgorithms orientationEstimationAlgorithms_;
		CostumeCalibrationAlgorithms calibrationAlgorithms_;
		CostumeMotionEstimationAlgorithms motionEstimationAlgorithms_;
		SkeletonModels skeletonModels_;
		CostumesProfiles costumesProfiles_;
	};
}

#endif	//	HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
