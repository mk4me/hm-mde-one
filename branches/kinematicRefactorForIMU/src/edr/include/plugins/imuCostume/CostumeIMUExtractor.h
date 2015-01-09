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
	//! Klasa realizuj¹ca filtracjê kompletnych danych IMU (pe³ne dane dla wszystkich czujników IMU)
	class IMU_EXPORT CostumeCompleteDataFilter
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeCompleteDataFilter(CostumeCompleteDataFilter && Other);
		//! \param Other Obiekt kopiowany
		CostumeCompleteDataFilter(const CostumeCompleteDataFilter & Other);
		//! \param sensorsIDs Identyfikatory sensorów IMU
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensorów
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Desturktor
		~CostumeCompleteDataFilter();
		//! \param dataToFilter Dane do filtracji
		//! \return Prawda jeœli dane s¹ kompletne
		bool operator()(const IMU::CostumeStream::value_type & dataToFilter) const;

	private:
		//! Zbiór identyfikatorów sensorów IMU
		imuCostume::Costume::SensorIDsSet sensorsIDs;
	};

	//! Klasa realizuj¹ca konwersjê ze strumienia danych czujników do kompletnych danych IMU
	class IMU_EXPORT CostumeIMUExtractor
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeIMUExtractor(CostumeIMUExtractor && Other);
		//! \param Other Obiekt kopiowany
		CostumeIMUExtractor(const CostumeIMUExtractor & Other);
		//! \param sensorsIDs Identyfikatory czujników IMU
		CostumeIMUExtractor(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensorów
		CostumeIMUExtractor(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Destruktor
		~CostumeIMUExtractor();
		//! \param dataToVerify Dane do veryfikacji pd k¹tem mo¿liwoœci konwersji
		//! \return Prawda jeœli dane mo¿na konwertowaæ
		bool verify(const IMU::CostumeStream::value_type & dataToVerify) const;
		//! \param convertedData [out] Dane po konwersji
		void extract(const IMU::CostumeStream::value_type & dataToConvert, IMU::SensorsStreamData & convertedData) const;

	private:
		//! Obiekt realizuj¹cy weryfikacjê
		CostumeCompleteDataFilter completeDataFilter;
	};
}

#endif	// __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__