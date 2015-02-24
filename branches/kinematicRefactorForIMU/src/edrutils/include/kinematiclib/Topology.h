/********************************************************************
	created:  2015/02/24	18:59:39
	filename: Topology.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__
#define __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__

#include <string>
#include <utils/TreeNode.h>

namespace kinematic
{
	//! Typ wêz³a topologii szkieletu
	typedef utils::TreeNodeT<std::string> TopologyNode;
}

#endif	// __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__