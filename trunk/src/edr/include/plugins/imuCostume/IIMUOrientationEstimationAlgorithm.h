/********************************************************************
	created:  2014/12/03	12:27:42
	filename: IIMUOrientationEstimationAlgorithm.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IIMUORIENTATIONESTIMATIONALGORITHM_H__
#define __HEADER_GUARD_IMU__IIMUORIENTATIONESTIMATIONALGORITHM_H__

#include <string>
#include <osg/Quat>
#include <osg/Vec3>

class QWidget;

namespace IMU
{
	//! Generic quaternion-based orientation filter - generates orientation as a quaternion using IMU sensor fusion
	class IIMUOrientationEstimationAlgorithm
	{
	public:
		//! Make it polymorphic
		virtual ~IIMUOrientationEstimationAlgorithm() {}

		//! \return Nowy algorytm estymacji
		virtual IIMUOrientationEstimationAlgorithm * create() const = 0;

		// Public Interface
		//! Returns internal filter name
		virtual std::string name() const = 0;

		//! Resets filter internal state
		virtual void reset() = 0;

		//! Returns number (n) of frames that are probably unstable after filter create() / reset() - call estimate() at least (n) times
		virtual unsigned int approximateEstimationDelay() const { return 0; }

		//! Calculates orientation from sensor fusion
		/*!
		\param inAcc accelerometer vector from IMU
		\param inGyro gyroscope vector from IMU
		\param inMag magnetometer vector from IMU
		\param inDeltaT time between acquisitions in seconds [s] from IMU sensor
		\return Returns estimated orientation.
		*/
		virtual osg::Quat estimate(const osg::Vec3& inAcc,
			const osg::Vec3& inGyro, const osg::Vec3& inMag,
			const double inDeltaT) = 0;

		// Not used
		virtual QWidget* configurationWidget() { return nullptr; }
	};
}

#endif	// __HEADER_GUARD_IMU__IIMUORIENTATIONESTIMATIONALGORITHM_H__