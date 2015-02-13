/********************************************************************
	created:  2015/02/12	12:38:39
	filename: CostumeProfile.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMEPROFILE_H__
#define __HEADER_GUARD_IMU__COSTUMEPROFILE_H__

#include <string>
#include <map>
#include <plugins/imuCostume/Types.h>
#include <kinematiclib/Skeleton.h>
#include <imucostumelib/ImuCostume.h>
#include <corelib/IIdentifiable.h>
#include <plugins/imuCostume/IIMUOrientationEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <plugins/imuCostume/Export.h>

namespace IMU
{
	class IIMUDataSource;

	struct IMU_EXPORT Skeleton : public kinematic::Skeleton, public plugin::IIdentifiable
	{
		const core::UniqueID id;

		Skeleton(const Skeleton & skeleton);
		Skeleton(Skeleton && skeleton);
		Skeleton(const core::UniqueID id, const kinematic::Skeleton & skeleton);
		Skeleton(const core::UniqueID id, kinematic::Skeleton && skeleton);
		virtual ~Skeleton();

		virtual const core::UniqueID ID() const override;
	};

	DEFINE_SMART_POINTERS(Skeleton);

	//! Profil kostiumu
	struct CostumeProfile
	{
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationAlgorithmsMapping;

		//! Nazwa profilu
		std::string name;
		//! Szkielet
		SkeletonConstPtr skeleton;
		//! Mapowanie sensor�w
		SensorsMapping sensorsMapping;
		//! Algorytm kalibracji
		IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithm;
		//! Poprawki sensor�w
		IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
		//! Algorytmy estymacji orientacji sensor�w			
		OrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
		//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchi�, ograniczneia staw�w, ...)
		IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithm;
	};

	//! Profil kostiumu
	struct IMU_EXPORT CostumeProfileInstance
	{
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;

		//! \param profil Profil z kt�rego tworzymy instancj�
		//! \return instancja profilu
		static CostumeProfileInstance create(const CostumeProfile & profile);

		//! Nazwa profilu
		std::string name;
		//! Szkielet
		SkeletonPtr skeleton;
		//! Mapowanie sensor�w
		SensorsMapping sensorsMapping;
		//! Algorytm kalibracji
		IMUCostumeCalibrationAlgorithmPtr calibrationAlgorithm;
		//! Poprawki sensor�w
		IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
		//! Algorytmy estymacji orientacji sensor�w			
		OrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
		//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchi�, ograniczneia staw�w, ...)
		IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;
	};

	struct IMU_EXPORT SerializableCostumeProfile
	{
		typedef std::map<imuCostume::Costume::SensorID, core::UniqueID> OrientationEstimationAlgorithmsMapping;

		enum VerificationStatus
		{
			Ok,
			Incomplete,
			Failed
		};

		//! Nazwa profilu
		std::string name;
		//! Szkielet
		core::UniqueID skeletonID;
		//! Mapowanie sensor�w
		IMU::SensorsMapping sensorsMapping;
		//! Algorytm kalibracji
		core::UniqueID calibrationAlgorithmID;
		//! Poprawki sensor�w
		IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustments;
		//! Algorytmy estymacji orientacji sensor�w			
		OrientationEstimationAlgorithmsMapping sensorsOrientationEstimationAlgorithms;
		//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchi�, ograniczneia staw�w, ...)
		core::UniqueID motionEstimationAlgorithmID;

		static SerializableCostumeProfile pack(const CostumeProfile & profile);
		static CostumeProfile unpack(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds);
		static VerificationStatus verify(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds);
		static void serialize(std::ostream & stream, const SerializableCostumeProfile & sProfile);
		static SerializableCostumeProfile deserialize(std::istream & stream);
	};
}

#endif	// __HEADER_GUARD_IMU__COSTUMEPROFILE_H__