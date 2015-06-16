/********************************************************************
	created:  2015/05/05	23:35:18
	filename: Tree.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__TREE_H__
#define __HEADER_GUARD_UTILS__TREE_H__

#include <utils/TreeNode.h>

namespace utils
{
	struct Tree
	{
		//! Typ opisujacy ilość, rozmiar
		using SizeType = TreeNode::SizeType;

		//! Struktura opisuje minimalny i maksymalny stopień węzłów drzewa
		struct DegreeLimits
		{
			//! Domyślny konstruktor
			DegreeLimits() : min(std::numeric_limits<SizeType>::max()), 
			max(std::numeric_limits<SizeType>::min()) {}

			//! \param other Kopiowany obiekt
			DegreeLimits(const DegreeLimits & other) : min(other.min), max(other.max) {}
			//! Destruktor
			~DegreeLimits() {}

			//! Minimalny stopień drzewa
			SizeType min;
			//! Maksymalny stopień drzewa
			SizeType max;
			//! \return Czy zaakres stopni jest poprawny
			inline bool valid() const { return min <= max; }
		};

		//! Klasa realizująca zbieranie informacji o max i min stopniu drzewa
		class DegreeLimitsVisitor
		{
		public:
			//! Konstruktor domyslny
			DegreeLimitsVisitor() {}
			//! Destruktor
			~DegreeLimitsVisitor() {}

			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
			template<typename NPtr, typename... Args>
			//! \param node odwiedzany w�ze�
			void operator()(NPtr node, Args... args)
			{
				const auto degree = node->degree();
				degreeLimits_.min = std::min(degreeLimits_.min, degree);
				degreeLimits_.max = std::max(degreeLimits_.max, degree);
			}

			//! Resetuje limity stopni drzewa
			void reset()
			{
				degreeLimits_ = DegreeLimits();
			}

			//! \return Limity stopni drzewa
			const DegreeLimits & degreeLimits() const { return degreeLimits_; }

		private:
			//! Limity stopni drzewa
			DegreeLimits degreeLimits_;
		};

		//Typ ściezki
		//! \tparam T Element ścieżki
		template<typename T>
		using Path = TreeNode::Nodes<T>;

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param startNode Weze� z kt�rego ma wystartowa� �cie�ka
		//! \param endNode Weze� do kt�rego doj�� �cie�ka
		//! \return D�ugo�� �cie�ki pomi�dzy w�z�ami
		static SizeType distance(NPtr startNode, NPtr endNode)
		{			
			return findPath(startNode, endNode).size() - 1;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param path [out] �cie�ka kt�r� odwracamy
		static void reversePath(Path<NPtr> & path)
		{
			std::reverse(path.begin(), path.end());
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param node W�ze� z kt�rego chcemy pobra� �cie�k� do roota
		static Path<NPtr> upPath(NPtr startNode)
		{
			Path<NPtr> ret;

			ret.push_back(startNode);
			if (startNode->isRoot() == false){
				startNode = startNode->parent();
				while (startNode->isRoot() == false) { ret.push_back(startNode); startNode = startNode->parent(); }
				ret.push_back(startNode);
			}		

			return ret;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param startNode W�ze� z kt�rego ma si� rozpocz�� �cie�ka
		//! \param endNode W�ze� w kt�rym ma si� ko�czy� �cie�ka
		//! \return �cie�ka pomi�dzy w�z�ami - kolejne w�z�y
		static Path<NPtr> findPath(NPtr startNode, NPtr endNode)
		{
			Path<NPtr> ret;

			if (startNode != endNode){
				auto p1 = upPath(startNode);

				//wariant end node jest przodkiem startNode
				{
					auto it = std::find(p1.begin(), p1.end(), endNode);

					if (it != p1.end()){
						ret.insert(ret.end(), p1.begin(), it + 1);
						return ret;
					}
				}

				auto p2 = upPath(endNode);

				//wariant end node jest potomkiem startNode
				{
					auto it = std::find(p2.begin(), p2.end(), startNode);

					if (it != p2.end()){
						ret.insert(ret.end(), p2.begin(), it + 1);						
						reversePath(ret);
						return ret;
					}
				}

				if (p1.empty() == false && p2.empty() == false){

					std::set<NPtr> s1(p1.begin(), p1.end());
					std::set<NPtr> s2(p2.begin(), p2.end());

					std::vector<NPtr> intersectResult(std::min(p1.size(), p2.size()));
					auto retIT = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), intersectResult.begin());

					if (retIT != intersectResult.end()){
						//mamy punkt przeciecia - drogi si� schodz�
						//szukam najblizszego punktu przeciecia
						std::set<NPtr> intersectionSet(intersectResult.begin(), retIT);
						NPtr intersectionPoint;

						if (p1.size() < p2.size()){
							for (const auto & n : p1)
							{
								if (intersectionSet.find(n) != intersectionSet.end()){
									intersectionPoint = n;
									break;
								}
							}
						}
						else{
							for (const auto & n : p2)
							{
								if (intersectionSet.find(n) != intersectionSet.end()){
									intersectionPoint = n;
									break;
								}
							}
						}

						ret.insert(ret.end(), p1.begin(), std::find(p1.begin(), p1.end(), intersectionPoint));
						ret.insert(ret.end(), std::find(p2.begin(), p2.end(), intersectionPoint), p2.end());						
					}
				}
			}
			else{
				ret.push_back(startNode);			
			}

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Czy drzewo jest puste
		static bool empty(const TreeType & tree)
		{
			return tree.root() == nullptr;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Ilość węzłów drzewa
		static SizeType size(const TreeType & tree)
		{
			if (empty(tree) == true){
				return 0;
			}

			return tree.root()->size();
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Limity stopni węzłów
		static DegreeLimits degreeLimits(const TreeType & tree)
		{
			DegreeLimitsVisitor v;
			
			PreOrderVisitPolicy::visit(tree, v);			

			return v.degreeLimits();
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Wysokość drzewa
		static SizeType height(const TreeType & tree)
		{
			if (empty(tree) == true){
				return -1;
			}

			return tree.root()->height();
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Poziom drzewa
		static SizeType level(const TreeType & tree)
		{
			return height(tree) + 1;
		}

		//! \tparam TreeTypeA Typ drzewa A
		//! \tparam TreeTypeB Typ drzewa B
		template<typename TreeTypeA, typename TreeTypeB>
		//! \param treeA Drzewo A
		//! \param treeB Drzewo B
		//! \return Czy struktury drzew są identyczne
		static bool compareStructure(const TreeTypeA & treeA, const TreeTypeB & treeB)
		{
			if ((void*)&treeA == (void*)&treeB){
				return true;
			}

			if (treeA.root() == treeB.root()){
				return true;
			}

			if (empty(treeA) != empty(treeB)){
				return false;
			}			

			return TreeNode::compareStructure(treeA.root(), treeB.root());
		}

		//! \tparam TreeType Typ drzewa
		//! \tparam Comp Typ obiektu porównującego dane drzewa
		template<typename TreeType, typename Comp>
		//! \param treeA Drzewo A
		//! \param treeB Drzewo B
		//! \return Czy struktury i wartości drzew są identyczne
		static bool compare(const TreeType & treeA, const TreeType & treeB, Comp comp = Comp())
		{
			if (&treeA == &treeB){
				return true;
			}

			if (treeA.root() == treeB.root()){
				return true;
			}

			if (empty(treeA) != empty(treeB)){
				return false;
			}

			return TreeNode::compare(treeA.root(), treeB.root(), comp);
		}

		//! Polityka przechodzenia drzewa schematem PreOrder
		struct PreOrderVisitPolicy
		{
			using NodeVisitOrder = TreeNode::PreOrderVisitPolicy;

			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false)
				{
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}

			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(const TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false)
				{
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}
		};

		//! Polityka przechodzenia drzewa schematem PostOrder
		struct PostOrderVisitPolicy
		{
			using NodeVisitOrder = TreeNode::PostOrderVisitPolicy;

			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false)
				{
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}

			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(const TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false)
				{
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}
		};

		//! Polityka przechodzenia drzewa schematem LevelOrder
		struct LevelOrderVisitPolicy
		{
			using NodeVisitOrder = TreeNode::LevelOrderVisitPolicy;

			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false)
				{
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}

			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(const TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false)
				{
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}
		};

		//! \tparam NodeVisitOrderT Polityka przechodzenia drzewa którą odwracamy
		template<typename NodeVisitOrderT>
		//! Polityka przechodzenia drzewa schematem odwrotnym do danego
		struct ReverseOrderVisitPolicy
		{			
			using NodeVisitOrder = TreeNode::ReverseOrderVisitPolicy<NodeVisitOrderT>;
			
			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
			static void visit(TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false){
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}
			
			//! \tparam TreeType Typ drzewa
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename Visitor>
			//! \param tree Drzewo
			//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
			static void visit(const TreeType & tree, Visitor & visitor)
			{
				if (empty(tree) == false){
					NodeVisitOrder::visit(tree.root(), visitor);
				}
			}
		};

		//! Polityka przechodzenia drzewa schematem LevelOrder z warunkiem
		struct LevelOrderWhileVisitPolicy
		{
			using NodeVisitOrder = LevelOrderWhileVisitPolicy;

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}
		};

		//! Polityka przechodzenia drzewa schematem PreOrder z warunkiem
		struct PreOrderWhileVisitPolicy
		{
			using NodeVisitOrder = PreOrderWhileVisitPolicy;

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
			static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
			static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}
		};

		//! Polityka przechodzenia drzewa schematem PostOrder z warunkiem
		struct PostOrderWhileVisitPolicy
		{
			using NodeVisitOrder = PostOrderWhileVisitPolicy;

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
			static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
			static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}
		};

		//! \tparam NodeVisitOrderT Polityka przechodzenia drzewa którą odwracamy
		template<typename NodeVisitOrderT>
		//! Polityka przechodzenia drzewa schematem odwrotnym do danego z warunkiem
		struct ReverseOrderWhileVisitPolicy
		{			
			using NodeVisitOrder = TreeNode::ReverseOrderWhileVisitPolicy<NodeVisitOrderT>;

			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>			
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}
		
			//! \tparam TreeType Typ drzewa
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename TreeType, typename CondVisitor>			
			//! \param tree Drzewo
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
			{
				if (empty(tree) == false){
					return NodeVisitOrder::visitWhile(tree.root(), condVisitor);
				}

				return false;
			}
		};
	};

	//! \tparam T Typ danych przechowywanych w drzewie
	template<typename T>
	//! Klasa obsługująca generyczne drzewo - główna funckja to trzymanie roota drzewa
	class TreeT
	{
	public:

		using Node = TreeNodeT < T > ;		

	public:
		//! Domyślny konstruktor
		TreeT() {}

		//! Konstruktor kopiujący
		//! \param Other Kopiowane drzewo
		TreeT(const TreeT & Other) : root_(Other.root_ == nullptr ? nullptr : Node::clone(Other.root_)) {}
		//! Konstruktor przenoszący
		//! \param Other Przenoszone drzewo
		TreeT(TreeT && Other) : root_(std::move(Other.root_)) {}

		//! \apram root Klonowany korzeń drzewa
		TreeT(typename Node::NodePtr root) : root_(root == nullptr ? nullptr : Node::clone(root)) {}
		//! \apram root Przenoszony korzeń drzewa
		TreeT(typename Node::NodePtr && root) : root_(std::move(root)) {}
		//! Destruktor
		~TreeT() {}

		//! \return Root drzewa
		typename Node::NodePtr root() { return root_; }
		//! \return Root drzewa
		typename Node::NodeConstPtr root() const { return root_; }
		//! \param Other Zamieniane drzewo
		void swap(TreeT & other) { root_.swap(other.root_); }

	private:
		//! Root drzewa
		typename Node::NodePtr root_;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREE_H__