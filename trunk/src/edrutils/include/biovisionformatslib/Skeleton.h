/********************************************************************
	created:  2014/12/10	22:49:08
	filename: Skeleton.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_BIOVISIONFORMATS__SKELETON_H__
#define __HEADER_GUARD_BIOVISIONFORMATS__SKELETON_H__

#include <biovisionformatslib/Joint.h>
#include <map>
#include <vector>

namespace biovision
{
	struct Skeleton
	{
		JointPtr root;
		std::map<std::string, JointPtr> joints;
		std::vector<JointPtr> sourceOrderJoints;
	};
}

#endif	// __HEADER_GUARD_BIOVISIONFORMATS__SKELETON_H__