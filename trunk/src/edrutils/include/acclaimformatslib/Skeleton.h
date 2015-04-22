#ifndef HEADER_GUARD_acclaimMODEL__SKELETON_H__
#define HEADER_GUARD_acclaimMODEL__SKELETON_H__

#include <map>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <utils/SmartPtr.h>
#include <acclaimformatslib/Bone.h>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <kinematicUtils/RotationConverter.h>
#include <osg/Vec3>

namespace acclaim
{
	/// \brief  Klasa przechowuje hierarchie kości
	struct Skeleton
	{	
		//! Typ identyfikatora kości
		typedef int ID;
		//! Typ indeksujący kości wg ich identyfikatorów
		typedef std::map<ID, Bone> Bones;
		//! Typ opisujący hierarchię kości - relacja rodzic <-> dziecko
		typedef boost::bimap<boost::bimaps::multiset_of<ID>, boost::bimaps::set_of<ID>> Hierarchy;

		//! Struktura opisująca sekcję Units
		struct Units
		{
			//! Mapa klucz - opis
			std::map<std::string, std::string> typeValueUnits;
			//! Mapa klucz - wartość
			std::map<std::string, double> defaultValues;
			//! Konstruktor domyślny
			Units();
			//! Destruktor
			~Units();

			//! \return Czy kąt podawany jest w radianach
			bool isAngleInRadians() const;
			//! \param angleType Typ kąta
			void setAngleType(const kinematicUtils::AngleUnitType angleType);
		};

		//! Konstruktor domyślny
		Skeleton();
		//! Destruktor
		~Skeleton();

		//! Wersja formatu
		std::string version;
		//! Nazwa szkieletu
		std::string name;
		//! Dokumentacja
		std::string documentation;
		//! Jednostki i opis
		Units units;
		//! korzeń, czyli kość od której zaczyna się hierarchia.
		ID root;
		//! Kości
		Bones bones;
		//! Hierarchia
		Hierarchy hierarchy;
		//! Początkowa pozycja w globalnym układzie odniesienia
		osg::Vec3d position;
		//! Początkowa orientacja w globalnym układzie odniesienia
		osg::Vec3d orientation;
		//! Kolejność rotacji (sekcja axis)
		kinematicUtils::AxisOrder::Type axisOrder;
		//! Kolejność rotacji (sekcja order)
		std::vector<kinematicUtils::Channel> dataOrder;
		//! \return Kolejność rotacji
		kinematicUtils::AxisOrder::Type rotationOrder() const;
		//! \return Aktywne kości (razem z rootem)
		unsigned int activeBones() const;

	};
	DEFINE_SMART_POINTERS(Skeleton);
}
#endif
