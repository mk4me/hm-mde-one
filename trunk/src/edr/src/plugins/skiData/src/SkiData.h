/********************************************************************
	created:  2016/07/18
	filename: SkiData.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIDATA_H__
#define __HEADER_GUARD_SKIDATA__SKIDATA_H__

#include <osg/Vec3>
#include <vector>
#include <objectwrapperlib/ObjectWrapper.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>

namespace skidata
{
	//! Dane IMU
	struct RawIMU
	{
		//! Akcelerometr [m/s^2]
		osg::Vec3 accelerometer;
		//! �yroskop [deg/s]
		osg::Vec3 gyroscope;
		//! Magnetometr [uT]
		osg::Vec3 magnetometer;
	};

	//! Przetworzone dane IMU - rozszerzone o orientacj�
	struct ProcessedIMU : public RawIMU
	{
		//! Orientacja jako 3 k�ty Eulera [deg]
		osg::Vec3 orientation;
	};

	//! Wszystkie dane zbierane z IMU na ca�ym narciarzu
	struct IMUData
	{
		//! Dane lewej narty
		ProcessedIMU leftSki;
		//! Dane prawej narty
		ProcessedIMU rightSki;
		//! Dane klatki piersiowej
		ProcessedIMU chest;
	};

	//! Dane pozycji z GPS
	struct GPSPosition
	{
		//! D�ugo�� georgraficzna [stopnie]
		float longnitude;
		//! Szeroko�� georgraficzna [stopnie]
		float latitude;
		//! Wysoko�� [m]
		float height;
		//! Kierunek (odchylenie od p�nocy) [deg]
		float heading;
	};

	//! Dane pr�dko�ci z GPS
	struct GPSSpeed
	{
		//! Pr�dko�� 2D [m/s]
		float speed2D;
		//! Pr�dko�� 3D [m/s]
		float speed3D;
	};

	//! Dane z GPS
	struct GPSData
	{
		//! Pozycja
		GPSPosition position;
		//! Pr�dko��
		GPSSpeed speed;
	};

	//! Dane �rodowiska
	struct EnvironmentData
	{
		//! Temperature [deg C]
		float temperature;
		//! Ci�nienie [hP]
		float preasure;
	};

	//! Ramka danych opisuj�ca zjazd
	struct SkiDataFrame
	{
		//! Dane z IMU
		IMUData imu;
		//! Dane z GPS
		GPSData gps;
		//! Dane �rodowiska
		EnvironmentData environment;
	};

	//! Wektor danych z nart
	using SkiData = std::vector<SkiDataFrame>;
}

DEFINE_WRAPPER(skidata::SkiData, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

#endif  // __HEADER_GUARD_SKIDATA__SKIDATA_H__