#ifndef HEADER_GUARD_acclaimMODEL__SKELETON_H__
#define HEADER_GUARD_acclaimMODEL__SKELETON_H__

#include <map>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <utils/SmartPtr.h>
#include <acclaimformatslib/Bone.h>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <osg/Vec3>

namespace acclaim
{
	/// \brief  Klasa przechowuje hierarchie kości
	struct Skeleton
	{	

		typedef std::map<int, Bone> Bones;
		typedef boost::bimap<boost::bimaps::multiset_of<int>, boost::bimaps::set_of<int>> Hierarchy;

		struct Units
		{
			std::map<std::string, std::string> typeValueUnits;
			std::map<std::string, float> defaultValues;

			bool isAngleInRadians() const {
				auto angleIt = typeValueUnits.find("angle");
				return angleIt != typeValueUnits.end() && angleIt->second == "rad";
			}
		};

		std::string version;
		std::string name;
		std::string documentation;
		Units units;
		//! korzeń, czyli kość od której zaczyna się hierarchia.
		int root;

		Bones bones;

		Hierarchy hierarchy;
		//! początkowa pozycja w globalnym układzie odniesienia
		osg::Vec3 position;
		//! początkowa orientacja w globalnym układzie odniesienia
		osg::Vec3 orientation;
		//! kolejność rotacji (sekcja axis)
		kinematicUtils::AxisOrder::Type axisOrder;
		//! kolejność rotacji (sekcja order)
		std::vector<kinematicUtils::Channel> dataOrder;
		kinematicUtils::AxisOrder::Type getRotationOrder() const
		{
			std::string s;
			int count = dataOrder.size();
			for (int i = 0; i < count; ++i) {
				if (dataOrder[i] == kinematicUtils::ChannelType::RX) {
					s += "X";
				} else if (dataOrder[i] == kinematicUtils::ChannelType::RY) {
					s += "Y";
				} else if (dataOrder[i] == kinematicUtils::ChannelType::RZ) {
					s += "Z";
				}
			}
			return Axis::getAxisOrder(s);
		}

	};
	DEFINE_SMART_POINTERS(Skeleton);
}
#endif
