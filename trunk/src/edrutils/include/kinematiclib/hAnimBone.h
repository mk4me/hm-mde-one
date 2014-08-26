/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:24
	filename: 	hAnimBone.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__HANIMBONE_H__
#define HEADER_GUARD_KINEMATIC__HANIMBONE_H__

#include <utils/SmartPtr.h>
#include <boost/noncopyable.hpp>
#include <osg/Vec3>
#include <utils/Debug.h>

namespace kinematic {
	class hAnimJoint;
	typedef utils::shared_ptr<hAnimJoint> hAnimJointPtr;
	typedef utils::weak_ptr<hAnimJoint> hAnimJointWeak;

	/// \brief Klasa reprezentuje segment standardu h-anim
	/// Klasa nie jest powiązana z danymi, pomaga jedynie budować hierarchie
	class hAnimBone : boost::noncopyable
	{
	public:
		ENABLE_PRIVATE_TESTS
	private:
		//! nazwa kości (powinna być taka jak w h-anim 1.1)
		std::string name;
		//! masa
		float mass;
		//! dlugosc kosci
		float length;
		//! pozycja srodka ciezkosci
		float cofMass;
		//! do jednej kości moze być podłączone wiele stawów
		std::vector<hAnimJointPtr> childrenJoints;
		//! staw, z którego wychodzi kość
		hAnimJointWeak parentJoint;

	public:

		hAnimBone() : name("UnnamedBone"), mass(-1), length(-1), cofMass(-1) {}

		const float getLength() const { return length; }
		const float getMass() const { return mass; }
		const float getCofMass() const { return cofMass; }

		void setLength(const float l)  { length = l; }
		void setMass(const float m) { mass = m; }
		void setCofMass(const float cfm) { cofMass = cfm; }

		//! \return nazwa kości
		const std::string& getName() const { return name; }
		//! ustawia nazwę danej kości
		//! \param val nowa nazwa
		void setName(const std::string& val) { name = val; }
		//! zwraca tablice ze stawami, które kończą kość (segment)
		std::vector<hAnimJointPtr>& getChildrenJoints() { return childrenJoints; }
		//! pobiera staw h-anim, który jest rodzicem danej kości
		kinematic::hAnimJointWeak getParentJoint() const { return parentJoint; }
		//! ustawia rodzica kości
		//! \param val staw h-anim
		void setParentJoint(kinematic::hAnimJointWeak val) { parentJoint = val; }

		void setChildrenJoints(const std::vector<hAnimJointPtr> & joint) { childrenJoints = joint; }
	};
	typedef utils::shared_ptr<hAnimBone> hAnimBonePtr;
	typedef utils::weak_ptr<hAnimBone> hAnimBoneWeak;
}

#endif
