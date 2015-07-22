/********************************************************************
	created:  2015/02/24	18:59:39
	filename: Topology.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__
#define __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__

#include <string>
#include <treecontainerlib/NodeT.h>
#include <utils/SmartPtr.h>

namespace kinematic
{
	//! Struktur opisuj�ca w�z�y topologii 
	struct TopologyData
	{
		//! Nazwa w�z�a
		std::string name;
	};

	//! Typ w�z�a topologii szkieletu
	typedef treeContainer::NodeT<TopologyData> TopologyNode;

	DEFINE_SMART_POINTERS(TopologyNode);
}

#endif	// __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__