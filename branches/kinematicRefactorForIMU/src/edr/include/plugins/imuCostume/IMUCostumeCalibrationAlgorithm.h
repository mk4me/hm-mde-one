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
#include <corelib/IIdentifiable.h>

class QWidget;

namespace IMU
{
	//! Interfejs algorytmów kalibruj¹cych kostium - znajduj¹cych lokalne skrêcenia sensorów na segmentach (jointach)
	class IMUCostumeCalibrationAlgorithm : public plugin::IIdentifiable
	{
	public:

		//! Struktura opisuj¹ca poprawkê dla sensora IMU
		struct SensorAdjustment
		{
			//! Lokalna rotacja wzglêdem rodzica
			osg::Quat rotation;
			//! Przesuniêcie wzglêdem rodzica w uk³¹dzie rodzica!
			osg::Vec3d offset;
		};

		//! Mapa poprawek sensorów: ID -> poprawka
		typedef std::map<imuCostume::Costume::SensorID, SensorAdjustment> SensorsAdjustemnts;

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
		//! \param mapping Mapowanie sensorów do szkieletu
		//! \param sensorsAdjustment Wstêpne ustawienie sensorów - pozwala zadaæ stan pocz¹tkowy bardziej zbli¿ony do rzeczywistoœci		
		virtual void initialize(kinematic::SkeletonConstPtr skeleton, const SensorsMapping & mapping,
			const SensorsAdjustemnts  & sensorsAdjustment = SensorsAdjustemnts()) = 0;
		
		//! \param data Dane z IMU
		//! \param inDeltaT Czas od poprzedniej ramki danych
		//! \return Returns if calibration successful and sufficient.		
		virtual bool calibrate(const SensorsData & data, const double inDeltaT) = 0;

		//! \return Widget konfiguracyjny
		virtual QWidget* configurationWidget() { return nullptr; }

		//! \return Widget kalibracyjny (informacje o aktualnym stanie kalibracji, instrukcje dla usera)
		virtual QWidget* calibrationWidget() { return nullptr; }

		//! \return Dane kalibracyjne szkieletu, poprawki dla sensorów
		virtual SensorsAdjustemnts sensorsAdjustemnts() const = 0;
	};

	DEFINE_SMART_POINTERS(IMUCostumeCalibrationAlgorithm);
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONALGORITHM_H__