/********************************************************************
	created:  2014/12/21	12:00:03
	filename: IMUCostumeCalibrationAlgorithm.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONALGORITHM_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONALGORITHM_H__

#include <string>
#include <vector>
#include <osg/Quat>
#include <osg/Vec3d>
#include <imucostumelib/ImuCostume.h>
#include <plugins/imuCostume/Types.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/LinearizedSkeleton.h>
#include <corelib/IIdentifiable.h>

class QWidget;

namespace IMU
{
	//! Interfejs algorytm�w kalibruj�cych kostium - znajduj�cych lokalne skr�cenia sensor�w na segmentach (jointach)
	class IMUCostumeCalibrationAlgorithm : public plugin::IIdentifiable
	{
	public:

		//! Struktura opisuj�ca poprawk� dla sensora IMU
		struct SensorAdjustment
		{
			//! Pre mul Lokalna rotacja wzgl�dem rodzica
			osg::Quat preMulRotation;
			//! Post mul
			osg::Quat postMulRotation;
			//! Przesuni�cie wzgl�dem rodzica w uk��dzie rodzica!
			osg::Vec3d offset;
		};

		//! Struktura opisuj�ca konfiguracj� sensor�w
		struct SensorDescription : public SensorAdjustment
		{
			//! Nazwa jointa na kt�rym umiejscowiony jest sensor
			std::string jointName;
			//! ID jointa w zlinearyzowanym szkielecie (lokalne, bez li�ci)
			kinematic::LinearizedSkeleton::NodeIDX jointIdx;
		};

		//! Mapa poprawek sensor�w: ID -> poprawka
		typedef std::map<imuCostume::Costume::SensorID, SensorAdjustment> SensorsAdjustemnts;

		//! Mapa poprawek sensor�w: ID -> opis sensora
		typedef std::map<imuCostume::Costume::SensorID, SensorDescription> SensorsDescriptions;

	public:
		//! Destruktor wirtualny
		virtual ~IMUCostumeCalibrationAlgorithm() {}

		//! \return Nowy algorytm kalibracji
		virtual IMUCostumeCalibrationAlgorithm * create() const = 0;
		
		//! \return Internal filter name
		virtual std::string name() const = 0;

		//! Resets algo internal state
		virtual void reset() = 0;

		//! \return Max number (n) of steps that algorithm might require to calibrate costume, 0 means no upper limit
		virtual unsigned int maxCalibrationSteps() const { return 0; }

		//! \param skeleton	Kalibrowany szkielet
		//! \param sensorsDescription Opis sensor�w
		virtual void initialize(kinematic::SkeletonConstPtr skeleton,
			const SensorsDescriptions & sensorsDescription) = 0;
		
		//! \param data Dane z IMU
		//! \param inDeltaT Czas od poprzedniej ramki danych
		//! \return Returns if calibration successful and sufficient.		
		virtual bool calibrate(const SensorsData & data, const double inDeltaT) = 0;

		//! \return Widget konfiguracyjny
		virtual QWidget* configurationWidget() { return nullptr; }

		//! \return Widget kalibracyjny (informacje o aktualnym stanie kalibracji, instrukcje dla usera)
		virtual QWidget* calibrationWidget() { return nullptr; }

		//! \return Dane kalibracyjne szkieletu, poprawki dla sensor�w
		virtual SensorsAdjustemnts sensorsAdjustemnts() const = 0;
	};

	DEFINE_SMART_POINTERS(IMUCostumeCalibrationAlgorithm);
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONALGORITHM_H__