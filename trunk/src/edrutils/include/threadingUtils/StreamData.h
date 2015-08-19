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
#include <limits>
#include <type_traits>
#include <atomic>
#include <mutex>
#include <functional>
#include <utils/Debug.h>
#include <list>
#include <utility>

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
		virtual void update() override;

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
		mutable std::recursive_mutex synch_;

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

		//! Typ kolekcji danych bufora
		typedef std::list<T> ListT;

		//! Typ rozmiaru bufora
		typedef typename ListT::size_type size_type;

		//! Typ referencji do danych
		typedef typename std::add_lvalue_reference<T>::type ref_type;

		//! Typ stałej referencji do danych
		typedef typename std::add_const<ref_type>::type const_ref_type;

	public:
		//! Konstruktor domyslny
		//! \param size Rozmiar bufora cyklicznego - domyślnie 50, 0 oznacza brak ograniczeń na bufor, 1 - wartość niedozwolona
		StreamBufferT(const size_type size = 50) : maxSize(size) {}
		//! Destruktor wirtualny
		~StreamBufferT() {}
		//! \param size Maksymalny rozmiar bufora - 0 lub mniej oznacza brak ograniczeń		
		void setMaxBufferSize(const size_type size)
		{
			std::lock_guard<std::mutex> lock(synch_);
			maxSize = size;
			adjustBufferSize();
		}

		//! \return Maksymalny rozmiar bufora
		const size_type maxBufferSize() const
		{
			std::lock_guard<std::mutex> lock(synch_);
			return maxSize;
		}

		//! \return Aktualny rozmiar bufora
		const size_type size() const
		{
			std::lock_guard<std::mutex> lock(synch_);
			return bufferData.size();
		}

		//! \param data Dane do włożenia do bufora
		void pushData(const_ref_type data)
		{
			std::lock_guard<std::mutex> lock(synch_);
			bufferData.push_back(data);
			adjustBufferSize();
		}

		//! \param data Dane do włożenia do bufora
		void pushData(T && data)
		{
			std::lock_guard<std::mutex> lock(synch_);
			bufferData.push_back(std::move(data));
			adjustBufferSize();
		}

		//! \param data [out] Obiekt docelowy dla danych bufora
		void data(ListT & data)
		{
			std::lock_guard<std::mutex> lock(synch_);
			data.splice(data.end(), bufferData);
		}

	private:

		//! Metoda dostosowuje rozmiar bufora danych do zadanej wielkości
		void adjustBufferSize()
		{
			if (maxSize > 0){
				while (bufferData.size() > maxSize) { bufferData.pop_front(); }
			}
		}

	private:
		//! Obiekt synchronizujący
		mutable std::mutex synch_;
		//! Bufor danych
		ListT bufferData;
		//! Rozmiar bufora
		volatile size_type maxSize;
	};

	template<typename T>
	class IStreamT : public StreamBase
	{
	public:

		//! Typ interfejsu strumienia do czytania
		typedef IStreamT<T> stream_reader_type;

		//! Typ bufora
		typedef StreamBufferT<T> StreamBufferType;

		//! Typ danych mojego strumienia
		typedef T value_type;

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

			std::lock_guard<std::recursive_mutex> lock(synch_);
			streamBufferList.remove(bufferPtr);
			streamBufferList.push_back(bufferPtr);
		}

		//! \param bufferPtr Odłanczany bufor danych
		void detachBuffer(StreamBufferPtr bufferPtr)
		{
			std::lock_guard<std::recursive_mutex> lock(synch_);
			streamBufferList.remove(bufferPtr);
		}

	protected:

		//! Return Czy podłączono jakieś bufory
		const bool buffersAttached() const
		{
			return streamBufferList.empty() == false;
		}

		//! \param data Data received from the stream
		void pushBufferData(const_ref_type data)
		{
			UTILS_ASSERT(streamBufferList.empty() == true, "Stream data with no buffers");

			for (auto & buff : streamBufferList){
				buff->pushData(data);
			}
		}

	private:
		//! Lista buforów
		StreamBuffersList streamBufferList;
	};

	//! Generyczny obiekt strumienia, synchronizuje dane
	//! \tparam T Typ danych w strumieniu
	template<typename T>
	class StreamT : public IStreamT < T >
	{
	public:
		//! Domyślny konstruktor
		StreamT() {}

		//! \param data Data received from the stream
		void pushData(typename IStreamT<T>::const_ref_type data)
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			pushBufferData(data);

			data_ = data;

			this->notify();
		}

		//! \param data Data received from the stream
		void pushData(T && data)
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			pushBufferData(std::forward<T>(data));

			data_ = std::move(data);

			this->notify();
		}

		//! Destruktor wirtualny
		virtual ~StreamT() {}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia
		virtual void data(typename IStreamT<T>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			d = data_;
		}

	private:
		//! Aktualne dane
		T data_;
	};

	//! Klasa obserwujaca strumien bazowy
	//! \tparam DestStream Strumień docelowy
	template<typename DestStream>
	class BaseStreamUpdater : public IStreamStatusObserver
	{
	public:
		//! \param destStream Strumien docelowy
		BaseStreamUpdater(DestStream * destStream)
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
		DestStream * destStream;
	};

	template<typename T, typename MyDestStream, typename BT = T>
	class StreamModifierBase : public IStreamT < T >
	{
	public:
		//! Typ strumienia bazowego
		typedef IStreamT<BT> BaseStreamType;

		//! Wskaźnik na strumień bazowy
		typedef utils::shared_ptr<BaseStreamType> BaseStreamTypePtr;

	protected:

		StreamModifierBase(BaseStreamTypePtr baseStream, MyDestStream * destStream)
			: baseStream_(baseStream)
		{
			if (baseStream_ == nullptr){
				throw std::invalid_argument("Uninitialized base stream");
			}

			sourceStreamObserver.reset(new BaseStreamUpdater<MyDestStream>(destStream));
			baseStream_->attachObserver(sourceStreamObserver);
		}

		virtual ~StreamModifierBase()
		{
			baseStream_->detachObserver(sourceStreamObserver);
		}

	protected:

		//! Strumień który przykrywam
		BaseStreamTypePtr baseStream_;

	private:
		//! Obserwator strumienia bazowego
		StreamStatusObserverPtr sourceStreamObserver;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename T, typename Filter>
	class StreamFilterT : public StreamModifierBase < T, StreamFilterT<T, Filter> >
	{
	public:

		//! Typ adaptera
		typedef StreamFilterT<T, Filter> StreamFilterType;

		typedef StreamModifierBase < T, StreamFilterT<T, Filter> > MyStreamModifierBase;

		friend class BaseStreamUpdater < StreamFilterType > ;

		typedef typename MyStreamModifierBase::BaseStreamTypePtr BaseStreamTypePtr;

	public:

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamFilterT(BaseStreamTypePtr baseStream,
			const Filter & filter = Filter(), const T & startValue = T())
			: MyStreamModifierBase(baseStream, this), filter_(filter),
			currentData_(startValue)
		{

		}

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamFilterT(BaseStreamTypePtr baseStream,
			const Filter & filter, T && startValue)
			: MyStreamModifierBase(baseStream, this), filter_(filter),
			currentData_(std::move(startValue))
		{

		}

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamFilterT(BaseStreamTypePtr baseStream,
			Filter && filter, const T & startValue = T())
			: MyStreamModifierBase(baseStream, this), filter_(std::move(filter)),
			currentData_(startValue)
		{

		}

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamFilterT(BaseStreamTypePtr baseStream,
			Filter && filter, T && startValue)
			: MyStreamModifierBase(baseStream, this), filter_(std::move(filter)),
			currentData_(std::move(startValue))
		{

		}

		//! Destruktor wirtualny
		virtual ~StreamFilterT()
		{

		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po filtracji
		virtual void data(typename IStreamT<T>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			d = currentData_;
		}

	private:

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		void localUpdate()
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			T bd;
			MyStreamModifierBase::baseStream_->data(bd);

			if (filter(bd) == true){
				currentData_ = bd;
				if (this->buffersAttached() == true){
					pushBufferData(currentData_);
				}

				this->notify();
			}
		}

	private:
		//! Aktualne dane
		mutable T currentData_;
		//! Obiekt do filtrowania danych danych
		Filter filter_;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename T, typename Processor>
	class StreamProcessorT : public StreamModifierBase < T, StreamProcessorT<T, Processor> >
	{
	public:

		//! Typ adaptera
		typedef StreamProcessorT<T, Processor> StreamProcessorType;

		typedef StreamModifierBase < T, StreamProcessorT<T, Processor> > MyStreamModifierBase;
		typedef typename MyStreamModifierBase::BaseStreamTypePtr BaseStreamTypePtr;

		friend class BaseStreamUpdater < StreamProcessorType > ;

	public:

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamProcessorT(BaseStreamTypePtr baseStream,
			const Processor & processor = Processor(), const T & startValue = T())
			: MyStreamModifierBase(baseStream, this), processor_(processor),
			currentData_(startValue), processReuired(false)
		{

		}

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamProcessorT(BaseStreamTypePtr baseStream,
			const Processor & processor, T && startValue)
			: MyStreamModifierBase(baseStream, this), processor_(processor),
			currentData_(std::move(startValue)), processReuired(false)
		{

		}

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamProcessorT(BaseStreamTypePtr baseStream,
			Processor && processor, const T & startValue = T())
			: MyStreamModifierBase(baseStream, this), processor_(std::move(processor)),
			currentData_(startValue), processReuired(false)
		{

		}

		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamProcessorT(BaseStreamTypePtr baseStream,
			Processor && processor, T && startValue)
			: MyStreamModifierBase(baseStream, this), processor_(std::move(processor)),
			currentData_(std::move(startValue)), processReuired(false)
		{

		}

		//! Destruktor wirtualny
		virtual ~StreamProcessorT()
		{

		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po filtracji
		virtual void data(typename IStreamT<T>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			if (processReuired == true){
				process();
			}

			d = currentData_;
		}

	private:

		//! Metoda przetwarza dane i zaznacza że już nie trzeba
		void process() const
		{
			processor_(currentData_);
			processReuired = false;
		}

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		void localUpdate()
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			MyStreamModifierBase::baseStream_->data(currentData_);

			if (this->buffersAttached() == true){
				process();
				pushBufferData(currentData_);
			}
			else{
				processReuired = true;
			}

			this->notify();
		}

	private:
		//! Aktualne dane
		mutable T currentData_;
		//! Obiekt do przetwarzania danych danych
		mutable Processor processor_;
		//! Czy należy przetworzyć dane czy zostały już przetworzone
		volatile mutable bool processReuired;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename Base, typename Dest, typename Extractor>
	class StreamAdapterT : public StreamModifierBase < Dest, StreamAdapterT<Base, Dest, Extractor>, Base >
	{
	public:

		//! Typ adaptera
		typedef StreamAdapterT<Base, Dest, Extractor> StreamAdapterType;

		typedef StreamModifierBase < Dest, StreamAdapterT<Base, Dest, Extractor>, Base > MyStreamModifierBase;
		typedef typename MyStreamModifierBase::BaseStreamTypePtr BaseStreamTypePtr;

		friend class BaseStreamUpdater < StreamAdapterType > ;

	public:

		//! \param baseStream Strumień którego reprezentację zmieniamy
		//! \param extractorFunction Funktor realizujące przepakowanie typów danych
		StreamAdapterT(BaseStreamTypePtr baseStream,
			const Extractor & extractor = Extractor(), const Dest & startValue = Dest())
			: MyStreamModifierBase(baseStream, this), extractor_(extractor),
			currentData_(startValue), unpackRequired_(false)
		{

		}

		//! \param baseStream Strumień którego reprezentację zmieniamy
		//! \param extractorFunction Funktor realizujące przepakowanie typów danych
		StreamAdapterT(BaseStreamTypePtr baseStream,
			Extractor && extractor, const Dest & startValue = Dest())
			: MyStreamModifierBase(baseStream, this), extractor_(std::move(extractor)),
			currentData_(startValue), unpackRequired_(false)
		{

		}

		//! \param baseStream Strumień którego reprezentację zmieniamy
		//! \param extractorFunction Funktor realizujące przepakowanie typów danych
		StreamAdapterT(BaseStreamTypePtr baseStream,
			const Extractor & extractor, Dest && startValue)
			: MyStreamModifierBase(baseStream, this), extractor_(extractor),
			currentData_(std::move(startValue)), unpackRequired_(false)
		{

		}

		//! \param baseStream Strumień którego reprezentację zmieniamy
		//! \param extractorFunction Funktor realizujące przepakowanie typów danych
		StreamAdapterT(BaseStreamTypePtr baseStream,
			Extractor && extractor, Dest && startValue)
			: MyStreamModifierBase(baseStream, this), extractor_(std::move(extractor)),
			currentData_(std::move(startValue)), unpackRequired_(false)
		{

		}

		//! Destruktor wirtualny
		virtual ~StreamAdapterT()
		{

		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po przepakowaniu
		virtual void data(typename IStreamT<Dest>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			if (unpackRequired_ == true){
				extractor_.extract(baseData_, currentData_);
				unpackRequired_ = false;
			}

			d = currentData_;
		}

	private:

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		void localUpdate()
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			Base bd;
			MyStreamModifierBase::baseStream_->data(bd);

			if (extractor_.verify(bd) == true){
				if (this->buffersAttached() == true){
					extractor_.extract(bd, currentData_);
					unpackRequired_ = false;
					pushBufferData(currentData_);
				}
				else{
					unpackRequired_ = true;
					baseData_ = bd;
				}

				this->notify();
			}
		}

	private:
		//! Aktualne dane do rozpakowania
		mutable Base baseData_;
		//! current data
		mutable Dest currentData_;
		//! Funktor do weryfikacji i wypakowywania danych
		Extractor extractor_;
		//! Czy dane wymagają pobrania ze źródła
		mutable volatile bool unpackRequired_;
	};
}

//! Makro definiujące nazwę extractora pola
//! \param memberName Nazwa wypakowywanego pola
#define MEMBER_EXTRACTOR_NAME(memberName) \
	MemberExtractor##memberName

//! Makro definiujące klasę wypakowującą zadane pole
//! \param memberName Nazwa wypakowywanego pola
#define MEMBER_EXTRACTOR(memberName) \
class MEMBER_EXTRACTOR_NAME(memberName){\
public:\
	template<typename SrcType>\
	inline static bool verify(const SrcType &) { return true; }\
	template<typename SrcType, typename DestType>\
	inline static void extract(const SrcType & src, DestType & dest){\
		dest = src.memberName;\
	}\
};

//! Makro definiujące nazwę adaptera dla zadanej klasy bazowej i pola
//! \param baseType Typ bazowy z którego wyciągamy pole
//! \param memberName Nazwa wypakowywanego pola
#define MEMBER_ADAPTER_NAME(baseType, memberName) \
	MemberAdapter##baseType##memberName

//! Makro definiujące adapter dla wyciąganego pola
//! \param baseType Typ bazowy z którego wyciągamy pole
//! \param destType Typ docelowy jeśli chcemy wymusić dostępną konwersję
//! \param memberName Nazwa wypakowywanego pola
#define MEMBER_ADAPTER_EXT(baseType, destType, memberName) \
typedef StreamAdapterT<baseType, destType, MEMBER_EXTRACTOR_NAME(memberName)> MEMBER_ADAPTER_NAME(baseType,memberName);

//! Makro definiujące adapter dla wyciąganego pola
//! \param baseType Typ bazowy z którego wyciągamy pole
//! \param memberName Nazwa wypakowywanego pola
#define MEMBER_ADAPTER(baseType, memberName) \
	MEMBER_ADAPTER_EXT(baseType, decltype(std::declval<baseType>().memberName), memberName);

#endif	//	HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__
