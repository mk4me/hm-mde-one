#ifndef HEADER_GUARD_acclaimMODEL__JOINT_H__
#define HEADER_GUARD_acclaimMODEL__JOINT_H__

#include <vector>
#include <osg/Vec3>
#include <utils/SmartPtr.h>
#include <boost/utility.hpp>
#include <acclaimformatslib/DegreeOfFreedom.h>

namespace acclaim
{
	//! Struktura przechowuje informacje o stawie
	struct Bone
	{
		//! unikalny identyfikator stawu
		int id;
		//! nazwa stawu
		std::string name;
		//! kierunek kości w globalnym układzie odniesienia
		osg::Vec3 direction;
		//! długość kości
		float length;
		//! określa początkową orientację dla kości
		osg::Vec3 axis;
		//! określa kolejność rotacji
		kinematicUtils::AxisOrder::Type axisOrder;
		//! stopnie swobody dla kości (razem z limitami)
		std::vector<DegreeOfFreedom> dofs;
		////! określa kolejność rotacji
		//kinematicUtils::AxisOrder::Type rotationOrder;

		Bone() : id(-1),
			axis(0.0, 0.0, 0.0),
			length(-1),
			axisOrder(kinematicUtils::AxisOrder::XYZ),
			//rotationOrder(kinematicUtils::AxisOrder::XYZ),
			direction(0.0, 0.0, 0.0),
			name(name)
		{}

		Bone(const Bone & Other)
			: id(Other.id),
			name(Other.name),
			direction(Other.direction),
			length(Other.length),
			axis(Other.axis),
			axisOrder(Other.axisOrder),
			//rotationOrder(Other.rotationOrder),
			dofs(Other.dofs)
		{}

		~Bone() {}
	};
}

#endif
