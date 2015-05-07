/********************************************************************
	created:  2015/04/27	17:59:28
	filename: LinearizedSkeleton.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__LINEARIZEDSKELETON_H__
#define __HEADER_GUARD_KINEMATIC__LINEARIZEDSKELETON_H__

#include <string>
#include <vector>
#include <boost/bimap.hpp>
#include <kinematiclib/Skeleton.h>
#include <utils/LinearizedTree.h>

namespace kinematic
{
	struct LinearizedSkeleton
	{
		//! Typ bimapy indeksów węzłów do nazw węzłów
		using Order = utils::LinearizedTree::Order< std::string > ;
		//! Typ indeksu węzła
		using NodeIDX = utils::LinearizedTree::NodeIDX;
		//! Typ bimapy indeksów węzłów do nazw węzłów
		using Mapping = utils::LinearizedTree::Mapping< std::string >;
		//! Typ obsługujący przechodzenie po zlinearyzowanym szkielecie
		using Visitor = utils::LinearizedTreeT < utils::Tree::LevelOrderVisitPolicy > ;

		//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
		//! \return Mapowanie jointów szkieletu
		static Order createCompleteOrder(const Skeleton & skeleton);
		//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
		//! \return Mapowanie jointów szkieletu
		static Mapping createCompleteMapping(const Skeleton & skeleton);

		//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (mających dzieci)
		//! \return Mapowanie aktywnych jointów szkieletu
		static Order createNonLeafOrder(const Skeleton & skeleton);
		//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (mających dzieci)
		//! \return Mapowanie aktywnych jointów szkieletu
		static Mapping createNonLeafMapping(const Skeleton & skeleton);
	};
}

#endif	// __HEADER_GUARD_KINEMATIC__LINEARIZEDSKELETON_H__