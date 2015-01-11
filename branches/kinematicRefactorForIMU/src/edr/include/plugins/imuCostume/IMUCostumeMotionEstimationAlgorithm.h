/********************************************************************
	created:  2014/12/21	12:00:30
	filename: IMUCostumeMotionEstimationAlgorithm.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__

#include <string>
#include <map>
#include <osg/Quat>
#include <plugins/imuCostume/Types.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>
#include <kinematiclib/Skeleton.h>
#include <corelib/IIdentifiable.h>

class QWidget;

namespace IMU
{
	//! Interfejs algorytm�w poprawiaj�cych og�ln� estymacj� pozy i pozycji szkieletu (uwzl�gniaj�c ograniczenia, hierarchi�, ...)
	class IMUCostumeMotionEstimationAlgorithm : public plugin::IIdentifiable
	{
	public:

		//! Opis stanu ruchu
		struct MotionState 
		{
			//! Mapa joint�w do ich orientacji lokalnych
			std::map<std::string, osg::Quat> jointsOrientations;
			//! Pozycja szkieletu
			osg::Vec3d position;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IMUCostumeMotionEstimationAlgorithm() {}

		//! \return Nowy algorytm kalibracji
		virtual IMUCostumeMotionEstimationAlgorithm * create() const = 0;
		
		//! Returns internal filter name
		virtual std::string name() const = 0;

		//! Resets algo internal state
		virtual void reset() = 0;

		//! \param skeleton	Kalibrowany szkielet
		//! \param sensorsAdjustment Poprawki dla sensor�w
		//! \param sensorsMapping Mapowanie sensor�w do joint�w
		virtual void initialize(kinematic::SkeletonConstPtr skeleton,
			const IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts & sensorsAdjustment,
			const SensorsMapping & sensorsMapping) = 0;
		
		//! \param motionState Aktualny stan ruchu szkieletu
		//! \param data Kompletne rozpakowane dane z IMU
		//! \param inDeltaT Czas od poprzedniej ramki danych
		//! \return Returns Lokalne orientacje wszystkich joint�w, bez end-sit�w (joint�w bez dzieci)
		virtual MotionState estimate(const MotionState & motionState,
			const SensorsData & data, const double inDeltaT) = 0;

		//! \return Widget konfiguruj�cy algorytm
		virtual QWidget* configurationWidget() { return nullptr; }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
	};

	DEFINE_SMART_POINTERS(IMUCostumeMotionEstimationAlgorithm);
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__