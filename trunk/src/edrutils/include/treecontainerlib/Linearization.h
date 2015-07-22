/********************************************************************
 created:  2015/05/06	15:04:24
 filename: Linearization.h
 author:	  Mateusz Janiak

 purpose:
 *********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__LINEARIZEDTREE_H__
#define __HEADER_GUARD_TREECONTAINER__LINEARIZEDTREE_H__

#include <vector>
#include <boost/bimap.hpp>
#include <treecontainerlib/HelperVisitors.h>
#include <treecontainerlib/Tree.h>

namespace treeContainer {

struct Linearization
{
	//! Typ indeksu w�z�a
	using NodeIDX = Node::SizeType;

	template<typename T>
	//! Typ bimapy indeks�w w�z��w do zadanej wartosci
	using Mapping = boost::bimap < NodeIDX, T >;

	template<typename T>
	//! Typ porzadkujacy po indeksach
	using Order = std::vector < T >;

	template<typename T>
	//! \param order Porz�dek
	//! \return Mapowanie wynikajace z porzadku
	static Mapping<T> convert(const Order<T> & order)
	{
		Mapping<T> ret;

		for (typename Order<T>::size_type i = 0; i < order.size(); ++i) {
			ret.insert(ret.end(), { i, order[i] });
		}
		return ret;
	}
};

template<typename LinearizationVisitPolicy, int VisitOrder = Forward>
struct NodeLinearization
{
	//! \tparam NPtr Typ wska�nika w�z�a
	//! \tparam Visitor Typ odwiedzaj�cego w�z�y
	template<typename NPtr>
	//! \param node Węzeł od którego zaczynamy linearyzacje struktury
	//! \param lp Polityka linearyzowania drzewa
	//! \return Ścieżka zlinearyzowanego drzewa
	static Node::Nodes<NPtr> linearize(NPtr node)
	{
		LinearizeVisitor<NPtr, VisitOrder> tlv;
		LinearizationVisitPolicy::visit(node, tlv);
		return tlv.linearizedNode();
	}
};

template<typename TreeVisitOrder>
struct LinearizationT
{
	using VisitOrder = TreeVisitOrder;

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void visit(const TreeType & tree, Visitor visitor)
	{
		ConsumingVisitor<Visitor> cv(visitor);
		VisitOrder::visit(tree, cv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void visit(TreeType & tree, Visitor visitor)
	{
		ConsumingVisitor<Visitor> cv(visitor);
		VisitOrder::visit(tree, cv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void globalIndexedVisit(const TreeType & tree, Visitor visitor)
	{
		visit(tree, IndexingVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void globalIndexedVisit(TreeType & tree, Visitor visitor)
	{
		visit(tree, IndexingVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void visitNonLeaf(const TreeType & tree, Visitor visitor)
	{
		NonLeafVisitor<Visitor> nlv(visitor);
		visit(tree, nlv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void visitNonLeaf(TreeType & tree, Visitor visitor)
	{
		visit(tree, NonLeafVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void localIndexedVisitNonLeaf(const TreeType & tree,
			Visitor visitor)
	{
		visit(tree,	NonLeafVisitor<IndexingVisitor<Visitor>>(
			IndexingVisitor<Visitor>(visitor)));
	}	

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void localIndexedVisitNonLeaf(TreeType & tree, Visitor visitor)
	{
		IndexingVisitor<Visitor> iv(visitor);
		NonLeafVisitor<IndexingVisitor<Visitor>> nlv(iv);
		visit(tree, nlv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void globalIndexedVisitNonLeaf(const TreeType & tree,
			Visitor visitor)
	{
		visit(tree,	IndexingVisitor<NonLeafVisitor<Visitor>>(
			NonLeafVisitor<Visitor>(visitor)));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void globalIndexedVisitNonLeaf(TreeType & tree, Visitor visitor)
	{
		visit(tree, IndexingVisitor<NonLeafVisitor<Visitor>>(
			NonLeafVisitor<Visitor>(visitor)));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Linearization::Order<decltype(tree.root())>
		createCompleteOrder(const TreeType & tree) {
		Node::LinearizeVisitor<decltype(tree.root()), Forward> lv;

		visit(tree, lv);

		return Linearization::Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Linearization::Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))>
		createCompleteOrderKey(const TreeType & tree, KeyExtractor keyExtractor)
	{
		Linearization::Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

		auto vis = [&ret, &keyExtractor](decltype(tree.root()) node)
				{
					ret.push_back(keyExtractor(node->value()));
				};
		visit(tree, vis);

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Linearization::Order<decltype(tree.root())>
		createCompleteOrder(TreeType & tree)
	{
		Node::LinearizeVisitor<decltype(tree.root()), Forward> lv;

		visit(tree, lv);

		return Linearization::Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Linearization::Mapping<decltype(tree.root())>
		createCompleteMapping(const TreeType & tree)
	{
		Linearization::Mapping<decltype(tree.root())> ret;

		globalIndexedVisit(tree,
				[&ret](decltype(tree.root()) node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Linearization::Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))>
		createCompleteMappingKey(const TreeType & tree, KeyExtractor keyExtractor)
	{
		Linearization::Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

		globalIndexedVisit(tree,
				[&ret, &keyExtractor](decltype(tree.root()) node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, keyExtractor(node->value())});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Linearization::Mapping<decltype(tree.root())>
		createCompleteMapping(TreeType & tree)
	{
		Linearization::Mapping<decltype(tree.root())> ret;

		globalIndexedVisit(tree,
				[&ret](decltype(tree.root()) node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Linearization::Order<decltype(tree.root())>
		createNonLeafOrder(const TreeType & tree)
	{
		Node::LinearizeVisitor<decltype(tree.root()), Forward> lv;

		visitNonLeaf(tree, lv);

		return Linearization::Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Linearization::Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))>
		createNonLeafOrderKey(const TreeType & tree, KeyExtractor keyExtractor)
	{
		Linearization::Order<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

		auto vis = [&ret, &keyExtractor](decltype(tree.root()) node)
				{
					ret.push_back(keyExtractor(node->value()));
				};
		visitNonLeaf(tree, vis);

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Linearization::Order<decltype(tree.root())>
		createNonLeafOrder(TreeType & tree)
	{
		Node::LinearizeVisitor<decltype(tree.root()), Forward> lv;

		visitNonLeaf(tree, lv);

		return Linearization::Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Linearization::Order<decltype(tree.root())>
		createNonLeafMapping(const TreeType & tree)
	{
		Linearization::Mapping<decltype(tree.root())> ret;

		globalIndexedVisitNonLeaf(tree,
				[&ret](decltype(tree.root()) node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Linearization::Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))>
		createNonLeafMappingKey(const TreeType & tree, KeyExtractor keyExtractor)
	{
		Linearization::Mapping<decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

		globalIndexedVisitNonLeaf(tree,
				[&ret, &keyExtractor](decltype(tree.root()) node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, keyExtractor(node->value())});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Linearization::Mapping<decltype(tree.root())>
		createNonLeafMapping(TreeType & tree)
	{
		Linearization::Mapping<decltype(tree.root())> ret;

		globalIndexedVisitNonLeaf(tree,
				[&ret](decltype(tree.root()) node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}
};
}

#endif	// __HEADER_GUARD_TREECONTAINER__LINEARIZEDTREE_H__
