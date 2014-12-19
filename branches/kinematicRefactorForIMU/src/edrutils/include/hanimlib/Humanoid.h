/********************************************************************
	created:  2014/12/16	19:57:52
	filename: Humanoid.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIM__HUMANOID_H__
#define __HEADER_GUARD_HANIM__HUMANOID_H__

#include <hanimlib/Object.h>
#include <hanimlib/Joint.h>
#include <hanimlib/Segment.h>
#include <hanimlib/Site.h>
#include <hanimlib/Displacer.h>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

namespace hAnim
{
	/// \brief  Klasa przechowuje hierarchie kości
	struct Humanoid
	{
		//rodzic -> dziecko
		typedef boost::bimap<boost::bimaps::multiset_of<std::string>, boost::bimaps::set_of<std::string>> Hierarchy;

		Humanoid();

		Humanoid(Humanoid && Other);

		virtual ~Humanoid() {}

		static Humanoid defaultHumanHumanoid();
		static const Hierarchy & defaultHumanHierarchy();

		osg::Vec3 bboxCenter;
		osg::Vec3 bboxSize;
		osg::Vec3 center;
		std::list<std::string> info;
		sequence<Joint> joints;
		osg::Quat rotation;
		osg::Vec3 scale;
		osg::Quat scaleOrientation;
		sequence<Segment> segments;
		sequence<Site> sites;
		sequence<Object> skeleton;
		sequence<Object> skin;
		std::vector<osg::Vec3> skinCoord;
		std::vector<osg::Vec3> skinNormal;
		osg::Vec3 translation;
		std::string version;
		sequence<Site> viewpoints;
	};
}

#endif	// __HEADER_GUARD_HANIM__HUMANOID_H__
