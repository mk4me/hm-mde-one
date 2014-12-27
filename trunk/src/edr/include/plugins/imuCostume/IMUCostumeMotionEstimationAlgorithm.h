/********************************************************************
	created:  2014/12/21	12:00:30
	filename: IMUCostumeMotionEstimationAlgorithm.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__

#include <string>
#include <vector>
#include <osg/Quat>
#include <plugins/imuCostume/Types.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>

class QWidget;

namespace IMU
{
	class IMUCostumeMotionEstimationAlgorithm
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMUCostumeMotionEstimationAlgorithm() {}

		//! \return Nowy algorytm kalibracji
		virtual IMUCostumeMotionEstimationAlgorithm * create() const = 0;

		// Public Interface
		//! Returns internal filter name
		virtual std::string name() const = 0;

		//! Resets algo internal state
		virtual void reset() = 0;

		//! \param skeleton	Kalibrowany szkielet
		virtual void initialize(void * skeleton,
			const IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustment,
			const SensorsMapping & sensorsMapping) = 0;

		//! Calculates orientation from sensor fusion
		/*!
		\param data Dane z IMU
		\param inDeltaT Czas od poprzedniej ramki danych
		\return Returns Lokalne orientacje wszystkich jointów, bez end-sitów
		*/
		virtual std::vector<osg::Quat> estimate(const SensorsData & data, const double inDeltaT) = 0;

		// Not used
		virtual QWidget* configurationWidget() { return nullptr; }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
	};
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__