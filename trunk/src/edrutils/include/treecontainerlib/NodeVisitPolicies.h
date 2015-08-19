﻿/********************************************************************
	created:  2015/07/22	13:15:05
	filename: NodeVisitPolicies.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__NODEVISITPOLICIES_H__
#define __HEADER_GUARD_TREECONTAINER__NODEVISITPOLICIES_H__

#include <treecontainerlib/Node.h>
#include <treecontainerlib/Linearization.h>

namespace treeContainer
{
	namespace VisitPolicies
	{
		namespace Node
		{

			//! Polityka odwiedzania węzłów
			struct PreOrder
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam Visitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename Visitor>
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param visitor Obiekt przegl�daj�cy wez�y
				static void visit(NPtr root, Visitor & visitor)
				{
					visitor(root);
					for (const auto & child : root->children())
					{
						visit(NPtr(child), visitor);
					}
				}
			};

			//! Polityka odwiedzania węzłów
			struct PostOrder
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam Visitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename Visitor>
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param visitor Obiekt przegl�daj�cy wez�y
				static void visit(NPtr root, Visitor & visitor)
				{
					for (const auto & child : root->children())
					{
						visit(NPtr(child), visitor);
					}
					visitor(root);
				}
			};

			//! Polityka odwiedzania węzłów
			struct LevelOrder
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam Visitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename Visitor>
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
				static void visit(NPtr root, Visitor & visitor)
				{
					treeContainer::Node::Nodes<NPtr> nodes;
					nodes.push_back(root);
					treeContainer::Node::SizeType level = 0;
					while (nodes.empty() == false)
					{
						//pobieramy dla nast�pnego poziomu dzieci
						treeContainer::Node::Nodes<NPtr> nextLevelNodes;
						for (const auto & node : nodes)
						{
							visitor(node, level);

							for (const auto & child : node->children()){
								nextLevelNodes.push_back(NPtr(child));
							}
						}

						nodes.swap(nextLevelNodes);
						++level;
					}
				}
			};

			//! \tparam VisitOrder Spos�b oryginalnego odwiedzania, kt�re chcemy odwr�ci�
			template<typename VisitOrder>
			//! Polityka odwiedzania węzłów
			struct ReverseOrder
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam Visitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename Visitor>
				//! \param visitOrder Spos�b przegl�dania w kierunku kt�ry b�dziemy odwraca�
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
				static void visit(NPtr root, Visitor & visitor)
				{
					auto lt = NodeLinearization<VisitOrder, Backward>::linearize(root);
					for (const auto & node : lt)
					{
						visitor(node);
					}
				}
			};

			//! Polityka odwiedzania węzłów z warunkiem
			struct LevelOrderWhile
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename CondVisitor>
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
				static bool visitWhile(NPtr root, CondVisitor & condVisitor)
				{
					treeContainer::Node::Nodes<NPtr> nodes;
					nodes.push_back(root);
					treeContainer::Node::SizeType level = 0;
					while (nodes.empty() == false)
					{
						//pobieramy dla nast�pnego poziomu dzieci
						treeContainer::Node::Nodes<NPtr> nextLevelNodes;
						for (const auto & node : nodes)
						{
							if (condVisitor(node, level) == false){
								return false;
							}

							for (const auto & child : node->children()) {
								nextLevelNodes.push_back(NPtr(child));
							}
						}

						nodes.swap(nextLevelNodes);
						++level;
					}

					return true;
				}
			};

			//! Polityka odwiedzania węzłów z warunkiem
			struct PreOrderWhile
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename CondVisitor>
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
				//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
				static bool visitWhile(NPtr root, CondVisitor & condVisitor)
				{
					if (condVisitor(root) == true){
						for (const auto & child : root->children())
						{
							if (visitWhile(NPtr(child), condVisitor) == false){
								return false;
							}
						}

						return true;
					}
					else {
						return false;
					}
				}
			};

			//! Polityka odwiedzania węzłów z warunkiem
			struct PostOrderWhile
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename CondVisitor>
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
				//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
				static bool visitWhile(NPtr root, CondVisitor & condVisitor)
				{
					for (const auto & child : root->children())
					{
						if (visitWhile(NPtr(child), condVisitor) == false){
							return false;
						}
					}
					return condVisitor(root);
				}
			};

			//! \tparam VisitOrder Spos�b oryginalnego odwiedzania, kt�re chcemy odwr�ci�
			template<typename VisitOrder>
			//! Polityka odwiedzania węzłów z warunkiem
			struct ReverseOrderWhile
			{
				//! \tparam NPtr Typ wska�nika w�z�a
				//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
				template<typename NPtr, typename CondVisitor>
				//! \param visitOrder Spos�b przegl�dania w kierunku kt�ry b�dziemy odwraca�
				//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
				//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
				static bool visitWhile(NPtr root, CondVisitor & condVisitor)
				{
					auto lt = NodeLinearization<VisitOrder, Backward>::linearize(root);
					for (const auto & node : lt)
					{
						if (condVisitor(node) == false){
							return false;
						}
					}

					return !lt.empty();
				}
			};
		}
	}
}

#endif	// __HEADER_GUARD_TREECONTAINER__NODEVISITPOLICIES_H__
