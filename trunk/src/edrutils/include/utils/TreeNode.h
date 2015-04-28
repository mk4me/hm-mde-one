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

	//! Klasa wizytująca, zliczająca ilość węzłów drzewa
	class TreeSizeVisitor
	{
	public:

		//! Konstruktor domyslny
		TreeSizeVisitor() : treeSize_(0) {}
		//! Destruktor
		~TreeSizeVisitor() {}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param node odwiedzany w�ze�
		void operator()(NPtr node)
		{
			++treeSize_;
		}

		//! Resetuje licznik
		void reset()
		{
			treeSize_ = 0;
		}

		//! \return Ilość węzłów drzewa
		std::size_t treeSize() const
		{
			return treeSize_;
		}

	private:

		//! Ilość węzłów w drzewie
		std::size_t treeSize_;
	};

	//! \tparam NPtr Typ wska�nika w�z�a
	template<typename NPtr>
	//! Klasa linearyzująca drzewo
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
			linearizedTree_.push_back(node);
		}

		//! \return Zlinearyzowane drzewo
		const LinearizedTree linearizedTree() const
		{
			return linearizedTree_;
		}

		//! Czyści zlinearyzowaną reprezentacje drzewa
		void reset()
		{
			LinearizedTree().swap(linearizedTree_);
		}

	private:

		//! Zlinearyzowane drzewo
		LinearizedTree linearizedTree_;
	};

	//! Klasa z podstawowymi typami i generycznymi algorytmami dla drzewa
	struct TreeNode
	{
		//! Typ rozmiaru
		typedef std::size_t SizeType;

		//Typ ściezki
		//! \tparam Element ścieżki
		template<typename T>
		using Path = std::list < T > ;

		//Typ kolekcji dzieci
		//! \tparam Typ dziecka
		template<typename T>
		using Children = Path < T > ;

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy przodka
		//! \param ancestor Weryfikowany węzeł przodek
		static bool isAncestor(NPtrA me, NPtrB ancestor)
		{
			if (me == nullptr || me == ancestor){
				return false;
			}

			auto p = me->parent.lock();

			while (p.get() != ancestor) { p = p->parent.lock(); }

			return p.get() == ancestor;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy potomka
		//! \param ancestor Weryfikowany węzeł potomek
		static bool isDescendant(NPtrA me, NPtrB descendant)
		{
			return isAncestor(descendant, me);
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param �cie�ka kt�rej d�ugo�� obliczam
		//! \return D�ugo�� �cie�ki
		static SizeType distance(const Path<NPtr> & path)
		{
			return path.empty() == true ? 0 : path.size() - 1;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param startNode Weze� z kt�rego ma wystartowa� �cie�ka
		//! \param endNode Weze� do kt�rego doj�� �cie�ka
		//! \return D�ugo�� �cie�ki pomi�dzy w�z�ami
		static SizeType distance(NPtrA startNode, NPtrB endNode)
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
		//! \param node W�ze� z kt�rego chcemy pobra� �cie�k� do roota
		static Path<NPtr> upPath(NPtr startNode)
		{
			Path<NPtr> ret;

			if (startNode != nullptr){

				if (startNode->isRoot() == true){
					ret.push_back(startNode);
				}
				else{
					while (startNode != nullptr && startNode->isRoot() == false) { ret.push_back(startNode); startNode = startNode->parent.lock(); }
				}
			}

			return ret;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param node Węzeł dla którego liczymy dzieci
		static SizeType childrenSize(NPtr node)
		{
			SizeType ret = 0;

			if (node != nullptr){

				for (const auto & c : node->children)
				{
					if (c != nullptr){
						TreeSizeVisitor tsv;
						TreeNode::visitPreOrder(c, tsv);
						ret += tsv.treeSize();
					}
					else{
						++ret;
					}
				}
			}

			return ret;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param startNode W�ze� z kt�rego ma si� rozpocz�� �cie�ka
		//! \param endNode W�ze� w kt�rym ma si� ko�czy� �cie�ka
		//! \return �cie�ka pomi�dzy w�z�ami - kolejne w�z�y
		static Path<NPtrA> findPath(NPtrA startNode, NPtrB endNode)
		{
			if (startNode != endNode){
				auto p1 = upPath(startNode);

				//wariant end node jest przodkiem startNode
				{
					auto it = std::find(p1.begin(), p1.end(), endNode);

					if (it != p1.end()){						
						return Path<NPtrA>(p1.begin(), it + 1);
					}
				}

				auto p2 = upPath(endNode);

				//wariant end node jest potomkiem startNode
				{
					auto it = std::find(p2.begin(), p2.end(), startNode);

					if (it != p2.end()){
						Path<NPtrA> ret(p2.begin(), it+1);
						reversePath(ret);						
						return ret;
					}
				}

				if (p1.empty() == false && p2.empty() == false){

					std::set<NPtrA> s1(p1.begin(), p1.end());
					std::set<NPtrA> s2(p2.begin(), p2.end());

					std::vector<NPtrA> intersectResult(std::min(p1.size(), p2.size()));
					auto retIT = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), intersectResult.begin());

					if (retIT != intersectResult.end()){
						//mamy punkt przeciecia - drogi si� schodz�
						//szukam najblizszego punktu przeciecia
						std::set<NPtrA> intersectionSet(intersectResult.begin(), retIT);
						NPtrA intersectionPoint;

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

						Path<NPtrA> ret(p1.begin(), std::find(p1.begin(), p1.end(), intersectionPoint));						
						ret.insert(ret.end(), std::find(p2.begin(), p2.end(), intersectionPoint), p2.end());
						return ret;
					}
				}
				else{
					return Path<NPtrA>();
				}
			}
			else{
				Path<NPtrA> ret;
				ret.push_back(startNode);
				return ret;
			}			
		}

		//! \tparam NPtrA Typ wskaźnika węzła startowego pierwszego drzewa
		//! \tparam NPtrB Typ wskaźnika węzła startowego drugiego drzewa
		//! \tparam Comp Typ obiektu używanego do poównania elementów drzew
		template<typename NPtrA, typename NPtrB,
			typename Comp = std::equal_to<>>
		//! \param nodeA Węzeł startowy pierwszego drzewa
		//! \param nodeB Węzeł startowy drugiego drzewa
		//! \param comp Obiekt porównyjący wartości węzłów
		//! \return Czy oba drzewa są równe - struktura i dane węzłów
		static bool compare(NPtrA nodeA, NPtrB nodeB, Comp comp = Comp())
		{
			//zakładamy że struktury takie same
			bool ret = true;
			//czy już początek nam się rózni?
			if ((nodeA != nullptr && nodeB == nullptr) ||
				(nodeA == nullptr && nodeB != nullptr)){
				ret = false;
			}
			else{
				//no to porównyjemy
				//szybko po rozmiarze
				ret = (nodeA->children.size() == nodeB->children.size());

				if (ret == true){

					ret == comp(nodeA->value, nodeB->value);

					if (ret == true){
						//idziemy głębiej
						auto itA = nodeA->children.begin();
						auto itB = nodeB->children.begin();
						for (;
							(itA != nodeA->children.end()) && (itB != nodeB->children.end()) && ((ret = compare(*itA, *itB, comp)) == true);
							++itA, ++itB) {
						}

						if (ret == true && (itA != nodeA->children.end() || itB != nodeB->children.end())){
							ret = false;
						}
					}
				}
			}

			return ret;
		}

		//! \tparam NPtrA Typ wskaźnika węzła startowego pierwszego drzewa
		//! \tparam NPtrB Typ wskaźnika węzła startowego drugiego drzewa
		template<typename NPtrA, typename NPtrB>
		//! \param nodeA Węzeł startowy pierwszego drzewa
		//! \param nodeB Węzeł startowy drugiego drzewa
		//! \return Czy struktury drzew są takie same
		static bool compareStructure(NPtrA nodeA, NPtrB nodeB)
		{	
			if (nodeA == nodeB){
				return true;
			}

			//zakładamy że struktury takie same
			bool ret = true;
			//czy już początek nam się rózni?
			if ((nodeA != nullptr && nodeB == nullptr) ||
				(nodeA == nullptr && nodeB != nullptr)){
				ret = false;
			}
			else{
				//no to porównyjemy
				//szybko po rozmiarze
				ret = (nodeA->children.size() == nodeB->children.size());

				if (ret == true){
					//idziemy głębiej
					auto itA = nodeA->children.begin();
					auto itB = nodeB->children.begin();
					for (;
						(itA != nodeA->children.end()) && (itB != nodeB->children.end()) && ((ret = compareStructure(*itA, *itB)) == true);
						++itA, ++itB) {}

					if (ret == true && (itA != nodeA->children.end() || itB != nodeB->children.end())){
						ret = false;
					}
				}
			}

			return ret;
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param node usuwany węzeł z hierarchi (nie może być to root)
		static void removeSubTree(NPtr node)
		{
			if (node->isRoot() == false){
				auto parent = node->parent.lock();
				parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), node), parent->children.end());
			}
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param node usuwany węzeł z hierarchi, jego dzieci przechodza wyżej na koniec listy dzieci (nie może być to root)
		static void removeNode(NPtr node)
		{
			if (node->isRoot() == false){
				auto parent = node->parent.lock();
				parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), node), parent->children.end());
				parent->children.insert(parent->children.end(), node->children.begin(), node->children.end());
				node->children.swap(decltype(node->children)());
			}
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y
		static void visitPreOrder(NPtr node, Visitor & visitor)
		{
			visitor(node);
			if (node != nullptr){
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
			}
			visitor(node);
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename Visitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
		static void visitLevelOrder(NPtr node, Visitor & visitor)
		{
			Children<NPtr> children;
			children.push_back(node);
			TreeNode::SizeType level = 0;
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
			auto lt = hv.linearizedTree();
			reversePath(lt);
			for (const auto & val : lt)
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
			Children<NPtr> children;
			children.push_back(node);
			SizeType level = 0;
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

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
		static bool visitPreOrderWhile(NPtr node, CondVisitor & condVisitor)
		{
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

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename CondVisitor>
		//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
		//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
		//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
		static bool visitPostOrderWhile(NPtr node, CondVisitor & condVisitor)
		{
			for (const auto & child : node->children)
			{
				if (visitPostOrderWhile(NPtr(child), condVisitor) == false){
					return false;
				}
			}
			return condVisitor(node);
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
			auto lt = hv.linearizedTree();
			reversePath(lt);
			for (const auto & val : lt)
			{
				if (condVisitor(NPtr(val)) == false){
					return false;
				}
			}

			return !lt.empty();
		}
	};

	//! Klasa wizytująca szukająca najgłebszego poziomu drzewa
	class FindMaxLevelVisitor
	{
	public:
		//! Konstruktor domyslny
		FindMaxLevelVisitor() : maxLevel_(0) {}
		//! Destruktor
		~FindMaxLevelVisitor() {}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param node Odwiedzany w�ze�
		//! \param level Aktualnie odwiedzany poziom (wzgledem w�z�a z kt�rego startowali�my)
		void operator()(NPtr node, const TreeNode::SizeType level)
		{
			maxLevel_ = std::max(maxLevel_, level);
		}

		//! \return Najgłebszy poziom drzewa
		TreeNode::SizeType maxLevel() const
		{
			return maxLevel_;
		}

		//! Zeruje najgłębszy poziom drzewa
		void reset()
		{
			maxLevel_ = 0;
		}

	private:
		//! Maksymalny poziom
		TreeNode::SizeType maxLevel_;
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
		TreeNode::SizeType degree() const
		{
			return children.size() + ((isRoot() == true) ? 0 : 1);
		}

		//! \return Wysoko�c w�z�a - odleg�o�� do roota
		TreeNode::SizeType height() const
		{
			TreeNode::SizeType l = 0;
			auto p = this;

			while (p->isRoot() == false) { p = p->parent.lock().get(); ++l; }

			return l;
		}

		//! \return G��boko�� w�z�a - poziom najdalej oddalonego dziecka
		TreeNode::SizeType depth() const
		{
			TreeNode::SizeType l = 0;

			if (isLeaf() == false){				
				for (const auto & child : children)
				{
					FindMaxLevelVisitor mlVisitor;
					visitLevelOrder(child, mlVisitor);
					l = std::max(l, mlVisitor.maxLevel());
				}

				++l;
			}			

			return l;
		}

		//! To samo co depth
		TreeNode::SizeType level() const
		{
			return depth();
		}

		//! \return Ilość węzłów w tym drzewie
		TreeNode::SizeType size() const
		{
			return 1 + TreeNode::childrenSize(this);
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
		TreeNode::SizeType degree() const
		{
			return children.size() + ((isRoot() == true) ? 0 : 1);
		}

		//! \return Wysoko�c w�z�a - odleg�o�� do roota
		TreeNode::SizeType height() const
		{
			TreeNode::SizeType l = 0;
			auto p = this;

			while (p->isRoot() == false) { p = p->parent.lock().get(); ++l; }

			return l;
		}

		//! \return G��boko�� w�z�a - poziom najdalej oddalonego dziecka
		TreeNode::SizeType depth() const
		{
			TreeNode::SizeType l = 0;

			if (isLeaf() == false){
				for (const auto & child : children)
				{
					FindMaxLevelVisitor mlVisitor;
					TreeNode::visitLevelOrder(child, mlVisitor);
					l = std::max(l, mlVisitor.maxLevel());
				}

				++l;
			}

			return l;
		}

		//! To samo co depth
		TreeNode::SizeType level() const
		{
			return depth();
		}

		//! \return Ilość węzłów w tym drzewie
		TreeNode::SizeType size() const
		{
			return 1 + TreeNode::childrenSize(this);
		}

		//! Rodzic
		NodeWeakPtr parent;
		//! Dzieci
		Children children;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREENODE_H__
