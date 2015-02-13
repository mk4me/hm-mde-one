#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <hanimlib/Humanoid.h>
#include <kinematiclib/Joint.h>
#include <acclaimformatslib/Skeleton.h>
#include <biovisionformatslib/Skeleton.h>
#include <kinematiclib/SkeletalMappingScheme.h>

namespace kinematic
{
	/// \brief  Klasa przechowuje zunifokowaną hierarchie stawów,
	//! w tej konwencji każda kość musi kończyć się stawem bez dzieci,
	//! tzw. end effector
	struct Skeleton
	{
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
		static bool convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton);
		static bool convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton);
		static bool convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton);

		static std::map<std::string, kinematic::JointPtr> getJoints(const Skeleton& s);
	};	
	DEFINE_SMART_POINTERS(Skeleton);
}
#endif
