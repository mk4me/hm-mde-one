/********************************************************************
	created:  2015/01/15	13:30:08
	filename: TreeNode.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__TREENODE_H__
#define __HEADER_GUARD_UTILS__TREENODE_H__

#include <utils/Utils.h>
#include <utils/SmartPtr.h>
#include <algorithm>
#include <list>
#include <set>

namespace utils
{
	template<typename T>
	class TreeNodeT;

	//! Klasa z podstawowymi typami i generycznymi algorytmami dla drzewa
	struct TreeNode
	{
		//! Typ wyliczeniowy opisujący kierunek linearyzacji drzewa
		enum Order {
			Forward, //! Zgodnie z kolejnością
			Backward //! W odwrotrnej kolejności
		};

		//! Wskaźniki
		template<typename T>
		using NodePtr = utils::shared_ptr < TreeNodeT<T> > ;
		template<typename T>
		using NodeConstPtr = utils::shared_ptr <const TreeNodeT<T> > ;
		template<typename T>
		using NodeWeakPtr = utils::weak_ptr <TreeNodeT<T> > ;
		template<typename T>
		using NodeWeakConstPtr = utils::weak_ptr <const TreeNodeT<T> > ;

		//Typ kolekcji węzłów
		//! \tparam Typ dziecka
		template<typename T>
		using Nodes = std::list < T > ;		

		//! Typ rozmiaru
		using SizeType = Nodes<NullType>::size_type;

		//! Klasa wizytująca, zliczająca ilość węzłów drzewa
		class SizeVisitor
		{
		public:

			//! Konstruktor domyslny
			SizeVisitor() : treeSize_(0) {}
			//! Destruktor
			~SizeVisitor() {}

			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
			template<typename NPtr, typename... Args>
			//! \param node odwiedzany w�ze�
			void operator()(NPtr node, Args...)
			{
				++treeSize_;
			}

			//! Resetuje licznik
			void reset()
			{
				treeSize_ = 0;
			}

			//! \return Ilość węzłów drzewa
			SizeType treeSize() const
			{
				return treeSize_;
			}

		private:

			//! Ilość węzłów w drzewie
			SizeType treeSize_;
		};

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr, int Order>
		//! Klasa linearyzująca drzewo
		class LinearizeVisitor
		{
			static_assert(Order == Forward || Order == Backward, "Undefined order");

		public:

			using Nodes = TreeNode::Nodes < NPtr > ;

		public:
			//! Konstruktor domyslny
			LinearizeVisitor() {}
			//! Destruktor
			~LinearizeVisitor() {}
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
			template<typename... Args>
			//! \param node odwiedzany w�ze�
			void operator()(NPtr node, Args...)
			{
				linearizedTree_.push_back(node);
			}

			//! \return Zlinearyzowane drzewo
			const Nodes & linearizedNode() const
			{
				return linearizedTree_;
			}

			//! Czyści zlinearyzowaną reprezentacje drzewa
			void reset()
			{
				Nodes().swap(linearizedTree_);
			}

		private:

			//! Zlinearyzowane drzewo
			Nodes linearizedTree_;
		};

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! Klasa linearyzująca drzewo
		class LinearizeVisitor<NPtr, Backward>
		{
		public:

			using Nodes = TreeNode::Nodes < NPtr > ;

		public:
			//! Konstruktor domyslny
			LinearizeVisitor() {}
			//! Destruktor
			~LinearizeVisitor() {}
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
			template<typename... Args>
			//! \param node odwiedzany w�ze�
			void operator()(NPtr node, Args...)
			{
				linearizedTree_.push_front(node);
			}

			//! \return Zlinearyzowane drzewo
			const Nodes & linearizedNode() const
			{
				return linearizedTree_;
			}

			//! Czyści zlinearyzowaną reprezentacje drzewa
			void reset()
			{
				Nodes().swap(linearizedTree_);
			}

		private:

			//! Zlinearyzowane drzewo
			Nodes linearizedTree_;
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
			void operator()(NPtr, const SizeType level)
			{
				maxLevel_ = std::max(maxLevel_, level);
			}

			//! \return Najgłebszy poziom drzewa
			SizeType maxLevel() const
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
			SizeType maxLevel_;
		};

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy przodka
		//! \param ancestor Weryfikowany węzeł przodek
		static bool isAncestor(NPtrA me, NPtrB * ancestor)
		{
			if (me == nullptr || me.get() == ancestor){
				return false;
			}

			auto p = me->parent();

			while (p.get() != ancestor) { p = p->parent(); }

			return p.get() == ancestor;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy przodka
		//! \param ancestor Weryfikowany węzeł przodek
		static bool isAncestor(NPtrA me, NPtrB ancestor)
		{
			if (me == nullptr || me == ancestor){
				return false;
			}

			auto p = me->parent();

			while (p != ancestor) { p = p->parent(); }

			return p == ancestor;
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy potomka
		//! \param ancestor Weryfikowany węzeł potomek
		static bool isDescendant(NPtrA me, NPtrB descendant)
		{
			return isAncestor(descendant, me);
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
			if (nodeA == nodeB){

			} else {
				//no to porównujemy
				//szybko po rozmiarze
				ret = (nodeA->children_.size() == nodeB->children_.size());

				if (ret == true){

					ret == comp(nodeA->value, nodeB->value);

					if (ret == true){
						//idziemy głębiej
						auto itA = nodeA->children_.begin();
						auto itB = nodeB->children_.begin();
						for (;
							(itA != nodeA->children_.end()) && (itB != nodeB->children_.end()) && ((ret = compare(*itA, *itB, comp)) == true);
							++itA, ++itB) {
						}

						if (ret == true && (itA != nodeA->children_.end() || itB != nodeB->children_.end())){
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
			//zakładamy że struktury takie same
			bool ret = true;
			if (nodeA == nodeB){

			} else{
				//no to porównujemy
				//szybko po rozmiarze
				ret = (nodeA->children_.size() == nodeB->children_.size());

				if (ret == true){
					//idziemy głębiej
					auto itA = nodeA->children_.begin();
					auto itB = nodeB->children_.begin();
					for (;
						(itA != nodeA->children_.end()) && (itB != nodeB->children_.end()) && ((ret = compareStructure(*itA, *itB)) == true);
						++itA, ++itB) {}

					if (ret == true && (itA != nodeA->children_.end() || itB != nodeB->children_.end())){
						ret = false;
					}
				}
			}

			return ret;
		}

		//! Polityka odwiedzania węzłów
		struct PreOrderVisitPolicy
		{
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename Visitor>
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(NPtr root, Visitor & visitor)
			{
				visitor(root);
				for (const auto & child : root->children_)
				{
					visit(NPtr(child), visitor);
				}
			}
		};	

		//! Polityka odwiedzania węzłów
		struct PostOrderVisitPolicy
		{
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename Visitor>
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param visitor Obiekt przegl�daj�cy wez�y
			static void visit(NPtr root, Visitor & visitor)
			{
				for (const auto & child : root->children_)
				{
					visit(NPtr(child), visitor);
				}
				visitor(root);
			}
		};

		//! Polityka odwiedzania węzłów
		struct LevelOrderVisitPolicy
		{
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename Visitor>
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
			static void visit(NPtr root, Visitor & visitor)
			{
				Nodes<NPtr> nodes;
				nodes.push_back(root);
				SizeType level = 0;
				while (nodes.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					Nodes<NPtr> nextLevelNodes;
					for (const auto & node : nodes)
					{
						visitor(node, level);

						for (const auto & child : node->children_){
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
		struct ReverseOrderVisitPolicy
		{		
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam Visitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename Visitor>
			//! \param visitOrder Spos�b przegl�dania w kierunku kt�ry b�dziemy odwraca�
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param visitor Obiekt przegl�daj�cy wez�y i poziomy
			static void visit(NPtr root, Visitor & visitor)
			{
				auto lt = linearize<NPtr, VisitOrder, Backward>(root);
				for (const auto & node : lt)
				{
					visitor(node);
				}
			}
		};

		//! Polityka odwiedzania węzłów z warunkiem
		struct LevelOrderWhileVisitPolicy
		{
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename CondVisitor>
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			static bool visitWhile(NPtr root, CondVisitor & condVisitor)
			{
				Nodes<NPtr> nodes;
				nodes.push_back(root);
				SizeType level = 0;
				while (nodes.empty() == false)
				{
					//pobieramy dla nast�pnego poziomu dzieci
					Nodes<NPtr> nextLevelNodes;
					for (const auto & node : nodes)
					{
						if (condVisitor(node, level) == false){
							return false;
						}
					
						for (const auto & child : node->children_) {
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
		struct PreOrderWhileVisitPolicy
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
					for (const auto & child : root->children_)
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
		struct PostOrderWhileVisitPolicy
		{
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename CondVisitor>
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			//! \return Czy nast�pi�a przerwa przy przechodzeniu drzewa
			static bool visitWhile(NPtr root, CondVisitor & condVisitor)
			{
				for (const auto & child : root->children_)
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
		struct ReverseOrderWhileVisitPolicy
		{			
			//! \tparam NPtr Typ wska�nika w�z�a
			//! \tparam CondVisitor Typ odwiedzaj�cego w�z�y
			template<typename NPtr, typename CondVisitor>
			//! \param visitOrder Spos�b przegl�dania w kierunku kt�ry b�dziemy odwraca�
			//! \param node W�ze� w kt�rym zaczynamy przegl�danie drzewa
			//! \param condVisitor Obiekt przegl�daj�cy wez�y i poziomy z warunkiem
			static bool visitWhile(NPtr root, CondVisitor & condVisitor)
			{
				auto lt = linearize<NPtr, VisitOrder, Backward>(root);
				for (const auto & node : lt)
				{
					if (condVisitor(node) == false){
						return false;
					}
				}

				return !lt.empty();
			}
		};

		//! \tparam NPtr Typ wska�nika w�z�a
		//! \tparam Visitor Typ odwiedzaj�cego w�z�y
		template<typename NPtr, typename LinearizationVisitPolicy = PreOrderVisitPolicy, int Order = Forward>
		//! \param node Węzeł od którego zaczynamy linearyzacje struktury
		//! \param lp Polityka linearyzowania drzewa
		//! \return Ścieżka zlinearyzowanego drzewa
		static Nodes<NPtr> linearize(NPtr node)
		{
			LinearizeVisitor<NPtr, Order> tlv;
			LinearizationVisitPolicy::visit(node, tlv);
			return tlv.linearizedNode();
		}

		//! \tparam NPtr Typ wska�nika w�z�a
		template<typename NPtr>
		//! \param node Węzeł dla którego liczymy dzieci
		static SizeType childrenSize(NPtr node)
		{
			SizeType ret = 0;

			for (const auto & child : node->children_)
			{
				SizeVisitor tsv;
				PreOrderVisitPolicy::visit(child, tsv);
				ret += tsv.treeSize();
			}

			return ret;
		}
	};

	//! \tparam ValueType Typ rpzechowywanej warto�ci
	template<typename ValueType>
	class TreeNodeT
	{
		friend struct TreeNode;

	public:
		//! Typ przechowywanych w w�z�ach warto�ci
		typedef ValueType value_type;
		//! Typ naszego w�z�a
		typedef TreeNodeT<value_type> node_type;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);	
		//! Dzieci wezla
		typedef TreeNode::Nodes<NodePtr> Children;	
		//! Typ ilo�ci
		typedef typename TreeNode::SizeType size_type;

	private:

		//! \param other Węzeł przenoszony wraz z wartością
		TreeNodeT(TreeNodeT && other)
			: parent_(std::move(other.parent_)),
			children_(std::move(other.children_)),
			value_(std::move(other.value_))
		{}

		//! \param value Warto�c przechowywana w w�le (kopiowanie)
		TreeNodeT(const value_type & value) : value_(value) {}
		//! \param value Warto�c przechowywana w w�le (move semantics)
		TreeNodeT(value_type && value) : value_(std::move(value)) {}

		//! \param value Warto�c przechowywana w w�le (kopiowanie)
		TreeNodeT(NodePtr parent, const value_type & value) : parent_(parent), value_(value) {}
		//! \param value Warto�c przechowywana w w�le (move semantics)
		TreeNodeT(NodePtr parent, value_type && value) : parent_(parent), value_(std::move(value)) {}

	public:

		//! \param node Węzeł od którego zaczynamy kopiowanie - automatycznie staje się rootem!!
		//! \return Klon danego węzła będący rootem nowego drzewa
		static NodePtr clone(NodeConstPtr node)
		{
			NodePtr ret(create(node->value_));

			for (const auto & cn : node->children_)
			{
				auto c(clone(cn));
				c->parent_ = ret;
				ret->children_.push_back(c);
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
		//! \param node Węzeł
		//! \return Czy udało się dodać węzeł
		static void add(NodePtr parentNode, NodePtr node)
		{
			if (node == parentNode){
				throw std::runtime_error("Recursive addition");
			}
			
			if (node->isRoot() == false) {
				throw std::runtime_error("Addition of non-root node");
			}
			
			parentNode->children_.push_back(node);
			node->parent_ = parentNode;			
		}

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka
		//! \param value Warto�c dodawanego w�z�a
		//! \return W�ze� dziecko z zadan� warto�ci�
		static NodePtr add(NodePtr parentNode, const value_type & value)
		{
			NodePtr ret(new TreeNodeT(parentNode, value));			
			parentNode->children_.push_back(ret);
			return ret;
		}

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka
		//! \param value Warto�c dodawanego w�z�a
		//! \return W�ze� dziecko z zadan� warto�ci�
		static NodePtr add(NodePtr parentNode, value_type && value)
		{
			NodePtr ret(new TreeNodeT(parentNode, std::move(value)));			
			parentNode->children_.push_back(ret);
			return ret;
		}

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka
		//! \param value Warto�c dodawanego w�z�a
		//! \return W�ze� dziecko z zadan� warto�ci�
		static void swap(NodePtr nodeToReplace, NodePtr node)
		{
			if ((nodeToReplace == node) ||
				((nodeToReplace->parent() == nullptr) && (node->parent() == nullptr)))
			{
				return;
			}

			//wspólny rodzic? To znaczy że zamieniamy ich tylko miejscami
			if (nodeToReplace->parent() == node->parent())
			{
				auto itA = std::find(parentNode->children_.begin(), parentNode->children_.end(), nodeToReplace);
				auto itB = std::find(parentNode->children_.begin(), parentNode->children_.end(), node);

				std::swap(*itA, *itB);
			}
			else{
				//przypadek ogólny - zmiana struktury drzew
				auto parentNode = nodeToReplace->parent();
				if (parentNode != nullptr){
					auto it = std::find(parentNode->children_.begin(), parentNode->children_.end(), nodeToReplace);
					*it = node;
				}

				nodeToReplace->parent_ = node->parent_;
				node->parent_ = parentNode;

				parentNode = nodeToReplace->parent();
				if (parentNode != nullptr){
					it = std::find(parentNode->children_.begin(), parentNode->children_.end(), node);
					*it = nodeToReplace;
				}
			}
		}

		//! \param node usuwany węzeł z hierarchi, jego dzieci przechodza wyżej na koniec listy dzieci (nie może być to root)
		static void remove(NodePtr node)
		{
			auto parent = node->parent();
			if (parent != nullptr){				
				parent->children_.erase(std::remove(parent->children_.begin(), parent->children_.end(), node), parent->children_.end());
				node->parent_ = NodePtr();				
			}
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
			auto p = parent();
			if (p != nullptr){
				return p->children_;
			}

			return Children();
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym przodkiem
		//! \return Prawda je�li w�ze� jest naszym przodkiem (znajdziemy go id�c w g�re po parentach)
		bool isAncestor(NodeConstPtr node) const
		{
			return TreeNode::isAncestor(this, node);
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym potomkiem
		//! \return Prawda je�li w�ze� jest naszym potomkiem (nasz w�ze� znajdziemy go id�c w g�re po parentach tego w�z�a)
		bool isDescendant(NodeConstPtr node) const
		{
			return TreeNode::isDescendant(this, node);
		}

		//! \return Czy w�ze� jest li�ciem - nie ma dzieci
		bool isLeaf() const
		{
			return children_.empty();
		}

		//! \return Czy w�ze� jest rootem - nie ma parenta
		bool isRoot() const
		{
			return parent() == nullptr;
		}

		//! \return Stopie� w�z�a - Ilo�� po��cze� jakie tworzy
		size_type degree() const
		{			
			return children_.size();
		}

		//! \return Wysoko�c w�z�a - odleg�o�� do roota
		size_type depth() const
		{
			size_type l = 0;
			auto p = this;

			while (p->isRoot() == false) { p = p->parent_.lock().get(); ++l; }

			return l;
		}

		//! \return G��boko�� w�z�a - poziom najdalej oddalonego dziecka
		size_type height() const
		{
			size_type l = 0;

			if (isLeaf() == false){				
				for (const auto & child : children_)
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
		size_type level() const
		{
			return depth() + 1;
		}

		//! \return Ilość węzłów w tym drzewie
		size_type size() const
		{
			return 1 + TreeNode::childrenSize(this);
		}

		//! Rodzic
		NodePtr parent() { return parent_.lock(); };
		//! Rodzic
		NodeConstPtr parent() const { return parent_.lock(); };
		//! Wartość
		value_type & value() { return value_; };
		//! Wartość
		const value_type & value() const { return value_; };
		//TODO - udostępnianie dzieci
		//! Dzieci
		const Children & children() const { return children_; }
		//TODO - udostępnianie dzieci
		//! Dzieci
		Children children() { return children_; }

	private:

		//! Rodzic
		NodeWeakPtr parent_;
		//! Warto��
		value_type value_;
		//! Dzieci
		Children children_;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREENODE_H__
