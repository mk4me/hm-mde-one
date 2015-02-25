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
	class FindMaxLevelVisitor
	{
	public:
		//! Konstruktor domyslny
		FindMaxLevelVisitor() : maxLevel(0) {}
		//! Destruktor
		~FindMaxLevelVisitor() {}
		//! \param node Odwiedzany w�ze�
		//! \param level Aktualnie odwiedzany poziom (wzgledem w�z�a z kt�rego startowali�my)
		void operator()(NPtr node, const unsigned int level)
		{
			maxLevel = std::max(maxLevel, level);
		}

		//! Maksymalny poziom
		unsigned int maxLevel;
	};

	struct TreeNode
	{
		//Typ ściezki
		template<typename T>
		using Path = std::list < T > ;

		//Typ kolekcji dzieci
		template<typename T>
		using Children = Path < T > ;

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param me Węzeł dla którego sprawdzamy przodka
		//! \param ancestor Weryfikowany węzeł przodek
		static bool isAncestor(NPtr me, NPtr ancestor)
		{
			if (me == nullptr || me == ancestor){
				return false;
			}

			auto p = me->parent.lock();

			while (p.get() != ancestor) { p = p->parent.lock(); }

			return p.get() == ancestor;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param me Węzeł dla którego sprawdzamy potomka
		//! \param ancestor Weryfikowany węzeł potomek
		static bool isDescendant(NPtr me, NPtr descendant)
		{
			return isAncestor(descendant, me);
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param �cie�ka kt�rej d�ugo�� obliczam
		//! \return D�ugo�� �cie�ki
		static typename Path<NPtr>::size_type distance(const Path<NPtr> & path)
		{
			return path.size() - 1;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param startNode Weze� z kt�rego ma wystartowa� �cie�ka
		//! \param endNode Weze� do kt�rego doj�� �cie�ka
		//! \return D�ugo�� �cie�ki pomi�dzy w�z�ami
		static typename Path<NPtr>::size_type distance(NPtr startNode, NPtr endNode)
		{
			return distance(findPath(startNode, endNode));
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
		//! \param startNode W�ze� z kt�rego ma si� rozpocz�� �cie�ka
		//! \param endNode W�ze� w kt�rym ma si� ko�czy� �cie�ka
		//! \return �cie�ka pomi�dzy w�z�ami - kolejne w�z�y
		static Path<NPtr> findPath(NPtr startNode, NPtr endNode)
		{
			Path<NPtr> ret;

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
		static Path<NPtr> upPath(NPtr startNode)
		{
			Path<NPtr> ret;
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
				for (const auto & child : node->children)
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
				for (const auto & child : node->children)
				{
					visitPostOrder(NPtr(child), visitor);
				}
				visitor(node);
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
		static void visitLevelOrder(NPtr node, Visitor & visitor)
		{
			if (node != nullptr){

				Children<NPtr> children;
				children.push_back(node);
				typename Children<NPtr>::size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					Children<NPtr> nextLevelChildren;
					for (const auto & child : children)
					{
						visitor(NPtr(child), level);

						if (child != nullptr){
							for (const auto & val : child->children){
								nextLevelChildren.push_back(NPtr(val));
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
			for (const auto & val : hv.linearizedTree)
			{
				visitor(NPtr(val));
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		static bool visitLevelOrderWhile(NPtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){

				Children<NPtr> children;
				children.push_back(node);
				typename Children<NPtr>::size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					Children<NPtr> nextLevelChildren;
					for (const auto & child : children)
					{
						if (condVisitor(NPtr(child), level) == false){
							return false;
						}

						if (child != nullptr){
							for (const auto & val : child->children) {
								nextLevelChildren.push_back(NPtr(val));
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
					for (const auto & child : node->children)
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
				for (const auto & child : node->children)
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
			for (const auto & val : hv.linearizedTree)
			{
				if (condVisitor(NPtr(val)) == false){
					return false;
				}
			}

			return !hv.linearizedTree.empty();
		}
	};

	//! \tparam ValueType Typ rpzechowywanej warto�ci
	template<typename ValueType>
	struct TreeNodeT
	{
	public:
		//! Typ przechowywanych w w�z�ach warto�ci
		typedef ValueType value_type;
		//! Typ naszego w�z�a
		typedef TreeNodeT<value_type> node_type;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);	
		//! Dzieci wezla
		typedef TreeNode::Children<NodePtr> Children;	
		//! Typ ilo�ci
		typedef typename Children::size_type size_type;
		//! Typ �cie�ki pomi�dzy w�z�ami
		typedef Children Path;

	private:

		//! \param other Węzeł przenoszony wraz z wartością
		TreeNodeT(TreeNodeT && other)
			: parent(std::move(other.parent)),
			children(std::move(other.children)),
			value(std::move(other.value))
		{}

		//! \param value Warto�c przechowywana w w�le (kopiowanie)
		TreeNodeT(const value_type & value) : value(value) {}
		//! \param value Warto�c przechowywana w w�le (move semantics)
		TreeNodeT(value_type && value) : value(std::move(value)) {}

	public:

		//! \param node Węzeł od którego zaczynamy kopiowanie - automatycznie staje się rootem!!
		//! \return Klon danego węzła będący rootem nowego drzewa
		static NodePtr clone(NodeConstPtr node)
		{
			NodePtr ret(create(node->value));

			for (const auto & cn : node->children)
			{
				auto c(clone(cn));
				c->parent = ret;
				ret->children.push_back(c);
			}

			return ret;
		}
		
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
		~TreeNodeT() {}

		//! \return Rodzenstwo na tym samym poziomie bez mojego wezla
		Children siblings() const
		{
			auto ret = allSiblings();

			auto it = std::find_if(ret.begin(), ret.end(), [this](NodePtr node) { return node.get() == this; });

			if (it != ret.end()){
				ret.erase(it);
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
			return TreeNode::isAncestor(this, node.get());
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym potomkiem
		//! \return Prawda je�li w�ze� jest naszym potomkiem (nasz w�ze� znajdziemy go id�c w g�re po parentach tego w�z�a)
		bool isDescendant(NodeConstPtr node) const
		{
			return TreeNode::isDescendant(this, node.get());
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
					FindMaxLevelVisitor<NodeConstPtr> mlVisitor;
					visitLevelOrder(child, mlVisitor);
					l = std::max(l, mlVisitor.maxLevel);
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

	//! \tparam ValueType Typ rpzechowywanej warto�ci
	template<>
	struct TreeNodeT<void>
	{
	public:
		//! Typ przechowywanych w w�z�ach warto�ci
		typedef void value_type;
		//! Typ naszego w�z�a
		typedef TreeNodeT<void> node_type;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);
		//! Dzieci wezla
		typedef TreeNode::Children<NodePtr> Children;
		//! Typ ilo�ci
		typedef Children::size_type size_type;
		//! Typ �cie�ki pomi�dzy w�z�ami
		typedef Children Path;

	private:

		//! Domyślny konstruktor
		TreeNodeT() {}

		//! \param other Węzeł przenoszony wraz z wartością
		TreeNodeT(TreeNodeT && other)
			: parent(std::move(other.parent)),
			children(std::move(other.children))			
		{}		

	public:

		//! \param node Węzeł od którego zaczynamy kopiowanie - automatycznie staje się rootem!!
		//! \return Klon danego węzła będący rootem nowego drzewa
		static NodePtr clone(NodeConstPtr node)
		{
			NodePtr ret(new node_type());

			for (const auto & cn : node->children)
			{
				auto c(clone(cn));
				c->parent = ret;
				ret->children.push_back(c);
			}

			return ret;
		}
		
		//! \return Nowy w�ze�
		static NodePtr create()
		{
			return NodePtr(new node_type());
		}		

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka		
		//! \return W�ze� dziecko z zadan� warto�ci�
		static NodePtr addChild(NodePtr parentNode)
		{
			NodePtr ret;

			if (parentNode != nullptr){
				ret = create();
				parentNode->children.push_back(ret);
				ret->parent = parentNode;
			}

			return ret;
		}

		//! Destruktor
		~TreeNodeT() {}

		//! \return Rodzenstwo na tym samym poziomie bez mojego wezla
		Children siblings() const
		{
			auto ret = allSiblings();

			auto it = std::find_if(ret.begin(), ret.end(), [this](NodePtr node) { return node.get() == this; });

			if (it != ret.end()){
				ret.erase(it);
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
			return TreeNode::isAncestor(this, node.get());
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym potomkiem
		//! \return Prawda je�li w�ze� jest naszym potomkiem (nasz w�ze� znajdziemy go id�c w g�re po parentach tego w�z�a)
		bool isDescendant(NodeConstPtr node) const
		{
			return TreeNode::isDescendant(this, node.get());
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
					FindMaxLevelVisitor<NodeConstPtr> mlVisitor;
					TreeNode::visitLevelOrder(child, mlVisitor);
					l = std::max(l, mlVisitor.maxLevel);
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
		//! Dzieci
		Children children;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREENODE_H__
