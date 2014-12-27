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
#include <osg/Vec3>
#include <imucostumelib/ImuCostume.h>
#include <plugins/imuCostume/Types.h>
#include <kinematiclib/Skeleton.h>

class QWidget;

namespace IMU
{
	class IMUCostumeCalibrationAlgorithm
	{
	public:

		//! Struktura opisuj¹ca poprawkê dla sensora IMU
		struct SensorAdjustment
		{
			osg::Quat rotation;
			osg::Vec3 offset;
		};

		//! Mapa poprawek sensorów
		typedef std::map<imuCostume::Costume::SensorID, SensorAdjustment> SensorsAdjustemnts;

	public:
		//! Destruktor wirtualny
		virtual ~IMUCostumeCalibrationAlgorithm() {}

		//! \return Nowy algorytm kalibracji
		virtual IMUCostumeCalibrationAlgorithm * create() const = 0;

		// Public Interface
		//! Returns internal filter name
		virtual std::string name() const = 0;

		//! Resets algo internal state
		virtual void reset() = 0;

		//! Returns max number (n) of steps that algorithm might require to calibrate costume, 0 means no upper limit
		virtual unsigned int maxCalibrationSteps() const { return 0; }

		//! \param skeleton	Kalibrowany szkielet
		//! \param mapping Mapowanie sensorów do szkieletu
		//! \param sensorsAdjustment Wstêpne ustawienie sensorów - pozwala zadaæ stan pocz¹tkowy bardziej zbli¿ony do rzeczywistoœci
		//! \param calibrationData Dane kalibracyjne
		virtual void initialize(void * skeleton, const SensorsMapping & mapping,
			const SensorsAdjustemnts  & sensorsAdjustment) = 0;

		//! Calculates orientation from sensor fusion
		/*!
		\param data Dane z IMU
		\param inDeltaT Czas od poprzedniej ramki danych
		\return Returns if calibration successful and sufficient.
		*/
		virtual bool calibrate(const SensorsData & data, const double inDeltaT) = 0;

		// Not used
		virtual QWidget* calibrationWidget() { return nullptr; }

		//! \return Dane kalibracyjne szkieletu, poprawki dla sensorów
		virtual SensorsAdjustemnts sensorsAdjustemnts() const = 0;
	};
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONALGORITHM_H__