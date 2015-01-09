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
}

#endif	// __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__