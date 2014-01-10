/********************************************************************
    created:  2013/11/18
    created:  18:11:2013   12:01
    filename: StreamData.h
    author:   Mateusz Janiak
    
    purpose:  Baza dla danych strumieniowych
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__STREAMDATABASE_H__
#define HEADER_GUARD_UTILS__STREAMDATABASE_H__

#include <utils/SmartPtr.h>
#include <boost/type_traits.hpp>
#include <threading/SynchronizationPolicies.h>
#include <utils/Debug.h>
#include <list>

namespace utils {

	//! Interfejs obiektu obserwującego strumień
	//! Implementacja musi gwarantować bezpieczne operacje wielowątkowe
	class IStreamStatusObserver
	{
	public:
		virtual ~IStreamStatusObserver() {}
		//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
		virtual void update() = 0;
		//! \return Czy stan strumienia się zmienił
		virtual const bool modified() const = 0;
	};

	//! Klasa obsługująca odpytywanie statusu strumienia
	//! (czy dane uległy aktualizacji)
	class StreamStatusObserver : public IStreamStatusObserver
	{
	private:

		//! Wewętrzna implementacja
		class StreamStatusImpl;
	public:
		//! Domyslny konstruktor
		StreamStatusObserver();
		//! Destruktor wirtualny
		~StreamStatusObserver();
		//! \return Czy strumień uległ zmianie
		virtual const bool modified() const;
		//! \param sb Strumień aktualizujący mój stan
		virtual void update();

	private:
		//! Implementacja
		utils::shared_ptr<StreamStatusImpl> impl;
	};

	typedef utils::shared_ptr<StreamStatusObserver> StreamStatusObserverPtr;

	//! Klasa bazowa strumienia
	class StreamBase
	{
	protected:
		//! Chroniony konstruktor domyślny
		StreamBase() {}

		//! Pomiadamia obserwatorów o zmianie
		void notify()
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			for(auto it = observers.begin(); it != observers.end(); ++it){
				(*it)->update();
			}
		}

	public:
		//! Destruktor wirtualny
		virtual ~StreamBase() {}

		//! \param observer Dołączany obiekt obserwujący strumień
		void attachObserver(StreamStatusObserverPtr observer)
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			if(observer == nullptr){
				throw std::invalid_argument("Empty observer");
			}

			if(std::find(observers.begin(), observers.end(), observer) == observers.end()){
				observers.push_back(observer);
			}
		}

		//! \param observer Odłączany obiekt obserwujący strumień
		void detachObserver(StreamStatusObserverPtr observer)
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			observers.remove(observer);			
		}

	protected:
		//! Obiekt synchronizujący
		mutable utils::RecursiveSyncPolicy synch_;

	private:
		//! Lista obserwatorów
		std::list<StreamStatusObserverPtr> observers;
	};


	//! Wzorzec klasy obsługującej bufor danych dla strumieni
	//! \tparam T Typ danych strumienia i bufora
	template<typename T>
	class StreamBufferT
	{
	public:
		//! Typ rozmiaru bufora
		typedef unsigned int size_type;

		//! Typ referencji do danych
		typedef typename boost::add_reference<T>::type ref_type;

		//! Typ stałej referencji do danych
		typedef typename boost::add_const<ref_type>::type const_ref_type;

		//! Typ kolekcji danych bufora
		typedef std::list<T> ListT;

	public:
		//! Konstruktor domyslny
		//! \param size Rozmiar bufora cyklicznego - domyślnie 50, 0 oznacza brak ograniczeń na bufor, 1 - wartość niedozwolona
		StreamBufferT(const size_type size = 50) : bufferSize_(size) {}
		//! Destruktor wirtualny
		~StreamBufferT() {}
		//! \param size Maksymalny rozmiar bufora - 0 oznacza brak ograniczeń, 1 - wartość niedozwolona
		//! (implementacja listy)
		void setBufferSize(const size_type size)
		{
			if(size == 1){
				throw std::runtime_error("Wrong buffer size");
			}

			utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
			bufferSize_ = size;

			if(bufferSize_ > 0){
				while(bufferData.size() > bufferSize_){
					bufferData.pop_front();
				}
			}
		}

		//! \return Maksymalny rozmiar bufora
		const size_type bufferSize() const
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
			return bufferSize_;
		}

		//! \return Aktualny rozmiar bufora
		const size_type size() const
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
			return bufferData.size();
		}

		//! \param data Dane do włożenia do bufora
		void pushData(const_ref_type data)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
			bufferData.push_back(data);

			if(bufferSize_ > 0 && bufferData.size() > bufferSize_){
				bufferData.pop_front();
			}
		}

		//! \param data [out] Obiekt docelowy dla danych bufora
		void data(ListT & data)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(synch_);
			data.insert(bufferData.begin(), bufferData.end());
			data.swap(ListT());
		}

	private:
		//! Obiekt synchronizujący
		utils::StrictSyncPolicy synch_;
		//! Bufor danych
		ListT bufferData;
		//! Maksymalny rozmiar bufora
		size_type bufferSize_;
	};


	//! Generyczny obiekt strumienia, synchronizuje dane
	//! \tparam T Typ danych w strumieniu
	template<typename T>
	class StreamT : public StreamBase
	{
	public:

		//! Typ bufora
		typedef StreamBufferT<T> StreamBufferType;

		//! Typ referencji do danych
		typedef typename StreamBufferType::ref_type ref_type;
		
		//! Typ stałej referencji do danych
		typedef typename StreamBufferType::const_ref_type const_ref_type;

		//! Typ wskaźnika na bufor
		typedef utils::shared_ptr<StreamBufferType> StreamBufferPtr;

		//! Typ listy buforów
		typedef std::list<StreamBufferPtr> StreamBuffersList;

	public:
		//! Domyślny konstruktor
		StreamT() {}

		//! \param data Data received from the stream
		void pushData(const_ref_type data)
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			
			UTILS_ASSERT(streamBufferList.empty() == true, "Stream data with no buffers");

			for(auto it = streamBufferList.begin();
				it != streamBufferList.end(); ++it){

				(*it)->pushData(data);
			}

			data_ = data;

			notify();
		}

		//! Destruktor wirtualny
		virtual ~StreamT() {}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia
		void data(ref_type d) const
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			d = data_;
		}

		//! \param bufferPtr Nowy bufor danych aktualizowany danymi strumienia
		void attachBuffer(StreamBufferPtr bufferPtr)
		{
			if(bufferPtr == nullptr){
				throw std::runtime_error("Uninitialized buffer pointer");
			}

			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			streamBufferList.remove(bufferPtr);
			streamBufferList.push_back(bufferPtr);
		}

		//! \param bufferPtr Odłanczany bufor danych
		void detachBuffer(StreamBufferPtr bufferPtr)
		{
			utils::ScopedLock<utils::RecursiveSyncPolicy> lock(synch_);
			streamBufferList.remove(bufferPtr);			
		}

	private:
		//! Lista buforów
		StreamBuffersList streamBufferList;
		//! Aktualne dane
		T data_;
	};
}

#endif	//	HEADER_GUARD_UTILS__STREAMDATABASE_H__
