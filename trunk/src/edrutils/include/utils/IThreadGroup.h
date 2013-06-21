/********************************************************************
    created:  2013/03/26
    created:  26:3:2013   12:31
    filename: IThreadGroup.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITHREADGROUP_H__
#define HEADER_GUARD___ITHREADGROUP_H__

#include <vector>
#include <utils/IThread.h>

namespace utils {

	//! Interfejs klasy obs�uguj�cej w�tki
	class IThreadGroup
	{
	public:
		//! Struktura opisuje wątek wraz z funkcją jaką ma realizować
		struct ThreadData{
			//! Wątek który będziemy uruchamiać
			IThreadPtr thread;
			//! Zadanie które będzie realizował wątek
			RunnablePtr runnable;
			//! Priorytet z jakim wątek będzie realizował swoje zadanie
			IThread::Priority priority;
		};

		//! Polityka uruchamiania i ewentualnego kończenia działania grupy
		enum StartPolicy {
			ALL_OR_NONE,	//! Błąd jednego wątku ciagnie za sobą anulowanie pozostałych
			ANY				//! Błąd jednego wątku nie wpływa na przerwanie pracy pozostałych
		};

		//! Typ agregujący wątki do kontrolowania
		typedef std::vector<ThreadData> Threads;		
		//! Typ opisujący rozmiar grupy wątków
		typedef Threads::size_type size_type;

	public:
		//! Wirtualny destruktor
		virtual ~IThreadGroup() {}

		//! Metoda uruchamia przetwarzanie przez grup�
		//! \param threads Wątki obsługiwane przez grupę
		//! \param finishPolicy Polityka kończenia pracy wątków w grupie
		virtual void start(const Threads & threads,
			const StartPolicy startPolicy = ALL_OR_NONE) = 0;
		//! \return Aktualna polityka kończenia grupy wątków
		virtual const StartPolicy startPolicy() const = 0;
		//! \return Czy grupa działa
		virtual const bool running() const = 0;
		//! \return Rozmiar grupy
		virtual const size_type size() const = 0;
		//! \param idx Indeks w�tku
		//! \return W�tek pod zadanym indeksem
		virtual IThreadConstPtr thread(const size_type idx) const = 0;
	};

	typedef boost::shared_ptr<IThreadGroup> ThreadGroupPtr;
	typedef boost::shared_ptr<const IThreadGroup> ThreadGroupConstPtr;
	typedef boost::weak_ptr<IThreadGroup> ThreadGroupWPtr;
	typedef boost::weak_ptr<const IThreadGroup> ThreadGroupWConstPtr;

}

#endif	//	HEADER_GUARD___ITHREADGROUP_H__
