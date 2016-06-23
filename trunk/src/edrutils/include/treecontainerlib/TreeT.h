/********************************************************************
	created:  2015/07/22	13:09:15
	filename: TreeT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREET_H__
#define __HEADER_GUARD_TREECONTAINER__TREET_H__

#include <treecontainerlib/Tree.h>

namespace treeContainer
{
	//! \tparam T Typ danych przechowywanych w drzewie
	template<typename T>
	//! Klasa obs�uguj�ca generyczne drzewo - g��wna funckja to trzymanie roota drzewa
	class TreeT
	{
	public:

		using Node = NodeT < T > ;

	public:
		//! Domy�lny konstruktor
		TreeT() {}

		//! Konstruktor kopiuj�cy
		//! \param Other Kopiowane drzewo
		TreeT(const TreeT & Other) : root_(Other.root_ == nullptr ? nullptr : Node::clone(Other.root_)) {}
		//! Konstruktor przenosz�cy
		//! \param Other Przenoszone drzewo
		TreeT(TreeT && Other) : root_(std::move(Other.root_)) {}

		//! \apram root Klonowany korze� drzewa
		TreeT(typename Node::Ptr root) : root_(root == nullptr ? nullptr : Node::clone(root)) {}
		//! \apram root Przenoszony korze� drzewa
		TreeT(typename Node::Ptr && root) : root_(std::move(root)) {}
		//! Destruktor
		~TreeT() {}

		//! \return Root drzewa
		typename Node::Ptr root() { return root_; }
		//! \return Root drzewa
		typename Node::ConstPtr root() const { return root_; }
		//! \param Other Zamieniane drzewo
		void swap(TreeT & other) { root_.swap(other.root_); }

	private:
		//! Root drzewa
		typename Node::Ptr root_;
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREET_H__