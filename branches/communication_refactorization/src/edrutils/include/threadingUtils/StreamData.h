/********************************************************************
	created:  2013/11/18
	created:  18:11:2013   12:01
	filename: StreamData.h
	author:   Mateusz Janiak

	purpose:  Baza dla danych strumieniowych
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__
#define HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__

#include <threadingUtils/Export.h>
#include <utils/SmartPtr.h>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include <threadingUtils/SynchronizationPolicies.h>
#include <utils/Debug.h>
#include <list>

namespace threadingUtils {
	//! Interfejs obiektu obserwującego strumień
	//! Implementacja musi gwarantować bezpieczne operacje wielowątkowe
	class IStreamStatusObserver
	{
	public:
		virtual ~IStreamStatusObserver() {}
		//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
		virtual void update() = 0;
	};

	//! Klasa obsługująca odpytywanie statusu strumienia
	//! (czy dane uległy aktualizacji) i resetująca status po sprawdzeniu
	class THREADINGUTILS_EXPORT ResetableStreamStatusObserver : public IStreamStatusObserver
	{
	private:

		//! Wewętrzna implementacja
		class ResetableStreamStatusImpl;
	public:
		//! Domyslny konstruktor
		ResetableStreamStatusObserver();
		//! Destruktor wirtualny
		~ResetableStreamStatusObserver();
		//! \return Czy strumień uległ zmianie
		const bool modified() const;
		//! \param sb Strumień aktualizujący mój stan
		virtual void update();

	private:
		//! Implementacja
		utils::shared_ptr<ResetableStreamStatusImpl> impl;
	};

	typedef utils::shared_ptr<IStreamStatusObserver> StreamStatusObserverPtr;

	//! Klasa bazowa strumienia
	class THREADINGUTILS_EXPORT StreamBase
	{
	protected:
		//! Chroniony konstruktor domyślny
		StreamBase();

		//! Pomiadamia obserwatorów o zmianie
		void notify();

	public:
		//! Destruktor wirtualny
		virtual ~StreamBase();

		//! \param observer Dołączany obiekt obserwujący strumień
		void attachObserver(StreamStatusObserverPtr observer);

		//! \param observer Odłączany obiekt obserwujący strumień
		void detachObserver(StreamStatusObserverPtr observer);

	protected:
		//! Obiekt synchronizujący
		mutable RecursiveSyncPolicy synch_;

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
		StreamBufferT(const size_type size) : bufferSize_(size) {}
		//! Destruktor wirtualny
		~StreamBufferT() {}
		//! \param size Maksymalny rozmiar bufora - 0 oznacza brak ograniczeń
		//! (implementacja listy)
		void setMaxBufferSize(const size_type size)
		{
			ScopedLock<StrictSyncPolicy> lock(synch_);
			bufferSize_ = size;

			if (verifyBufferSize() == false){
				auto diff = bufferData.size() - bufferSize_;
				auto it = bufferData.begin();
				std::advance(it, diff);
				bufferData.erase(bufferData.begin(), it);
			}
		}

		//! \return Maksymalny rozmiar bufora
		const size_type maxBufferSize() const
		{
			ScopedLock<StrictSyncPolicy> lock(synch_);
			return bufferSize_;
		}

		//! \return Aktualny rozmiar bufora
		const size_type size() const
		{
			ScopedLock<StrictSyncPolicy> lock(synch_);
			return bufferData.size();
		}

		//! \param data Dane do włożenia do bufora
		void pushData(const_ref_type data)
		{
			ScopedLock<StrictSyncPolicy> lock(synch_);
			bufferData.push_back(data);

			if (verifyBufferSize() == false){
				bufferData.pop_front();
			}
		}

		//! \param data [out] Obiekt docelowy dla danych bufora
		void data(ListT & data)
		{
			ScopedLock<StrictSyncPolicy> lock(synch_);
			data.insert(bufferData.begin(), bufferData.end());
			ListT().swap(data);
		}

	private:

		//! \return Czy bufor ma poprawny rozmiar względem zadeklarowanej wielkości
		inline const bool verifyBufferSize() const
		{
			return !(bufferSize_ > 0 && bufferData.size() > bufferSize_);
		}

	private:
		//! Obiekt synchronizujący
		StrictSyncPolicy synch_;
		//! Bufor danych
		ListT bufferData;
		//! Maksymalny rozmiar bufora
		volatile size_type bufferSize_;
	};

	template<typename T>
	class IStreamT : public StreamBase
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

	private:

		//! Typ listy buforów
		typedef std::list<StreamBufferPtr> StreamBuffersList;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamT() {}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia
		virtual void data(ref_type d) const = 0;

		//! \param bufferPtr Nowy bufor danych aktualizowany danymi strumienia
		void attachBuffer(StreamBufferPtr bufferPtr)
		{
			if (bufferPtr == nullptr){
				throw std::runtime_error("Uninitialized buffer pointer");
			}

			ScopedLock<RecursiveSyncPolicy> lock(synch_);
			streamBufferList.remove(bufferPtr);
			streamBufferList.push_back(bufferPtr);
		}

		//! \param bufferPtr Odłanczany bufor danych
		void detachBuffer(StreamBufferPtr bufferPtr)
		{
			ScopedLock<RecursiveSyncPolicy> lock(synch_);
			streamBufferList.remove(bufferPtr);
		}

	protected:

		//! Return Czy podłączono jakieś bufory
		const bool buffersAttached() const
		{
			ScopedLock<RecursiveSyncPolicy> lock(synch_);
			return !streamBufferList.empty();
		}

		//! \param data Data received from the stream
		void pushBufferData(const_ref_type data)
		{
			UTILS_ASSERT(streamBufferList.empty() == true, "Stream data with no buffers");

			for (auto it = streamBufferList.begin();
				it != streamBufferList.end(); ++it){
				(*it)->pushData(data);
			}
		}

	private:
		//! Lista buforów
		StreamBuffersList streamBufferList;
	};

	//! Generyczny obiekt strumienia, synchronizuje dane
	//! \tparam T Typ danych w strumieniu
	template<typename T>
	class StreamT : public IStreamT<T>
	{
	public:
		//! Domyślny konstruktor
		StreamT() {}

		//! \param data Data received from the stream
		void pushData(const_ref_type data)
		{
			ScopedLock<RecursiveSyncPolicy> lock(synch_);

			try{
				pushBufferData(data);
			}
			catch (...){
			}

			data_ = data;

			notify();
		}

		//! Destruktor wirtualny
		virtual ~StreamT() {}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia
		virtual void data(ref_type d) const
		{
			ScopedLock<RecursiveSyncPolicy> lock(synch_);
			d = data_;
		}

	private:
		//! Aktualne dane
		T data_;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename Base, typename Dest>
	class StreamAdapterT : public IStreamT<Dest>
	{
	public:

		//! Typ adaptera
		typedef StreamAdapterT<Base, Dest> StreamAdapterType;

		//! Typ strumienia bazowego
		typedef IStreamT<Base> BaseStreamType;

		//! Wskaźnik na strumień bazowy
		typedef utils::shared_ptr<BaseStreamType> BaseStreamTypePtr;

		//! Typ funktora odpowiedzialnego za wypakowanie danych
		typedef boost::function<void(const Base &, Dest &)> ExtractorFunction;

	private:

		//! Klasa obserwujaca strumien bazowy
		class BaseStreamUpdater : public IStreamStatusObserver
		{
		public:
			//! \param destStream Strumien docelowy
			BaseStreamUpdater(StreamAdapterType * destStream)
				: destStream(destStream)
			{
			}

			//! Lapie zmiane w strumieniu zrodlowym i propaguje na nasz strumien
			virtual void update()
			{
				destStream->localUpdate();
			}

		private:
			//! Strumien docelowy
			StreamAdapterType * destStream;
		};

		friend class BaseStreamUpdater;

	public:
		//! \param baseStream Strumień którego reprezentację zmieniamy
		//! \param extractorFunction Funktor realizujące przepakowanie typów danych
		StreamAdapterT(BaseStreamTypePtr baseStream,
			const ExtractorFunction & extractorFunction)
			: baseStream_(baseStream), extractorFunction_(extractorFunction)
		{
			if (baseStream_ == nullptr){
				throw std::invalid_argument("Uninitialized base stream");
			}

			if (extractorFunction.empty() == true){
				throw std::invalid_argument("Empty extractor function");
			}

			sourceStreamObserver.reset(new BaseStreamUpdater(this));
			baseStream_->attachObserver(sourceStreamObserver);
		}

		//! Destruktor wirtualny
		virtual ~StreamAdapterT()
		{
			baseStream_->detachObserver(sourceStreamObserver);
		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po przepakowaniu
		virtual void data(ref_type d) const
		{
			Base bd;
			baseStream_->data(bd);
			extractorFunction_(bd, d);
		}

	private:

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		void localUpdate()
		{
			if (buffersAttached() == true){
				Dest d;
				data(d);
				pushBufferData(d);
			}

			notify();
		}

	private:
		//! Strumień który przykrywam
		BaseStreamTypePtr baseStream_;
		//! Funktor do wypakowywania danych
		ExtractorFunction extractorFunction_;
		//! Obserwator strumienia bazowego
		StreamStatusObserverPtr sourceStreamObserver;
	};
}

#endif	//	HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__
