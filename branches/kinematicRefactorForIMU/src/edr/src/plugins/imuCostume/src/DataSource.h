/********************************************************************
	created:  2013/11/15
	created:  15:11:2013   13:54
	filename: DataSource.h
	author:   Mateusz Janiak

	purpose:  �r�d�o danych obs�uguj�ce kostiumy z IMU
	*********************************************************************/
#ifndef HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
#define HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__

#include <plugins/imuCostume/Wrappers.h>
#include <corelib/Variant.h>
#include <corelib/ISource.h>
#include <threadingUtils/StreamData.h>
#include <osg/Vec3>
#include <mutex>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <corelib/HierarchyItem.h>
#include <corelib/ThreadPool.h>
#include <corelib/HierarchyDataItem.h>


namespace IMU
{
	class IMUCostumeDataSource : public plugin::ISource
	{
		UNIQUE_ID("{441BB894-1019-4382-97EE-F18A511A49CB}");
		CLASS_DESCRIPTION("IMUCostume", "IMU Costume Data Source");

	public:

		enum 
		{
			AccIdx = 0,
			GyroIdx = 1,
			MagIdx = 2
		};

		enum ConnectionStatus
		{
			ONLINE,
			OFFLINE,
			CONNECTION_PROBLEMS,
			UNKNOWN
		};

		typedef threadingUtils::StreamT<imuCostume::ProtocolSendBufferHelper::Buffer> RawDataStream;

		DEFINE_SMART_POINTERS(RawDataStream);

		typedef threadingUtils::StreamT<imuCostume::CostumeCANopenIO::Data> CANopenFramesStream;

		DEFINE_SMART_POINTERS(CANopenFramesStream);

		typedef threadingUtils::IStreamT<imuCostume::Costume::Data> CostumeStream;

		DEFINE_SMART_POINTERS(CostumeStream);

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

		const bool refreshCostumes();

		const unsigned int costumesCout() const;

		const imuCostume::Costume::SensorsConfiguration & costumeConfiguration(const unsigned int idx) const;
		const imuCostume::CostumeRawIO::CostumeAddress costumeAddress(const unsigned int idx) const;
		const ConnectionStatus costumeStatus(const unsigned int idx) const;
		const RawDataStreamPtr costumeRawDataStream(const unsigned int idx) const;
		const core::VariantsList costumeData(const unsigned int idx) const;

		void loadCostume(const unsigned int idx);
		void unloadCostume(const unsigned int idx);
		const bool costumeLoaded(const unsigned int idx) const;
		const unsigned int costumesLoadedCount() const;

		void loadAllCostumes();
		void unloadAllCostumes();

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
	};
}

DEFINE_WRAPPER(threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER_INHERITANCE(IMU::IMUCostumeDataSource::RawDataStream, threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>);
DEFINE_WRAPPER(IMU::IMUCostumeDataSource::CANopenFramesStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::IMUCostumeDataSource::CostumeStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif	//	HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
