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
#include <utils/TreeNodeHelperVisitors.h>
#include <utils/TreeNode.h>
#include <utils/Tree.h>

namespace utils {
struct LinearizedTree {
	//! Typ indeksu w�z�a
	using NodeIDX = TreeNode::SizeType;

	template<typename T>
	//! Typ bimapy indeks�w w�z��w do zadanej wartosci
	using Mapping = boost::bimap < NodeIDX, T >;

	template<typename T>
	//! Typ porzadkujacy po indeksach
	using Order = std::vector < T >;

	template<typename T>
	//! \param order Porz�dek
	//! \return Mapowanie wynikajace z porzadku
	static Mapping<T> convert(const Order<T> & order) {
		Mapping<T> ret;

		for (typename Order<T>::size_type i = 0; i < order.size(); ++i) {
			ret.insert(ret.end(), { i, order[i] });
		}
		return ret;
	}
};

template<typename TreeVisitOrder>
struct LinearizedTreeT: public LinearizedTree {
	using VisitOrder = TreeVisitOrder;

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void visit(const TreeType & tree, Visitor visitor = Visitor()) {
		ConsumingVisitor<Visitor> cv(visitor);
		VisitOrder::visit(tree, cv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void visit(TreeType & tree, Visitor visitor = Visitor()) {
		ConsumingVisitor<Visitor> cv(visitor);
		VisitOrder::visit(tree, cv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void globalIndexedVisit(const TreeType & tree, Visitor visitor =
			Visitor()) {
		visit(tree, IndexingVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y
	static void globalIndexedVisit(TreeType & tree, Visitor visitor =
			Visitor()) {
		visit(tree, IndexingVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void visitNonLeaf(const TreeType & tree, Visitor visitor =
			Visitor()) {
		NonLeafVisitor<Visitor> nlv(visitor);
		visit(tree, nlv);
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void visitNonLeaf(TreeType & tree, Visitor visitor = Visitor()) {
		visit(tree, NonLeafVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void localIndexedVisitNonLeaf(const TreeType & tree,
			Visitor visitor = Visitor()) {
		visit(tree,
				NonLeafVisitor<IndexingVisitor<Visitor>>(
						IndexingVisitor<Visitor>(visitor)));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void localIndexedVisitNonLeaf(TreeType & tree, Visitor visitor =
			Visitor()) {
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
			Visitor visitor = Visitor()) {
		visit(tree,
				IndexingVisitor<NonLeafVisitor<Visitor>>(
						NonLeafVisitor<Visitor>(visitor)));
	}

	//! \tparam NPtr Typ wskaxnika w�z�a drzewa z kt�rego zaczynamy
	//! \tparam Visitor Typ odwiedzaj�cego
	template<typename TreeType, typename Visitor>
	//! \param node W�ze� od kt�rego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzaj�cy w�z�y nie li�cie
	static void globalIndexedVisitNonLeaf(TreeType & tree, Visitor visitor =
			Visitor()) {
		visit(tree,
				IndexingVisitor<NonLeafVisitor<Visitor>>(
						NonLeafVisitor<Visitor>(visitor)));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static auto createCompleteOrder(
			const TreeType & tree) -> Order<decltype(tree.root())> {
		TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

		visit(tree, lv);

		return Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Order<
			decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createCompleteOrderKey(
			const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor()) {
		Order<
				decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

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
	static auto createCompleteOrder(
			TreeType & tree) -> Order<decltype(tree.root())> {
		TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

		visit(tree, lv);

		return Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static auto createCompleteMapping(
			const TreeType & tree) -> Mapping<decltype(tree.root())> {
		Mapping<decltype(tree.root())> ret;

		globalIndexedVisit(tree,
				[&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static Mapping<
			decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createCompleteMappingKey(
			const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor()) {
		Mapping<
				decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

		globalIndexedVisit(tree,
				[&ret, &keyExtractor](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, keyExtractor(node->value())});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie wszystkich joint�w
	//! \return Mapowanie joint�w szkieletu
	static auto createCompleteMapping(
			TreeType & tree) -> Mapping<decltype(tree.root())> {
		Mapping<decltype(tree.root())> ret;

		globalIndexedVisit(tree,
				[&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static auto createNonLeafOrder(
			const TreeType & tree) -> Order<decltype(tree.root())> {
		TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

		visitNonLeaf(tree, lv);

		return Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Order<
			decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createNonLeafOrderKey(
			const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor()) {
		Order<
				decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

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
	static auto createNonLeafOrder(
			TreeType & tree) -> Order<decltype(tree.root())> {
		TreeNode::LinearizeVisitor<decltype(tree.root()), TreeNode::Forward> lv;

		visitNonLeaf(tree, lv);

		return Order<decltype(tree.root())>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static auto createNonLeafMapping(
			const TreeType & tree) -> Order<decltype(tree.root())> {
		Mapping<decltype(tree.root())> ret;

		globalIndexedVisitNonLeaf(tree,
				[&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static Mapping<
			decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> createNonLeafMappingKey(
			const TreeType & tree, KeyExtractor keyExtractor = KeyExtractor()) {
		Mapping<
				decltype(std::declval<KeyExtractor>()(std::declval<const TreeType>().root()->value()))> ret;

		globalIndexedVisitNonLeaf(tree,
				[&ret, &keyExtractor](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, keyExtractor(node->value())});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla kt�rego generujemy mapowanie aktywnych joint�w (maj�cych dzieci)
	//! \return Mapowanie aktywnych joint�w szkieletu
	static auto createNonLeafMapping(
			TreeType & tree) -> Mapping<decltype(tree.root())> {
		Mapping<decltype(tree.root())> ret;

		globalIndexedVisitNonLeaf(tree,
				[&ret](decltype(tree.root()) node, const LinearizedTree::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}
};
}

#endif	// __HEADER_GUARD_UTILS__LINEARIZEDTREE_H__
