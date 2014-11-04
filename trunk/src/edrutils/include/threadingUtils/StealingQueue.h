/********************************************************************
	created:  2014/10/06	19:10:48
	filename: StealingQueue.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__STEALINGQUEUE_H__
#define __HEADER_GUARD_THREADINGUTILS__STEALINGQUEUE_H__

#include <deque>
#include <mutex>

namespace threadingUtils
{
	//! Klasa realizuj�ca kolejk� danych umo�liwiaj�c �ci�gni�cie danych z jej ko�ca (kradzie�)
	//! \tparam T Typ danych przechowywanych w kolejce
	template<typename T>
	class StealingQueue
	{
	private:
		//! Kolejka danych
		std::deque<T> queue;
		//! Obiekt synchronizuj�cy
		mutable std::mutex mutex;

	public:

		//! Typ opisuj�cy rozmiar kolejki
		typedef typename std::deque<T>::size_type size_type;

	public:
		//! Konstruktor domy�lny
		StealingQueue() = default;
		//! Destruktor
		~StealingQueue() = default;

		StealingQueue(const StealingQueue& other) = delete;
		StealingQueue& operator=(const StealingQueue& other) = delete;

		//! \param data Dane wrzucane do kolejki
		void push(T data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			queue.push_front(std::move(data));
		}		

		//! \return Czy kolejka jest pusta
		const bool empty() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return queue.empty();
		}

		//! \return Czy kolejka jest pusta
		const size_type size() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return queue.size();
		}

		//! \param data [out] Obiekt w kt�rym zapiszemy warto�� �ci�gni�t� z kolejki
		//! \return Czy uda�o si� �ci�gn�� dane z kolejki
		const bool tryPop(T & data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (queue.empty())
			{
				return false;
			}

			data = std::move(queue.front());
			queue.pop_front();
			return true;
		}

		//! \param data [out] Obiekt w kt�rym zapiszemy warto�� �ci�gni�t� z ko�ca kolejki (kradzie�)
		//! \return Czy uda�o si� �ci�gn�� dane z ko�ca kolejki
		const bool trySteal(T & data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (queue.empty())
			{
				return false;
			}

			data = std::move(queue.back());
			queue.pop_back();
			return true;
		}

		//! Metoda czy�ci kolejk�
		void clear()
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::deque<T>().swap(queue);
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__STEALINGQUEUE_H__