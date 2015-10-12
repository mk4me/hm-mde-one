/********************************************************************
	created:  2015/07/22	13:07:42
	filename: NodeT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__NODET_H__
#define __HEADER_GUARD_TREECONTAINER__NODET_H__

#include <treecontainerlib/HelperVisitors.h>
#include <treecontainerlib/NodeVisitPolicies.h>

namespace treeContainer
{
	//! \tparam ValueType Typ rpzechowywanej warto�ci
	template<typename ValueType>
	class NodeT
	{
	public:
		//! Typ przechowywanych w w�z�ach warto�ci
		typedef ValueType value_type;
		//! Typ naszego w�z�a
		typedef NodeT<value_type> node_type;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);
		//! Dzieci wezla
		typedef Node::Nodes<NodePtr> Children;
		//! Typ ilo�ci
		typedef Node::SizeType size_type;

	private:

		//! \param other Węzeł przenoszony wraz z wartością
		NodeT(NodeT && other)
			: parent_(std::move(other.parent_)),
			children_(std::move(other.children_)),
			value_(std::move(other.value_))
		{}

		//! \param value Warto�c przechowywana w w�le (kopiowanie)
		NodeT(const value_type & value) : value_(value) {}
		//! \param value Warto�c przechowywana w w�le (move semantics)
		NodeT(value_type && value) : value_(std::move(value)) {}

		//! \param value Warto�c przechowywana w w�le (kopiowanie)
		NodeT(NodePtr parent, const value_type & value) : parent_(parent), value_(value) {}
		//! \param value Warto�c przechowywana w w�le (move semantics)
		NodeT(NodePtr parent, value_type && value) : parent_(parent), value_(std::move(value)) {}

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
			NodePtr ret(new NodeT(parentNode, value));
			parentNode->children_.push_back(ret);
			return ret;
		}

		//! \param parentNode W�ze� do kt�rego dodajemy warto�� dziecka
		//! \param value Warto�c dodawanego w�z�a
		//! \return W�ze� dziecko z zadan� warto�ci�
		static NodePtr add(NodePtr parentNode, value_type && value)
		{
			NodePtr ret(new NodeT(parentNode, std::move(value)));
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
				auto parentNode = nodeToReplace->parent();
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
					auto it = std::find(parentNode->children_.begin(), parentNode->children_.end(), node);
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
		~NodeT() {}

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
			return Node::isAncestor(this, node);
		}

		//! \param node Weze� kt�ry sprawdzamy czy jest naszym potomkiem
		//! \return Prawda je�li w�ze� jest naszym potomkiem (nasz w�ze� znajdziemy go id�c w g�re po parentach tego w�z�a)
		bool isDescendant(NodeConstPtr node) const
		{
			return Node::isDescendant(this, node);
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
			return 1 + Node::childrenCount(this);
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
		//Children children() { return children_; }

	private:

		//! Rodzic
		NodeWeakPtr parent_;
		//! Warto��
		value_type value_;
		//! Dzieci
		Children children_;
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__NODET_H__