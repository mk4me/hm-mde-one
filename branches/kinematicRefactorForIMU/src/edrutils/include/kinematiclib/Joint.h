/********************************************************************
	created:  2014/12/15	11:46:34
	filename: Joint.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__JOINT_H__
#define __HEADER_GUARD_KINEMATIC__JOINT_H__

#include <osg/Quat>
#include <osg/Vec3>
#include <string>
#include <list>
#include <utils/SmartPtr.h>
#include <utils/TreeNode.h>

namespace kinematic
{
	//! Struktura opisuje staw modelu
	struct JointData
	{		
		//! Lokalna pozycja
		osg::Vec3 position;
		//! Lokalna orientacja
		osg::Quat orientation;
		//! Unikalna nazwa
		std::string name;
	};	

	typedef utils::TreeNodeT<JointData> Joint;
	DEFINE_SMART_POINTERS(Joint);
}

#endif	// __HEADER_GUARD_KINEMATIC__JOINT_H__
