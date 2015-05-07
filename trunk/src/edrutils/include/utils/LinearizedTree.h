/********************************************************************
	created:  2015/05/06	15:04:24
	filename: LinearizedTree.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__LINEARIZEDTREE_H__
#define __HEADER_GUARD_UTILS__LINEARIZEDTREE_H__

#include <vector>
#include <boost/bimap.hpp>
#include <utils/TreeNode.h>
#include <utils/Tree.h>

namespace utils
{
	//! \tparam Visitor Typ odzwiedzaj�cego
	template<typename Visitor>
	//! Klasa pozwalaj�ca przechodzi� wszystkie w�z�y drzewa
	class ConsumingVisitor
	{
	public:
		ConsumingVisitor(Visitor & visitor) : visitor(visitor) {}
		~ConsumingVisitor() {}

		//! Operator "konsumuje" ewentualne nadmiarowe argumenty zostawiaj�c tylko w�ze�
		template<typename NPtr, typename... Args>
		void operator() (NPtr node, Args...)
		{
			visitor(node);
		}

	private:
		Visitor & visitor;
	};

	//! \tparam Visitor Typ odzwiedzaj�cego
	template<typename Visitor>
	//! Klasa realizuj�ca filtracje nie-li�ci w drzewie, pozwala przechodzi� wszystkie pozosta�e w�z�y
	class NonLeafVisitor
	{
	public:
		NonLeafVisitor(Visitor & visitor) : visitor(visitor) {}
		~NonLeafVisitor() {}

		template<typename NPtr, typename... Args>
		void operator() (NPtr node, Args... args)
		{
			if (node->isLeaf() == false){
				visitor(node, args...);
			}
		}

	private:
		Visitor & visitor;
	};

	//! \tparam Visitor Typ odzwiedzaj�cego
	template<typename Visitor>
	//! Klasa wizutuj�ca drzewo i indeksuj�ca w�z�y
	class IndexingVisitor
	{
	public:
		//! \param visitor Odwiedzaj�cy drzewo
		IndexingVisitor(Visitor & visitor) : idx(0), visitor(visitor) {}
		//! Destruktor
		~IndexingVisitor() {}

		template<typename NPtr, typename... Args>
		void operator() (NPtr node, Args...)
		{
			visitor(node, idx++);
		}

	private:
		TreeNode::SizeType idx;
		Visitor & visitor;
	};

	struct LinearizedTree
	{
		//! Typ indeksu w�z�a
		using NodeIDX = TreeNode::SizeType;

		template<typename T>
		//! Typ bimapy indeks�w w�z��w do zadanej wartosci
		using Mapping = boost::bimap < NodeIDX, T > ;

		template<typename T>
		//! Typ porzadkujacy po indeksach
		using Order = std::vector < T > ;

		template<typename T>
		//! \param order Porz�dek
		//! \return Mapowanie wynikajace z porzadku
		static Mapping<T> convert(const Order<T> & order)
		{
			Mapping<T> ret;
			for (Order<T>::size_type i = 0; i < order.size(); ++i)
			{
				ret.insert(ret.end(), { i, order[i] });
			}
			return ret;
		}
	};

	template<typename TreeVisitOrder>
	struct LinearizedTreeT : public LinearizedTree
	{
		using VisitOrder = TreeVisitOrder;

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y
		static void visit(const TreeType & tree, Visitor visitor = Visitor())
		{
			TreeVisitOrder::visit(tree, ConsumingVisitor<Visitor>(visitor));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y
		static void visit(TreeType & tree, Visitor visitor = Visitor())
		{
			TreeVisitOrder::visit(tree, ConsumingVisitor<Visitor>(visitor));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y
		static void globalIndexedVisit(const TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, IndexingVisitor<Visitor>(visitor));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y
		static void globalIndexedVisit(TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, IndexingVisitor<Visitor>(visitor));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
		static void visitNonLeaf(const TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, NonLeafVisitor<Visitor>(visitor));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
		static void visitNonLeaf(TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, NonLeafVisitor<Visitor>(visitor));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
		static void localIndexedVisitNonLeaf(const TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, NonLeafVisitor<IndexingVisitor<Visitor >>(IndexingVisitor<Visitor>(visitor)));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
		static void localIndexedVisitNonLeaf(TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, NonLeafVisitor<IndexingVisitor<Visitor >>(IndexingVisitor<Visitor>(visitor)));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
		static void globalIndexedVisitNonLeaf(const TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, IndexingVisitor<NonLeafVisitor<Visitor >>(NonLeafVisitor<Visitor>(visitor)));
		}

		//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
		//! \tparam Visitor Typ odwiedzaj�cego
		template<typename TreeType, typename Visitor>
		//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
		//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
		static void globalIndexedVisitNonLeaf(TreeType & tree, Visitor visitor = Visitor())
		{
			visit(tree, IndexingVisitor<NonLeafVisitor<Visitor >>(NonLeafVisitor<Visitor>(visitor)));
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
		//! \return Mapowanie joint�w szkieletu
		static Order<decltype(tree.root())> createCompleteOrder(const TreeType & tree)
		{
			TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

			visit(tree, lv);

			return Order<decltype(tree.root())>(lv.linearizedNode().begin(), lv.linearizedNode().end());
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType, typename KeyExtractor>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
		//! \return Mapowanie joint�w szkieletu
		static Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createCompleteOrderKey(const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor())
		{
			Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

			visit(tree, [&ret, &keyExtractor](decltype(tree.root()) node)
			{
				ret.push_back(keyExtractor(node->value()));
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
		//! \return Mapowanie joint�w szkieletu
		static Order<decltype(tree.root())> createCompleteOrder(TreeType & tree)
		{
			TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

			visit(tree, lv);

			return Order<decltype(tree.root())>(lv.linearizedNode().begin(), lv.linearizedNode().end());
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
		//! \return Mapowanie joint�w szkieletu
		static Mapping<decltype(tree.root())> createCompleteMapping(const TreeType & tree)
		{
			Mapping<decltype(tree.root())> ret;

			globalIndexedVisit(tree, [&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
			{
				ret.left.insert(ret.left.end(), { idx, node });
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType, typename KeyExtractor>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
		//! \return Mapowanie joint�w szkieletu
		static Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createCompleteMappingKey(const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor())
		{
			Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

			globalIndexedVisit(tree, [&ret, &keyExtractor](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
			{
				ret.left.insert(ret.left.end(), { idx, keyExtractor(node->value()) });
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
		//! \return Mapowanie joint�w szkieletu
		static Mapping<decltype(tree.root())> createCompleteMapping(TreeType & tree)
		{
			Mapping<decltype(tree.root())> ret;

			globalIndexedVisit(tree, [&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
			{
				ret.left.insert(ret.left.end(), { idx, node });
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
		//! \return Mapowanie aktywnych joint�w szkieletu
		static Order<decltype(tree.root())> createNonLeafOrder(const TreeType & tree)
		{
			TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

			visitNonLeaf(tree, lv);

			return Order<decltype(tree.root())>(lv.linearizedNode().begin(), lv.linearizedNode().end());
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType, typename KeyExtractor>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
		//! \return Mapowanie aktywnych joint�w szkieletu
		static Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createNonLeafOrderKey(const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor())
		{
			Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

			visitNonLeaf(tree, [&ret, &keyExtractor](decltype(tree.root()) node)
			{
				ret.push_back(keyExtractor(node->value()));
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
		//! \return Mapowanie aktywnych joint�w szkieletu
		static Order<decltype(tree.root())> createNonLeafOrder(TreeType & tree)
		{
			TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

			visitNonLeaf(tree, lv);

			return Order<decltype(tree.root())>(lv.linearizedNode().begin(), lv.linearizedNode().end());
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
		//! \return Mapowanie aktywnych joint�w szkieletu
		static Mapping<decltype(tree.root())> createNonLeafMapping(const TreeType & tree)
		{
			Mapping<decltype(tree.root())> ret;

			globalIndexedVisitNonLeaf(tree, [&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
			{
				ret.left.insert(ret.left.end(), { idx, node });
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType, typename KeyExtractor>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
		//! \return Mapowanie aktywnych joint�w szkieletu
		static Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createNonLeafMappingKey(const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor())
		{
			Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

			globalIndexedVisitNonLeaf(tree,[&ret, &keyExtractor](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
			{
				ret.left.insert(ret.left.end(), { idx, keyExtractor(node->value()) });
			});

			return ret;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
		//! \return Mapowanie aktywnych joint�w szkieletu
		static Mapping<decltype(tree.root())> createNonLeafMapping(TreeType & tree)
		{
			Mapping<decltype(tree.root())> ret;

			globalIndexedVisitNonLeaf(tree, [&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
			{
				ret.left.insert(ret.left.end(), { idx, node });
			});

			return ret;
		}
	};
}

#endif	// __HEADER_GUARD_UTILS__LINEARIZEDTREE_H__