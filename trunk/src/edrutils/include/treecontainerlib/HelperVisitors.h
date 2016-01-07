/********************************************************************
	created:  2015/06/08	19:26:33
	filename: TreeNodeHelperVisitors.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREENODEHELPERVISITORS_H__
#define __HEADER_GUARD_TREECONTAINER__TREENODEHELPERVISITORS_H__

#include <treecontainerlib/Node.h>

namespace treeContainer
{
	//! Typ wyliczeniowy opisujący kierunek linearyzacji drzewa
	enum VisitOrder {
		Forward, //! Zgodnie z kolejnością
		Backward //! W odwrotrnej kolejności
	};

	//! Klasa wizytująca, zliczająca ilość węzłów drzewa
	class SizeVisitor
	{
	public:

		//! Konstruktor domyslny
		SizeVisitor() : treeSize_(0) {}
		//! Destruktor
		~SizeVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł
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
		Node::SizeType treeSize() const
		{
			return treeSize_;
		}

	private:

		//! Ilość węzłów w drzewie
		Node::SizeType treeSize_;
	};

	//! Klasa wizytująca szukająca najgłebszego poziomu drzewa
	class FindMaxLevelVisitor
	{
	public:
		//! Konstruktor domyslny
		FindMaxLevelVisitor() : maxLevel_(0) {}
		//! Destruktor
		~FindMaxLevelVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \param level Aktualnie odwiedzany poziom (wzgledem węzła z którego startowaliśmy)
		void operator()(NPtr, const Node::SizeType level)
		{
			maxLevel_ = std::max(maxLevel_, level);
		}

		//! \return Najgłebszy poziom drzewa
		Node::SizeType maxLevel() const
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
		Node::SizeType maxLevel_;
	};

	//! \tparam NPtr Typ wskaźnika węzła
	template<typename NPtr, int VisitOrder>
	//! Klasa linearyzująca drzewo
	class LinearizeVisitor
	{
		static_assert(VisitOrder == Forward || VisitOrder == Backward, "Undefined order");

	public:

		using Nodes = Node::Nodes < NPtr > ;

	public:
		//! Konstruktor domyslny
		LinearizeVisitor() {}
		//! Destruktor
		~LinearizeVisitor() {}
		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename... Args>
		//! \param node odwiedzany węzeł
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

	//! \tparam NPtr Typ wskaźnika węzła
	template<typename NPtr>
	//! Klasa linearyzująca drzewo
	class LinearizeVisitor < NPtr, Backward >
	{
	public:

		using Nodes = Node::Nodes < NPtr > ;

	public:
		//! Konstruktor domyslny
		LinearizeVisitor() {}
		//! Destruktor
		~LinearizeVisitor() {}
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename... Args>
		//! \param node odwiedzany węzeł
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

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa pozwalająca przechodzić wszystkie węzły drzewa
	class ConsumingVisitor
	{
	public:
		//! \param visitor Odwiedzający
		ConsumingVisitor(Visitor & visitor) : visitor(visitor) {}
		//! Destruktor
		~ConsumingVisitor() {}

		//! Operator "konsumuje" ewentualne nadmiarowe argumenty zostawiając tylko węzeł
		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł
		void operator() (NPtr node, Args...)
		{
			visitor(node);
		}

	private:
		//! Odwiedzający
		Visitor & visitor;
	};

	//! \tparam FilterType Typ filtra dla węzłów
	//! \tparam Visitor Typ odwiedzającego przefiltrowane węzły
	template<typename FilterType, typename Visitor>
	//! Klasa realizująca odwiedzanie przefiltrowanych węzłów
	class VisitorFilter
	{
	public:
		//! \param filter Filtr dla węzłów
		//! \param visitor Odwiedzający przefiltrowane węzły
		VisitorFilter(Visitor & visitor, FilterType filter = FilterType())
			: visitor(visitor), filter(filter){}
		//! Destruktor
		~VisitorFilter() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł
		void operator()(NPtr node, Args... args)
		{
			if (filter(node) == true){
				visitor(node, args...);
			}				
		}

	private:
		//! Filtr
		FilterType filter;
		//! Odwiedzający
		Visitor & visitor;
	};

	struct NonLeafFilter
	{
		//! \tparam NPtr Typ wskaźnika węzła		
		template <typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \return Czy węzeł jest liściem
		inline bool operator()(NPtr node) const
		{
			return node->isLeaf() == false;
		}
	};

	struct LeafFilter
	{
		//! \tparam NPtr Typ wskaźnika węzła		
		template <typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \return Czy węzeł nie jest liściem
		inline bool operator()(NPtr node) const
		{
			return node->isLeaf() == true;
		}
	};

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa realizująca filtracje nie-liści w drzewie, pozwala przechodzić wszystkie pozostałe węzły
	using NonLeafVisitor = VisitorFilter < NonLeafFilter, Visitor > ;	

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa realizująca filtracje nie-liści w drzewie, pozwala przechodzić wszystkie pozostałe węzły
	using LeafVisitor = VisitorFilter < LeafFilter, Visitor > ;

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa wizutująca drzewo i indeksująca węzły
	class IndexingVisitor
	{
	public:
		//! \param visitor Odwiedzający drzewo
		IndexingVisitor(Visitor & visitor) : idx(0), visitor(visitor) {}
		//! Destruktor
		~IndexingVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł		
		void operator() (NPtr node, Args...)
		{
			visitor(node, idx++);
		}

	private:
		//! Aktualny index węzła
		Node::SizeType idx;
		//! Odwiedzający
		Visitor & visitor;
	};

	//! Struktura opisuje minimalny i maksymalny stopień węzłów drzewa
	struct DegreeLimits
	{
		//! Domyślny konstruktor
		DegreeLimits() : min(std::numeric_limits<Node::SizeType>::max()),
			max(std::numeric_limits<Node::SizeType>::min()) {}

		//! \param other Kopiowany obiekt
		DegreeLimits(const DegreeLimits & other) : min(other.min), max(other.max) {}
		//! Destruktor
		~DegreeLimits() {}

		//! Minimalny stopień drzewa
		Node::SizeType min;
		//! Maksymalny stopień drzewa
		Node::SizeType max;
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

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł
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
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREENODEHELPERVISITORS_H__
