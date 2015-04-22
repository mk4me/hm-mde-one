#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <hanimlib/Humanoid.h>
#include <kinematiclib/Joint.h>
#include <acclaimformatslib/Skeleton.h>
#include <biovisionformatslib/Skeleton.h>
#include <kinematiclib/SkeletalMappingScheme.h>
#include <kinematicUtils/Types.h>

namespace kinematic
{
	/// \brief  Klasa przechowuje zunifokowaną hierarchie stawów,
	//! w tej konwencji każda kość musi kończyć się stawem bez dzieci,
	//! tzw. end effector
	struct Skeleton
	{
		//! Mapa nazw jointów do ich kolejności rotacji
		typedef std::map<std::string, kinematicUtils::AxisOrder::Type> JointsAxis;

		//! Konstruktor domyślny
		Skeleton();
		//! Konstruktor kopiujący
		//! \param other Kopiowany szkielet - klonujemy węzły
		Skeleton(const Skeleton & other);
		//! Konstruktor przenoszący
		//! \param other przenoszony szkielet
		Skeleton(Skeleton && other);
		//! Destruktor wirtualny
		virtual ~Skeleton();

		//! Staw rodzic reprezentujący cały model
		JointPtr root;
		//! Nazwa modelu
		std::string name;

		//! Konwersja z popularnych formatów
		//! \param srcSkeleton Szkielet acclaim
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton);

		//! Konwersja z popularnych formatów
		//! \param srcSkeleton Szkielet acclaim
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const Skeleton & srcSkeleton, acclaim::Skeleton & destSkeleton,
			const kinematicUtils::AxisOrder::Type axisOrder,
			const kinematicUtils::AngleUnitType angleUnit,
			const JointsAxis & jointsAxis = JointsAxis());

		//! \param srcSkeleton Szkielet biovision
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton);
		//! \param srcSkeleton Szkielet hAnim
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton);

		//! \param topology Topologia dla której tworzymy szkielet
		//! \return Szkielet na bazie topologii
		static Skeleton create(TopologyNodeConstPtr topology);

		//! \param skeleton Szkielet z którego wyciągamy jointy
		//! \return Mapowanie nazw jointów do jointów
		static std::map<std::string, kinematic::JointPtr> getJoints(const Skeleton& s);
	};	
	DEFINE_SMART_POINTERS(Skeleton);
}
#endif
