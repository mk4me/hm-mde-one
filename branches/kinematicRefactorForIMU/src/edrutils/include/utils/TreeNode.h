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

namespace utils
{
	//! \tparam NPtr Typ wskaŸnika wêz³a
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

		//! \param node odwiedzany wêze³
		void operator()(NPtr node)
		{
			linearizedTree.push_back(node);
		}

		//! Zlinearyzowane drzewo
		LinearizedTree linearizedTree;
	};

	//! \tparam NPtr Typ wskaŸnika wêz³a
	template<typename NPtr>
	class MaxLevelVisitor
	{
	public:
		//! Konstruktor domyslny
		MaxLevelVisitor() : level(0) {}
		//! Destruktor
		~MaxLevelVisitor() {}
		//! \param node Odwiedzany wêze³
		//! \param level Aktualnie odwiedzany poziom (wzgledem wêz³a z którego startowaliœmy)
		void operator()(NPtr node, const unsigned int level)
		{
			this->level = std::max(this->level, level);
		}

		//! Maksymalny poziom
		unsigned int level;
	};

	//! \tparam ValueType Typ rpzechowywanej wartoœci
	template<typename ValueType>
	struct TreeNode
	{
	public:
		//! Typ przechowywanych w wêz³ach wartoœci
		typedef ValueType value_type;
		//! Typ naszego wêz³a
		typedef TreeNode<value_type> node_type;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);	
		//! Dzieci wezla
		typedef std::list<NodePtr> Children;	
		//! Typ iloœci
		typedef typename Children::size_type size_type;
		//! Typ œcie¿ki pomiêdzy wêz³ami
		typedef Children Path;

	private:

		//! \param value Wartoœc przechowywana w wêŸle (kopiowanie)
		TreeNode(const value_type & value) : value(value) {}
		//! \param value Wartoœc przechowywana w wêŸle (move semantics)
		TreeNode(value_type && value) : value(std::move(value)) {}

		//! \param node Wêze³ który sprawdzamy pod k¹tem bycia naszym przodkiem
		//! \return Czy dany wezê³ jest naszym przodkiem (id¹c w górê po parentach trafimy do niego)
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
		
		//! \param value Wartoœæ z jak¹ tworzymy wêze³
		//! \return Nowy wêze³ z zadan¹ wartoœci¹
		static NodePtr create(const value_type & value = value_type())
		{
			return NodePtr(new node_type(value));
		}

		//! \param value Wartoœæ z jak¹ tworzymy wêze³
		//! \return Nowy wêze³ z zadan¹ wartoœci¹
		static NodePtr create(value_type && value)
		{
			return NodePtr(new node_type(std::move(value)));
		}

		//! \tparam NPtr Typ wskaŸnika wêz³a
		template<typename NPtr>
		//! \param Œcie¿ka której d³ugoœæ obliczam
		//! \return D³ugoœæ œcie¿ki
		static size_type distance(const std::list<NPtr> & path)
		{
			return path.size() - 1;
		}

		//! \param startNode Weze³ z którego ma wystartowaæ œcie¿ka
		//! \param endNode Weze³ do którego dojœæ œcie¿ka
		//! \return D³ugoœæ œcie¿ki pomiêdzy wêz³ami
		static size_type distance(NodeConstPtr startNode, NodeConstPtr endNode)
		{			
			return distance(findPath(startNode, endNode));
		}

		//! \tparam NPtr Typ wskaŸnika wêz³a
		template<typename NPtr>
		//! \param path [out] Œcie¿ka któr¹ odwracamy
		static void reversePath(std::list<NPtr> & path)
		{
			std::reverse(path.begin(), path.end());
		}

		//! \tparam NPtr Typ wskaŸnika wêz³a
		template<typename NPtr>
		//! \param startNode Wêze³ z którego ma siê rozpocz¹æ œcie¿ka
		//! \param endNode Wêze³ w którym ma siê koñczyæ œcie¿ka
		//! \return Œcie¿ka pomiêdzy wêz³ami - kolejne wêz³y
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
						//mamy punkt przeciecia - drogi siê schodz¹
						
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

		//! \tparam NPtr Typ wskaŸnika wêz³a
		template<typename NPtr>
		//! \param node Wêze³ z którego chcemy pobraæ œcie¿kê do roota
		static std::list<NPtr> upPath(NPtr startNode)
		{
			std::list<NPtr> ret;			
			while (startNode != nullptr && startNode->isRoot() == false) { ret.push_back(startNode); startNode = startNode->parent.lock(); }
			return ret;
		}

		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam Visitor Typ odwiedzaj¹cego wêz³y
		template<typename NPtr, typename Visitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param visitor Obiekt przegl¹daj¹cy wez³y
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

		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam Visitor Typ odwiedzaj¹cego wêz³y
		template<typename NPtr, typename Visitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param visitor Obiekt przegl¹daj¹cy wez³y
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

		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam Visitor Typ odwiedzaj¹cego wêz³y
		template<typename Visitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param visitor Obiekt przegl¹daj¹cy wez³y i poziomy
		static void visitLevelOrder(NodePtr node, Visitor & visitor)
		{
			if (node != nullptr){

				Children children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nastêpnego poziomu dzieci
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

		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam Visitor Typ odwiedzaj¹cego wêz³y
		template<typename Visitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param visitor Obiekt przegl¹daj¹cy wez³y i poziomy
		static void visitLevelOrder(NodeConstPtr node, Visitor & visitor)
		{
			if (node != nullptr){

				std::list<NodeConstPtr> children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nastêpnego poziomu dzieci
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

		//! \tparam VisitOrder Sposób oryginalnego odwiedzania, które chcemy odwróciæ
		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam Visitor Typ odwiedzaj¹cego wêz³y
		template<typename VisitOrder, typename NPtr, typename Visitor>
		//! \param visitOrder Sposób przegl¹dania w kierunku który bêdziemy odwracaæ
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param visitor Obiekt przegl¹daj¹cy wez³y i poziomy
		static void visitReverseOrder(VisitOrder visitOrder, NPtr node, Visitor & visitor)
		{
			TreeLinearizeVisitor<NPtr> hv;
			visitOrder(node, hv);
			reversePath(hv.linearizedTree);
			for (auto & val : hv.linearizedTree)
			{
				visitor(*it);
			}
		}

		//! \tparam CondVisitor Typ odwiedzaj¹cego wêz³y
		template<typename CondVisitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param condVisitor Obiekt przegl¹daj¹cy wez³y i poziomy z warunkiem
		static bool visitLevelOrderWhile(NodePtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){

				Children children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nastêpnego poziomu dzieci
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

		//! \tparam CondVisitor Typ odwiedzaj¹cego wêz³y
		template<typename CondVisitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param condVisitor Obiekt przegl¹daj¹cy wez³y i poziomy z warunkiem
		//! \return Czy nast¹pi³a przerwa przy przechodzeniu drzewa
		static bool visitLevelOrderWhile(NodeConstPtr node, CondVisitor & condVisitor)
		{
			if (node != nullptr){

				std::list<NodeConstPtr> children;
				children.push_back(node);
				size_type level = 0;
				while (children.empty() == false)
				{
					//pobieramy dla nastêpnego poziomu dzieci
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

		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam CondVisitor Typ odwiedzaj¹cego wêz³y
		template<typename NPtr, typename CondVisitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param condVisitor Obiekt przegl¹daj¹cy wez³y i poziomy z warunkiem
		//! \return Czy nast¹pi³a przerwa przy przechodzeniu drzewa
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

		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam CondVisitor Typ odwiedzaj¹cego wêz³y
		template<typename NPtr, typename CondVisitor>
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param condVisitor Obiekt przegl¹daj¹cy wez³y i poziomy z warunkiem
		//! \return Czy nast¹pi³a przerwa przy przechodzeniu drzewa
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

		//! \tparam VisitOrder Sposób oryginalnego odwiedzania, które chcemy odwróciæ
		//! \tparam NPtr Typ wskaŸnika wêz³a
		//! \tparam CondVisitor Typ odwiedzaj¹cego wêz³y
		template<typename VisitOrder, typename NPtr, typename CondVisitor>
		//! \param visitOrder Sposób przegl¹dania w kierunku który bêdziemy odwracaæ
		//! \param node Wêze³ w którym zaczynamy przegl¹danie drzewa
		//! \param condVisitor Obiekt przegl¹daj¹cy wez³y i poziomy z warunkiem
		static void visitReverseOrderWhile(VisitOrder visitOrder, NPtr node, CondVisitor & condVisitor)
		{
			TreeLinearizeVisitor<NPtr> hv;
			visitOrder(node, hv);
			reversePath(hv.linearizedTree);
			for (auto & val : hv.linearizedTree)
			{
				if (condVisitor(*it) == false){
					return false;
				}
			}

			return !hv.linearizedTree.empty();
		}

		//! \param parentNode Wêze³ do którego dodajemy wartoœæ dziecka
		//! \param value Wartoœc dodawanego wêz³a
		//! \return Wêze³ dziecko z zadan¹ wartoœci¹
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

		//! \param parentNode Wêze³ do którego dodajemy wartoœæ dziecka
		//! \param value Wartoœc dodawanego wêz³a
		//! \return Wêze³ dziecko z zadan¹ wartoœci¹
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

		//! \return Ca³e rodzeñstwo - razem z moim wêz³em
		Children allSiblings() const
		{
			auto p = parent.lock();
			if (p != nullptr){
				return p->children;
			}

			return Children();
		}

		//! \param node Weze³ który sprawdzamy czy jest naszym przodkiem
		//! \return Prawda jeœli wêze³ jest naszym przodkiem (znajdziemy go id¹c w góre po parentach)
		bool isAncestor(NodeConstPtr node) const
		{
			return rawIsAncestor(node.get());
		}

		//! \param node Weze³ który sprawdzamy czy jest naszym potomkiem
		//! \return Prawda jeœli wêze³ jest naszym potomkiem (nasz wêze³ znajdziemy go id¹c w góre po parentach tego wêz³a)
		bool isDescendant(NodeConstPtr node) const
		{
			return node->rawIsAncestor(this);
		}

		//! \return Czy wêze³ jest liœciem - nie ma dzieci
		bool isLeaf() const
		{
			return children.empty();
		}

		//! \return Czy wêze³ jest rootem - nie ma parenta
		bool isRoot() const
		{
			return parent.expired();
		}

		//! \return Stopieñ wêz³a - Iloœæ po³¹czeñ jakie tworzy
		size_type degree() const
		{
			return children.size() + ((isRoot() == true) ? 0 : 1);
		}

		//! \return Wysokoœc wêz³a - odleg³oœæ do roota
		size_type height() const
		{
			size_type l = 0;
			auto p = this;

			while (p->isRoot() == false) { p = p->parent.lock().get(); ++l; }

			return l;
		}

		//! \return G³êbokoœæ wêz³a - poziom najdalej oddalonego dziecka
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
		//! Wartoœæ
		value_type value;
		//! Dzieci
		Children children;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREENODE_H__