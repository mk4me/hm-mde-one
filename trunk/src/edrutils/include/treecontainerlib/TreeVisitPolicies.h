/********************************************************************
	created:  2015/07/22	13:41:20
	filename: TreeVisitPolicies.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREEVISITPOLICIES_H__
#define __HEADER_GUARD_TREECONTAINER__TREEVISITPOLICIES_H__

#include <treecontainerlib/Tree.h>

namespace treeContainer
{
	namespace VisitPolicies
	{
		namespace Tree
		{
			//! Polityka przechodzenia drzewa schematem PreOrder
			struct PreOrder
			{
				using NodeVisitOrder = VisitPolicies::Node::PreOrder;

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły
				static void visit(TreeType & tree, Visitor & visitor)
				{
					if (empty(tree) == false)
					{
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły
				static void visit(const TreeType & tree, Visitor & visitor)
				{
					if (empty(tree) == false)
					{
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}
			};

			//! Polityka przechodzenia drzewa schematem PostOrder
			struct PostOrder
			{
				using NodeVisitOrder = VisitPolicies::Node::PostOrder;

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły
				static void visit(TreeType & tree, Visitor & visitor)
				{
					if (empty(tree) == false)
					{
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły
				static void visit(const TreeType & tree, Visitor & visitor)
				{
					if (empty(tree) == false)
					{
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}
			};

			//! Polityka przechodzenia drzewa schematem LevelOrder
			struct LevelOrder
			{
				using NodeVisitOrder = VisitPolicies::Node::LevelOrder;

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły
				static void visit(TreeType & tree, Visitor & visitor)
				{
					if (treeContainer::Tree::empty(tree) == false)
					{
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły
				static void visit(const TreeType & tree, Visitor & visitor)
				{
					if (treeContainer::Tree::empty(tree) == false)
					{
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}
			};

			//! \tparam NodeVisitOrderT Polityka przechodzenia drzewa którą odwracamy
			template<typename NodeVisitOrderT>
			//! Polityka przechodzenia drzewa schematem odwrotnym do danego
			struct ReverseOrder
			{
				using NodeVisitOrder = VisitPolicies::Node::ReverseOrder < NodeVisitOrderT >;

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły i poziomy
				static void visit(TreeType & tree, Visitor & visitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename TreeType, typename Visitor>
				//! \param tree Drzewo
				//! \param visitor Obiekt przeglądający węzły i poziomy
				static void visit(const TreeType & tree, Visitor & visitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						NodeVisitOrder::visit(tree.root(), visitor);
					}
				}
			};

			//! Polityka przechodzenia drzewa schematem LevelOrder z warunkiem
			struct LevelOrderWhile
			{
				using NodeVisitOrder = VisitPolicies::Node::LevelOrderWhile;

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}
			};

			//! Polityka przechodzenia drzewa schematem PreOrder z warunkiem
			struct PreOrderWhile
			{
				using NodeVisitOrder = VisitPolicies::Node::PreOrderWhile;

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
				static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
				static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}
			};

			//! Polityka przechodzenia drzewa schematem PostOrder z warunkiem
			struct PostOrderWhile
			{
				using NodeVisitOrder = VisitPolicies::Node::PostOrderWhile;

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
				static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
				static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}
			};

			//! \tparam NodeVisitOrderT Polityka przechodzenia drzewa którą odwracamy
			template<typename NodeVisitOrderT>
			//! Polityka przechodzenia drzewa schematem odwrotnym do danego z warunkiem
			struct ReverseOrderWhile
			{
				using NodeVisitOrder = VisitPolicies::Node::ReverseOrderWhile < NodeVisitOrderT >;

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}

				//! \tparam TreeType Typ drzewa
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename TreeType, typename CondVisitor>
				//! \param tree Drzewo
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
				{
					if (treeContainer::Tree::empty(tree) == false){
						return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
					}

					return false;
				}
			};
		}
	}
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREEVISITPOLICIES_H__