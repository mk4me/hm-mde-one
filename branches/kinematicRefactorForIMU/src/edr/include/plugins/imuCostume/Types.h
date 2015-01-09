/********************************************************************
	created:  2014/12/21	13:24:57
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__TYPES_H__
#define __HEADER_GUARD_IMU__TYPES_H__

#include <imucostumelib/ImuCostume.h>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <osg/Quat>
#include <osg/Vec3d>
#include <map>

namespace IMU
{
	//! Mapa identyfikatorów sensorów do nazw jointów z którymi s¹ skojarzone, jeden sensor, jeden joint, ale joint moze miec wiêcej sensorów!!
	typedef boost::bimap<boost::bimaps::set_of<imuCostume::Costume::SensorID>, boost::bimaps::multiset_of<std::string>> SensorsMapping;

	//! Mapa indeksów orientacji i jointów którym odpowiadaj¹ te oreintacje
	typedef boost::bimap<unsigned int, std::string> DataIndexToJointMapping;

	//! Struktura opisuj¹ca orientacje sensora
	struct SensorData
	{
		//! Orientacja
		osg::Quat orientation;
		//! Akcelerometr
		osg::Vec3d accelerometer;
		//! ¯yroskop
		osg::Vec3d gyroscope;
		//! Magnetometr
		osg::Vec3d magnetometer;
	};

	//! Agregacja danych z sensorów
	typedef std::map<imuCostume::Costume::SensorID, SensorData> SensorsData;

	//! Struktura opisuj¹ca dane strumienia z danymi czujników
	struct SensorsStreamData
	{
		//! Stempel czasu danych
		imuCostume::CostumeCANopenIO::Timestamp timestamp;
		//! Dane sensorów
		SensorsData sensorsData;
	};
}

#endif	// __HEADER_GUARD_IMU__TYPES_H__