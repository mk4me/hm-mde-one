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
	//! Mapa identyfikator�w sensor�w do nazw joint�w z kt�rymi s� skojarzone, jeden sensor, jeden joint, ale joint moze miec wi�cej sensor�w!!
	typedef boost::bimap<boost::bimaps::set_of<imuCostume::Costume::SensorID>, boost::bimaps::multiset_of<std::string>> SensorsMapping;

	//! Mapa indeks�w orientacji i joint�w kt�rym odpowiadaj� te oreintacje
	typedef boost::bimap<unsigned int, std::string> DataIndexToJointMapping;

	//! Struktura opisuj�ca orientacje sensora
	struct SensorData
	{
		//! Orientacja
		osg::Quat orientation;
		//! Akcelerometr
		osg::Vec3d accelerometer;
		//! �yroskop
		osg::Vec3d gyroscope;
		//! Magnetometr
		osg::Vec3d magnetometer;
	};

	//! Agregacja danych z sensor�w
	typedef std::map<imuCostume::Costume::SensorID, SensorData> SensorsData;

	//! Struktura opisuj�ca dane strumienia z danymi czujnik�w
	struct SensorsStreamData
	{
		//! Stempel czasu danych
		imuCostume::CostumeCANopenIO::Timestamp timestamp;
		//! Dane sensor�w
		SensorsData sensorsData;
	};

	//! Struktura opisuj�ca joint
	struct JointData
	{
		//! Orientacja lokalna
		osg::Quat localOrientation;
		//! Orientacja lokalna
		osg::Quat globalOrientation;
		//! Lokalna pozycja
		osg::Vec3d localPosition;
		//! Lokalna pozycja
		osg::Vec3d globalPosition;
	};
}

//! Makro realizujące nazwę ekstraktora pola strumienia czasowego
#define TIMEMEMBER_EXTRACTOR_NAME(memberName) \
	TimeMemberExtractor##memberName

//! Makro definiujące ekstraktor pola strumienia czasowego
#define TIMEMEMBER_EXTRACTOR(memberName) \
class TIMEMEMBER_EXTRACTOR_NAME(memberName){\
public:\
	template<typename SrcType>\
	inline static bool verify(const SrcType &) { return true; }\
	template<typename SrcType, typename DestType = typename SrcType::second_type>\
	inline static void extract(const SrcType & src, DestType & dest){\
		dest.first = src.first;\
		dest.second = src.second.memberName;\
	}\
};

//! Makro realizujące adapter pola strumienia czasowego
#define TIMEMEMBER_ADAPTER_NAME(baseType, memberName) \
	TimeMemberAdapter##baseType##memberName

//! Makro definiujące adapter pola strumienia czasowego
#define TIMEMEMBER_ADAPTER_EXT(baseType, destType, memberName) \
typedef StreamAdapterT<baseType, destType, TIMEMEMBER_EXTRACTOR_NAME(memberName)> TIMEMEMBER_ADAPTER_NAME(baseType,memberName);

//! Makro definiujące adapter pola strumienia czasowego
#define TIMEMEMBER_ADAPTER(baseType, memberName) \
	TIMEMEMBER_ADAPTER_EXT(baseType, decltype(std::declval<baseType>().second.memberName), memberName);

#endif	// __HEADER_GUARD_IMU__TYPES_H__
