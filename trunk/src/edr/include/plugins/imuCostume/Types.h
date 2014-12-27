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
#include <list>

namespace IMU
{
	//! Mapa identyfikatorów sensorów do nazw jointów z którymi s¹ skojarzone, jeden sensor, jeden joint, ale joint moze miec wiêcej sensorów!!
	typedef boost::bimap<boost::bimaps::set_of<imuCostume::Costume::SensorID>, boost::bimaps::multiset_of<std::string>> SensorsMapping;

	//! Struktura opissuj¹ca orientacje sensora
	struct SensorData
	{
		imuCostume::Costume::SensorID id;
		osg::Quat orientation;
		osg::Vec3 accelerometer;
		osg::Vec3 gyroscope;
		osg::Vec3 magnetometer;
	};

	//! Agregacja danych z sensorów
	typedef std::list<SensorData> SensorsData;
}

#endif	// __HEADER_GUARD_IMU__TYPES_H__