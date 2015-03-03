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
#include <kinematiclib/Topology.h>
#include <utils/SmartPtr.h>

namespace kinematic
{
	//! Struktura opisuje staw modelu
	struct JointData : public TopologyData
	{		
		//! Lokalna pozycja
		osg::Vec3 position;
		//! Lokalna orientacja
		osg::Quat orientation;
	};	

	typedef utils::TreeNodeT<JointData> Joint;
	DEFINE_SMART_POINTERS(Joint);
}

#endif	// __HEADER_GUARD_KINEMATIC__JOINT_H__
