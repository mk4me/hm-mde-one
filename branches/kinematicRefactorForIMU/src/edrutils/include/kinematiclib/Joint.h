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

namespace kinematic
{
	struct Joint;
	DEFINE_SMART_POINTERS(Joint);

	//! Struktura opisuje staw modelu
	struct Joint
	{
		//! Rodzic
		JointWeakPtr parent;
		//! Lokalna pozycja
		osg::Vec3 position;
		//! Lokalna orientacja
		osg::Quat orientation;
		//! Unikalna nazwa
		std::string name;
		//! Dzieci
		std::list<JointPtr> children;

		template<typename T>
		static void visit(JointPtr joint, T & visitor)
		{
			visitor(joint);
			for (auto c : joint->children)
			{
				visit(c, visitor);
			}
		}

		template<typename T>
		static void visit(JointConstPtr joint, T & visitor)
		{
			visitor(joint);
			for (auto c : joint->children)
			{
				visit(c, visitor);
			}
		}
	};	
}

#endif	// __HEADER_GUARD_KINEMATIC__JOINT_H__