/********************************************************************
	created:  2015/06/08	19:26:33
	filename: TreeNodeHelperVisitors.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__TREENODEHELPERVISITORS_H__
#define __HEADER_GUARD_UTILS__TREENODEHELPERVISITORS_H__

namespace utils
{
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
		VisitorFilter(Visitor & visitor, FilterType  filter = FilterType())
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
		FilterType & filter;
		//! Odwiedzający
		Visitor & visitor;
	};

	struct NonLeafFilter
	{
		//! \tparam NPtr Typ wskaźnika węzła		
		template <typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \return Czy węzeł jest liściem
		bool operator()(NPtr node) const
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
		bool operator()(NPtr node) const
		{
			return node->isLeaf();
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
		TreeNode::SizeType idx;
		//! Odwiedzający
		Visitor & visitor;
	};
}

#endif	// __HEADER_GUARD_UTILS__TREENODEHELPERVISITORS_H__
