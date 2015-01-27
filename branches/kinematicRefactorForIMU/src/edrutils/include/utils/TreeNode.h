/********************************************************************
	created:  2015/01/15	13:30:08
	filename: TreeNode.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__TREENODE_H__
#define __HEADER_GUARD_UTILS__TREENODE_H__

#include <utils/SmartPtr.h>
#include <algorithm>
#include <list>
#include <set>

namespace utils
{
	//! \tparam NPtr Typ wska�nika w�z�a
	template<typename NPtr>
	class TreeLinearizeVisitor
	{
		//! Typ zlinearyzowanego drzewa
		typedef std::list<NPtr> LinearizedTree;

	public:

		//! Konstruktor domyslny
		TreeLinearizeVisitor() {}
		//! Destruktor
		~TreeLinearizeVisitor() {}

		//! \param node odwiedzany w�ze�
		void operator()(NPtr node)
		{
			linearizedTree.push_back(node);
		}

		//! Zlinearyzowane drzewo
		LinearizedTree linearizedTree;
	};

	//! \tparam NPtr Typ wska�nika w�z�a
	template<typename NPtr>
	class MaxLevelVisitor
	{
	public:
		//! Konstruktor domyslny
		MaxLevelVisitor() : level(0) {}
		//! Destruktor
		~MaxLevelVisitor() {}
		//! \param node Odwiedzany w�ze�
		//! \param level Aktualnie odwiedzany poziom (wzgledem w�z�a z kt�rego startowali�my)
		void operator()(NPtr node, const unsigned int level)
		{
			this->level = std::max(this->level, level);
		}

		//! Maksymalny poziom
		unsigned int level;
	};

	//! \tparam ValueType Typ rpzechowywanej warto�ci
	template<typename ValueType>
	struct TreeNode
	{
	public:
		//! Typ przechowywanych w w�z�ach warto�ci
		typedef ValueType value_type;
		//! Typ naszego w�z�a
		typedef TreeNode<value_type> node_type;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);	
		//! Dzieci wezla
		typedef std::list<NodePtr> Children;	
		//! Typ ilo�ci
		typedef typename Children::size_type size_type;
		//! Typ �cie�ki pomi�dzy w�z�ami
		typedef Children Path;

	private:

		//! \param value Warto�c przechowywana w w�le (kopiowanie)
		TreeNode(const value_type & value) : value(value) {}
		//! \param value Warto�c przechowywana w w�le (move semantics)
		TreeNode(value_type && value) : value(std::move(value)) {}

		//! \param node W�ze� kt�ry sprawdzamy pod k�tem bycia naszym przodkiem
		//! \return Czy dany wez� jest naszym przodkiem (id�c w g�r� po parentach trafimy do niego)
		bool rawIsAncestor(const node_type * node) const
		{
			if (node == nullptr || node == this){
				return false;
			}

			auto p = parent.lock();

			while (p.get() != node) { p = p->parent.lock(); }

			return p.get() == node;
		}		

	public:
		
		//! \param value Warto�� z jak� tworzymy w�ze�
		//! \return Nowy w�ze� z zadan� warto�ci�
		static NodePtr create(const value_type & value = value_type())
		{
			return NodePtr(new node_type(value));
		}

		//! \param value Warto�� z jak� tworzymy w�ze�
		//! \return Nowy w�ze� z zadan� warto�ci�
		static NodePtr create(value_type && value)
		{
			return NodePtr(new node_type(std::move(value)));
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param �cie�ka kt�rej d�ugo�� obliczam
		//! \return D�ugo�� �cie�ki
		static size_type distance(const std::list<NPtr> & path)
		{
			return path.size() - 1;
		}

		//! \param startNode Weze� z kt�rego ma wystartowa� �cie�ka
		//! \param endNode Weze� do kt�rego doj�� �cie�ka
		//! \return D�ugo�� �cie�ki pomi�dzy w�z�ami
		static size_type distance(NodeConstPtr startNode, NodeConstPtr endNode)
		{			
			return distance(findPath(startNode, endNode));
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param path [out] �cie�ka kt�r� odwracamy
		static void reversePath(std::list<NPtr> & path)
		{
			std::reverse(path.begin(), path.end());
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param startNode W�ze� z kt�rego ma si� rozpocz�� �cie�ka
		//! \param endNode W�ze� w kt�rym ma si� ko�czy� �cie�ka
		//! \return �cie�ka pomi�dzy w�z�ami - kolejne w�z�y
		static std::list<NPtr> findPath(NPtr startNode, NPtr endNode)
		{
			std::list<NPtr> ret;		

			if (startNode != endNode){
				auto p1 = upPath(startNode);

				auto it = std::find(p1.begin(), p1.end(), endNode);

				if (it != p1.end()){
					ret.push_back(startNode);
					ret.insert(ret.end(), p1.begin(), it);
					ret.push_back(endNode);
					return ret;
				}

				auto p2 = upPath(endNode);

				it = std::find(p2.begin(), p2.end(), endNode);

				if (it != p2.end()){
					ret.insert(ret.end(), p2.begin(), it);
					reversePath(ret);
					ret.push_front(startNode);
					ret.push_back(endNode);
					return ret;
				}

				if (p1.empty() == false && p2.empty() == false){

					std::set<NPtr> s1(p1.begin(), p1.end());
					std::set<NPtr> s2(p2.begin(), p2.end());

					std::vector<NPtr> intersectResult(std::min(p1.size(), p2.size()));
					auto retIT = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), intersectResult.begin());

					if (retIT != intersectResult.end()){
						//mamy punkt przeciecia - drogi si� schodz�
						
						ret.insert(ret.end(), p2.begin(), std::find(p2.begin(), p2.end(), intersectResult.front()));
						reversePath(ret);
						ret.push_front(intersectResult.front());
						ret.insert(ret.begin(), p1.begin(), std::find(p1.begin(), p1.end(), intersectResult.front()));
						ret.push_front(startNode);
						ret.push_back(startNode);
					}
				}
			}			

			return ret;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param node W�ze� z kt�rego chcemy pobra� �cie�k� do roota
		static std::list<NPtr> upPath(NPtr startNode)
		{
			std::list<NPtr> ret;			
			while (startNode != nullptr && startNode->isRoot() == false) { ret.push_back(startNode); startNode = startNode->parent.lock(); }
			return ret;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y
		static void visitPreOrder(NPtr node, Visitor & visitor)
		{
			if (node != nullptr){
				visitor(node);
				for (auto & child : node->children)
				{
					visitPreOrder(NPtr(child), visitor);
				}
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y
		static void visitPostOrder(NPtr node, Visitor & visitor)
		{
			if (node != nullptr){
				for (auto & child : node->children)
				{
					visitPostOrder(NPtr(child), visitor);
				}
				visitor(node);
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
		static void visitLevelOrder(NodePtr node, Visitor & visitor)
		{
			if (node != nullptr){

				Children children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					Children nextLevelChildren;
					for (auto & child : children)
					{
						visitor(child, level);

						if (child != nullptr){
							nextLevelChildren.insert(nextLevelChildren.end(), child->children.begin(), child->children.end());
						}						
					}

					children.swap(nextLevelChildren);
					++level;
				}
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
		static void visitLevelOrder(NodeConstPtr node, Visitor & visitor)
		{
			if (node != nullptr){

				std::list<NodeConstPtr> children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					std::list<NodeConstPtr> nextLevelChildren;
					for (auto & child : children)
					{
						visitor(child, level);

						if (child != nullptr){
							for (auto & v : child->children){
								nextLevelChildren.push_back(NodeConstPtr(v));
							}
						}
					}

					children.swap(nextLevelChildren);
					++level;
				}
			}
		}

		//! \tparam VisitOrder Spos�b oryginalnego odwiedzania, kt�re chcemy odwr�ci�
		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename VisitOrder, typename NPtr, typename Visitor>
		//! \param visitOrder Spos�b przegl�dania w kierunku kt�ry b�dziemy odwraca�
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
		static void visitReverseOrder(VisitOrder visitOrder, NPtr node, Visitor & visitor)
		{
			TreeLinearizeVisitor<NPtr> hv;
			visitOrder(node, hv);
			reversePath(hv.linearizedTree);
			for (auto & val : hv.linearizedTree)
			{
				visitor(val);
			}
		}

		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		static bool visitLevelOrderWhile(NodePtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){

				Children children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					Children nextLevelChildren;
					for (auto & child : children)
					{
						if (condVisitor(child, level) == false){
							return false;
						}

						if (child != nullptr){
							nextLevelChildren.insert(nextLevelChildren.end(), child->children.begin(), child->children.end());
						}
					}

					children.swap(nextLevelChildren);
					++level;
				}

				return true;
			}
			else{
				return false;
			}
		}

		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
		static bool visitLevelOrderWhile(NodeConstPtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){

				std::list<NodeConstPtr> children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					std::list<NodeConstPtr> nextLevelChildren;
					for (auto & child : children)
					{
						if (condVisitor(child) == false){
							return false;
						}

						if (child != nullptr){

							for (auto & val : child->children)
							{
								nextLevelChildren.push_back(val);
							}
						}
					}

					children.swap(nextLevelChildren);
					++level;
				}

				return true;
			}
			else{
				return false;
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
		static bool visitPreOrderWhile(NPtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){
				if (condVisitor(node) == true){
					for (auto & child : node->children)
					{
						if (visitPreOrderWhile(NPtr(child), condVisitor) == false){
							return false;
						}
					}

					return true;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
		static bool visitPostOrderWhile(NPtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){
				for (auto & child : node->children)
				{
					if (visitPostOrderWhile(NPtr(child), condVisitor) == false){
						return false;
					}
				}
				return condVisitor(node);
			}
			else{
				return false;
			}
		}

		//! \tparam VisitOrder Spos�b oryginalnego odwiedzania, kt�re chcemy odwr�ci�
		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename VisitOrder, typename NPtr, typename CondVisitor>
		//! \param visitOrder Spos�b przegl�dania w kierunku kt�ry b�dziemy odwraca�
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		static void visitReverseOrderWhile(VisitOrder visitOrder, NPtr node, CondVisitor & condVisitor)
		{
			TreeLinearizeVisitor<NPtr> hv;
			visitOrder(node, hv);
			reversePath(hv.linearizedTree);
			for (auto & val : hv.linearizedTree)
			{
				if (condVisitor(val) == false){
					return false;
				}
			}

			return !hv.linearizedTree.empty();
		}

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka
		//! \param value Warto�c dodawanego w�z�a
		//! \return W�ze� dziecko z zadan� warto�ci�
		static NodePtr addChild(NodePtr parentNode, const value_type & value)
		{
			NodePtr ret;

			if (parentNode != nullptr){
				ret = create(value);			
				parentNode->children.push_back(ret);
				ret->parent = parentNode;
			}

			return ret;
		}

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka
		//! \param value Warto�c dodawanego w�z�a
		//! \return W�ze� dziecko z zadan� warto�ci�
		static NodePtr addChild(NodePtr parentNode, value_type && value)
		{
			NodePtr ret;

			if (parentNode != nullptr){
				ret = create(std::move(value));
				parentNode->children.push_back(ret);
				ret->parent = parentNode;
			}

			return ret;
		}

		//! Destruktor
		~TreeNode() {}

		//! \return Rodzenstwo na tym samym poziomie bez mojego wezla
		Children siblings() const
		{
			auto ret = allSiblings();

			for (auto it = ret.begin(); it != ret.end(); ++it)
			{
				if ((*it).get() == this){
					ret.erase(it);
					break;
				}
			}

			return ret;
		}

		//! \return Ca�e rodze�stwo - razem z moim w�z�em
		Children allSiblings() const
		{
			auto p = parent.lock();
			if (p != nullptr){
				return p->children;
			}

			return Children();
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym przodkiem
		//! \return Prawda je�li w�ze� jest naszym przodkiem (znajdziemy go id�c w g�re po parentach)
		bool isAncestor(NodeConstPtr node) const
		{
			return rawIsAncestor(node.get());
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym potomkiem
		//! \return Prawda je�li w�ze� jest naszym potomkiem (nasz w�ze� znajdziemy go id�c w g�re po parentach tego w�z�a)
		bool isDescendant(NodeConstPtr node) const
		{
			return node->rawIsAncestor(this);
		}

		//! \return Czy w�ze� jest li�ciem - nie ma dzieci
		bool isLeaf() const
		{
			return children.empty();
		}

		//! \return Czy w�ze� jest rootem - nie ma parenta
		bool isRoot() const
		{
			return parent.expired();
		}

		//! \return Stopie� w�z�a - Ilo�� po��cze� jakie tworzy
		size_type degree() const
		{
			return children.size() + ((isRoot() == true) ? 0 : 1);
		}

		//! \return Wysoko�c w�z�a - odleg�o�� do roota
		size_type height() const
		{
			size_type l = 0;
			auto p = this;

			while (p->isRoot() == false) { p = p->parent.lock().get(); ++l; }

			return l;
		}

		//! \return G��boko�� w�z�a - poziom najdalej oddalonego dziecka
		size_type depth() const
		{
			size_type l = 0;

			if (isLeaf() == false){				
				for (const auto & child : children)
				{
					MaxLevelVisitor<NodeConstPtr> mlVisitor;
					visitLevelOrder(child, mlVisitor);
					l = std::max(l, mlVisitor.level);
				}

				++l;
			}			

			return l;
		}

		//! To samo co depth
		size_type level() const
		{
			return depth();
		}

		//! Rodzic
		NodeWeakPtr parent;
		//! Warto��
		value_type value;
		//! Dzieci
		Children children;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREENODE_H__
