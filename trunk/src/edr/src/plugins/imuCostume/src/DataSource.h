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
#include <corelib/ISource.h>
#include <corelib/IThread.h>
#include <utils/StreamData.h>
#include <osg/Vec3>
#include <kinematiclib/hAnimSkeleton.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <threading/SynchronizationPolicies.h>

namespace IMU
{
	class IMUCostumeDataSource : public plugin::ISource
	{

		UNIQUE_ID("{441BB894-1019-4382-97EE-F18A511A49CB}");
		CLASS_DESCRIPTION("IMUCostume", "IMU Costume Data Source");

	public:
		//! Struktura opisuj�ca konfiguracj� kostiumu
		struct CostumeConfiguration
		{
			std::string name;						//! Nazwa
			unsigned int id;						//! Identyfikator
			kinematic::hAnimSkeletonPtr skeleton;	//! Szkielet
			unsigned int IMUcount;					//! Ilo�� sensor�w
			unsigned int jointsCount;				//! Ilo�� joint�w
		};

		//! Wyliczenie opisuj�ce status danych kostiumu
		enum CostumeDataStatus {
			NODATA,	//! Inicjalny status kiedy jeszcze nic nie odebrano
			OK,		//! Odebrano dane
			ERROR	//! B��d odbioru danych
		};

	private:

		struct CostumeRawData
		{
			kinematic::SkeletalDataStreamPtr skeletalDataStream;
			core::shared_ptr<kinematic::JointAnglesCollectionStream> skeletonStream;
			IMUsStreamPtr imuDataStream;
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
		virtual void getOfferedTypes(core::TypeInfoList & offeredTypes) const;

		const unsigned int costumesCout() const;

		const CostumeConfiguration & costumeConfiguration(const unsigned int idx) const;

		const CostumeDataStatus costumeDataStatus(const unsigned int idx) const;
		const std::vector<CostumeDataStatus> costumesDataStatus() const;

		void loadCostume(const unsigned int idx);
		void unloadCostume(const unsigned int idx);
		const bool costumeLoaded(const unsigned int idx) const;
		const unsigned int costumesLoadedCount() const;

		void loadAllCostumes();
		void unloadAllCostumes();

		void connectCostiumes();

		void disconnectCostiumes();

		const bool connected() const;

	private:

		void refreshData();

		void innerLoadCostume(const unsigned int idx);
		void innerUnloadCostume(const unsigned int idx);

		void addToUpdate(const unsigned int idx,
			const CostumeRawData & rd,
			const core::ObjectsList & ol);

		void removeFromUpdate(const unsigned int idx);

	private:
		//! Czy od�wie�amy dane
		volatile bool refreshData_;
		//! Czy po��czono ju� z kostiumami
		bool connected_;
		//! Obiekt synchronizuj�cy
		mutable utils::RecursiveSyncPolicy synch;
		//! Obiekt synchronizuj�cy aktualizacj� danych
		utils::RecursiveSyncPolicy updateSynch;
		//! Manager danych w pami�ci
		core::IMemoryDataManager * memoryDM;
		//! Watek odswiezajacy dane
		core::IThreadPtr refreshThread;
		//! Kostiumy
		std::vector<CostumeConfiguration> costumesConfigurations;
		//! Status danych
		std::vector<CostumeDataStatus> costumesDataStatus_;
		//! Mapowanie kostiumu do dostarczanych danych
		std::map<unsigned int, core::ObjectsList> coreData;
		//! Mapowanie kostiumu do danych surowych
		std::map<unsigned int, CostumeRawData> rawData;
	};
}

#endif	//	HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
