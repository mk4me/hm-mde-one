/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   13:57
	filename: DataChannel.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__DATACHANNEL_H__
#define __HEADER_GUARD_UTILS__DATACHANNEL_H__

#include <vector>
#include <string>

#include <boost/call_traits.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include <utils/Debug.h>
#include <utils/ObserverPattern.h>


////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

	//! Wzorzec wyciągający const referencję do danych - przydatny przy przekazywaniu danych w innych wzorcach
    template<class PointType>
    struct ConstReferenceType
    {
        typedef typename boost::mpl::if_<typename boost::is_pod<PointType>::type, PointType, typename boost::call_traits<PointType>::const_reference>::type type;
    };

    template<class PointType>
    struct ReferenceType
    {
        typedef typename boost::mpl::if_<typename boost::is_pod<PointType>::type, PointType, typename boost::call_traits<PointType>::reference>::type type;
    };

    //! Klasa pozwalająca obserwować obiekty z danymi i modyfikować własne wartości
    //! Używana do tworzenia modyfikatorów danych - średnich kroczących, całkowania, ...
    template<class PointType, class TimeType>
    class ChannelAutoModifier;

	//! Interfejs na potrzeby kanałów sledzacych, umozliwia zmiane wartości próbek dla danego indesku
	template<class PointType, class TimeType>
	class IRawDataChannelExtendedWriter;

    //! Interfejs zapewniający odczyt surowych danych po indexach próbek - czas próbek w kolejności chronologicznej, uniezaleznia od sposobu przechowywania danych(vector, map, ...)
    //! Mozna obserwowac
    template<class PointType, class TimeType>
    class IRawGeneralDataChannelReader : public Observable<IRawGeneralDataChannelReader<PointType, TimeType>>
    {
    public:
		//! Mój typ obiektu czytającego dane
        typedef IRawGeneralDataChannelReader<PointType, TimeType> _MyRawChannelReaderType;

        //! Typ czasu
        typedef TimeType time_type;
        //! Typ danych
        typedef PointType point_type;
        //! Typ stałej referencji do danych dla przekazywania parametrów
        typedef typename ConstReferenceType<point_type>::type point_type_const_reference;

        //! Typ referencji do danych dla przekazywania parametrów
        typedef typename ReferenceType<point_type>::type point_type_reference;

    public:
        //! Typ ilości elementów w kanale
        //typedef typename TimeData::size_type size_type;
		typedef unsigned int size_type;

		//! Zakres danych wg indeksów próbek
		typedef typename std::pair<size_type, size_type> data_range;

        //! Typ obserwatora dla kanału o podanym interfejsie ( typach danych)
        typedef Observer<_MyRawChannelReaderType> _MyObserverType;
        //! Typ dający się obserwować dla kanału o podanym interfejsie ( typach danych)
        typedef Observable<_MyRawChannelReaderType> _MyObservableType;
        //! Timer dla danego kanału danych

        typedef boost::shared_ptr<_MyRawChannelReaderType> _MyChannelPtr;
        typedef boost::shared_ptr<const _MyRawChannelReaderType> _MyChannelConstPtr;

        typedef ChannelAutoModifier<point_type, time_type> _MyChannelTracker;

        typedef IRawDataChannelExtendedWriter<point_type, time_type> _MyExtendedWriter;

        //! Funktor do modyfikacji danych w Trackerach kanałów
        typedef boost::function<void(_MyExtendedWriter &, const _MyRawChannelReaderType &, const _MyRawChannelReaderType &)> _MyModifierType;

    public:
		//! Wirtualny destruktor
        virtual ~IRawGeneralDataChannelReader() {};

		//! \return Sklonowany kanał kanału
        virtual _MyRawChannelReaderType* clone() const = 0;

		//! \return Nowy, pusty kanał o identycznych parametrach
		//virtual _MyRawChannelReaderType* create() const = 0;

        //! \return Nazwa kanału
        virtual const std::string& getName() const = 0;

        //! \return Czas trwania kanału
        virtual time_type getLength() const = 0;

        //! \param idx Indeks próbki
        //! \return Wartość czasu dla danego indeksu
        virtual time_type argument(size_type idx) const = 0;

        //! \param idx Indeks próbki
        //! \return Wartość próbki dla danego indeksu
        virtual point_type_const_reference value(size_type idx) const = 0;

        //! \return Ilość próbek w kanale
        virtual size_type size() const = 0;

        //! \return Czy kanał nie zawiera danych
        virtual bool empty() const = 0;
        //! \param time Czas dla którego chemy uzyskać dwie najblizsze próbki
        //! \return Para indeksow, pierwszy wskazujke próbke o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje próbkę o czasie wiekszym lub rownym zadanemu
		//! Ta implementacja działa w czasie O(log(N)) - podział binarny bo dane sa ułożone chronologicznie
        virtual data_range getValueHelper(time_type time) const
        {
            size_type minIdx = 0;
            size_type maxIdx = size() - 1;

            while(minIdx < maxIdx){
                size_type midIdx = (maxIdx + minIdx) >> 1;
                time_type t = argument(midIdx);

                if(t < time){
                    minIdx = midIdx + 1;
                }else if(t > time){
                    maxIdx = midIdx - 1;
                }else{
                    maxIdx = minIdx = midIdx;
                }
            }

            if(minIdx != 0 && minIdx == maxIdx && argument(minIdx) != time){
                --minIdx;
            }

            return data_range(minIdx, maxIdx);
        }
    };

    template<class PointType, class TimeType>
    class IRawUniformDataChannelReader : public IRawGeneralDataChannelReader<PointType, TimeType>
    {
    public:
        // TODO znalezc lepszy sposob na dostanie się do tych typów (pod gcc)
        typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::data_range data_range;
        typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::time_type time_type;
        typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type size_type;
        virtual ~IRawUniformDataChannelReader() {};

        // Fix na buga w Visual Studio z virtualnym dziedziczeniem i koherentnymi typami
        // Niestety nie można wtedy klonować obiektów typu IRawUniformChannelReader :(
        // Link: https://connect.microsoft.com/VisualStudio/feedback/details/590625/visual-c-incorrectly-reports-ambiguity-when-covariance-is-used-with-virtual-inheritance
        #ifndef _MSC_VER

        virtual IRawUniformDataChannelReader<PointType, TimeType>* clone() const = 0;

        #endif

        //! \return Czas trwania kanału
        virtual float getSamplesPerSecond() const = 0;

		//! \return Czas trwania jednej próbki
        virtual float getSampleDuration() const = 0;

		//! Wydajniejszy mechanizm uzyskiwania próbek okalających dany czas
        virtual data_range getValueHelper(time_type time) const
        {
            // wyznaczenie indeksów
            size_type idx = static_cast<size_type>(time * getSamplesPerSecond());
            UTILS_ASSERT( idx >= 0 && idx < size() );

            return data_range(idx, (idx == size() - 1) ? idx : idx + 1);
        }
    };

	//! Interfejs umozliwia dodawanie próbek do kanału w sposob chronologiczny
    template<class PointType, class TimeType>
    class IRawDataChannelBasicWriter
    {
    public:
		//Wirtualny destruktor
        virtual ~IRawDataChannelBasicWriter() {}
		//! \param time Czas próbki którą dodajemy
		//! \param point Wartość próbki którą dodajemy
        virtual void addPoint(TimeType time, typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point) = 0;
		//! \param point Wartość próbki którą dodajemy (czas generowany automatycznie przez kanał o znanej rozdzielczości)
        virtual void addPoint(typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point) = 0;
    };

	//! Interfejs na potrzeby kanałów sledzacych, umozliwia zmiane wartości próbek dla danego indesku
    template<class PointType, class TimeType>
    class IRawDataChannelExtendedWriter : public virtual IRawDataChannelBasicWriter<PointType, TimeType>
    {
    public:
		//Wirtualny destruktor
        virtual ~IRawDataChannelExtendedWriter() {}
		//! \param idx Indeks próbki którą nadpisujemy
		//! \param point Nowa wartość zadanej próbki
        virtual void setIndexData(typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type idx, typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point) = 0;
    };

	//! Podstawowa implementacja przechowywania danych
	template<class PointType, class TimeType>
	class ChannelStorage
	{
	public:

		//! Typ stałej referencji do danych dla przekazywania parametrów
		typedef typename ConstReferenceType<PointType>::type point_type_const_reference;

		//! Typ referencji do danych dla przekazywania parametrów
		typedef typename ReferenceType<PointType>::type point_type_reference;

		//! Typ przechowujący czas i wartość
		typedef std::pair<TimeType, PointType> value_type;
		//! Indeksowane dane
		typedef std::vector<value_type> TimeData;

		typedef typename TimeData::size_type size_type;

		//! Const iterator dla danych indeksowanych czasem
		typedef typename TimeData::const_iterator const_iterator;
		//! Const reverse iterator dla danych indeksowanych czasem
		typedef typename TimeData::const_reverse_iterator const_reverse_iterator;
		//! Const referencja do danych o charakterze czasowym
		typedef typename TimeData::const_reference const_reference;

	private:
		// Dane stablicowane wg indeksów, zawierają info o czasie próbki i jej wartości
		TimeData timeData;

	public:

		//! Konstruktor domyślny
		ChannelStorage() {};
		//! Konstruktor kopiujący
		//! \param storage Storage do skopiowania
		ChannelStorage(const ChannelStorage & storage) : timeData(storage.timeData) {}
		//! Destruktor
		~ChannelStorage() {}

		//! \return Pierwsza próbka
		const_reference front() const
		{
			return timeData.front();
		}

		//! \return Ostatnia próbka
		const_reference back() const
		{
			return timeData.back();
		}

		//! \return Iteraotr początku
		const_iterator begin() const
		{
			return timeData.begin();
		}

		//! \return Iterator końca
		const_iterator end() const
		{
			return timeData.end();
		}

		//! \return Odwrotny iterator początku
		const_reverse_iterator rbegin() const
		{
			return timeData.rbegin();
		}
		//! \return Odwrotny iterator końca
		const_reverse_iterator rend() const
		{
			return timeData.rend();
		}

		//! \return Czas trwania kanału
		TimeType getLength() const
		{
			TimeType l = 0;
			if(timeData.empty() == false){
				l = timeData.back().first;
			}

			return l;
		}

		//! \param idx Indeks próbki
		//! \return Wartość czasu dla danego indeksu
		TimeType argument(size_type idx) const
		{
			return timeData[idx].first;
		}

		//! \param idx Indeks próbki
		//! \return Wartość próbki dla danego indeksu
		point_type_const_reference value(size_type idx) const
		{
			return timeData[idx].second;
		}

		//! \param idx Indeks próbki
		//! \return Wartość czasu dla danego indeksu
		const_reference pair(size_type idx) const
		{
			return timeData[idx];
		}

		//! \return Ilość próbek w kanale
		size_type size() const
		{
			return timeData.size();
		}

		//! \return Czy kanał nie zawiera danych
		bool empty() const
		{
			return timeData.empty();
		}


		//! \param time Czas dodawanej próbki
		//! \param point Wartość dodawanej próbki
		void addPoint(TimeType time, point_type_const_reference point)
		{
			timeData.push_back(value_type(time, point));
		}

		//! \param idx Indeks ustawianej próbki
		//! \param point Nowa wartość próbki
		void setPoint(size_type idx, point_type_const_reference point)
		{
			timeData[idx].second = point;
		}

		//! Czyści wszystkie próbki
		void clear()
		{
			TimeData().swap(timeData);
		}
	};

	//! Klasa realizująca surowy kanał danych gdzie próbki muszą być dodawane w sposób hronologiczny
    template<class PointType, class TimeType>
    class RawGeneralDataChannel : public IRawGeneralDataChannelReader<PointType, TimeType>, public IRawDataChannelBasicWriter<PointType, TimeType>
    {
        friend class ChannelAutoModifier<PointType, TimeType>;

	public:
		//! Typ obiektu przechowującego dane
		typedef ChannelStorage<PointType, TimeType> Storage;
		typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type size_type;
		typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point_type_const_reference;
		typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::time_type time_type;

        //! Const iterator dla danych indeksowanych czasem
        typedef typename Storage::const_iterator const_iterator;
        //! Const reverse iterator dla danych indeksowanych czasem
        typedef typename Storage::const_reverse_iterator const_reverse_iterator;
        //! Const referencja do danych o charakterze czasowym
        typedef typename Storage::const_reference const_reference;

    protected:
		//! Obiekt przechowujący dane
        Storage storage;
		//! Nazwa kanału
        std::string name;

    public:

		//! Konstruktor
		//! \param name Nazwa kanału
        RawGeneralDataChannel(const std::string & name = std::string("")) : name(name) {}

		//! Konstruktor kopiujący
		//! \param channel Kanał do skopiowania
        RawGeneralDataChannel(const RawGeneralDataChannel & channel) : storage(channel.storage), name(channel.name) {}

		//! Konstruktor "pseudo" kopiujący
		//! \param channel Interfejs kanału do skopiowania
		RawGeneralDataChannel(const IRawGeneralDataChannelReader<PointType, TimeType> & channel) : name(channel.getName())
		{
			//kopiuję zawartość
			for(size_type i = 0; i < channel.size(); ++i){
				const auto & v = channel.pair(i);
				storage.addPoint(v.first, v.second);
			}
		}

		//! \return Klon kanału
        virtual RawGeneralDataChannel<PointType, TimeType> * clone() const
        {
            return new RawGeneralDataChannel(*this);
        }

		//! \return Pusty kanał tego samego typu (parametry kanału są przekazane, ale brak danych)
        virtual RawGeneralDataChannel<PointType, TimeType> * create() const
        {
            return new RawGeneralDataChannel();
        }

		//! \return Nazwa kanału
        virtual const std::string & getName() const
        {
            return name;
        }

		//! \param name Nazwa kanału
        virtual void setName(const std::string & name)
        {
            this->name = name;
        }

        //! \return Czas trwania kanału
        virtual time_type getLength() const
        {
            return storage.getLength();
        }

		//! \param idx Indeks próbki dla której pobieramy czas
		//! \return Czas próbki
        virtual time_type argument(size_type idx) const
        {
            return storage.argument(idx);
        }

		//! \param idx Indeks próbki dla której pobieramy wartość
		//! \return Wartość próbki
        virtual point_type_const_reference value(size_type idx) const
        {
            return storage.value(idx);
        }

        //! \return Ilość próbek w kanale
        virtual size_type size() const
        {
            return storage.size();
        }

        //! \return Czy kanał nie zawiera danych
        virtual bool empty() const
        {
            return storage.empty();
        }

        //! \param idx Indeks punktu.
        //! \return Wartość na podstawie indeksu punktu czasowego.
        const_reference operator[](size_type idx) const
        {
            UTILS_ASSERT((idx >= 0 && idx < size()), "Wrong index parameter");
            return storage.pair(idx);
        }

        //! \return
        const_iterator begin() const
        {
            return storage.begin();
        }

        //! \return
        const_iterator end() const
        {
            return storage.end();
        }

        //! \return
        const_reverse_iterator rbegin() const
        {
            return storage.rbegin();
        }

        //! \return
        const_reverse_iterator rend() const
        {
            return storage.rend();
        }

        virtual void addPoint(time_type time, point_type_const_reference point)
        {
            if(storage.empty() == true){
                if(time != 0) {
                    throw std::runtime_error("First sample time must be 0");
                }
            }else if(time < storage.back().first){
                throw std::runtime_error("Samples time must be in chronological order");
            }

            storage.addPoint(time, point);

            notify();
        }

        virtual void addPoint(point_type_const_reference point)
        {
            throw std::runtime_error("Not supported in general channel");
            notify();
        }
    };

	//! Kanał danych o równo rozmieszczonych próbkach w czasie - rozdzielczość jest znana
    template<class PointType, class TimeType>
    class RawUniformDataChannel : public IRawUniformDataChannelReader<PointType, TimeType>, public IRawDataChannelBasicWriter<PointType, TimeType>
    {
	public:

		typedef ChannelStorage<PointType, TimeType> Storage;
		typedef typename IRawUniformDataChannelReader<PointType, TimeType>::size_type size_type;
		typedef typename IRawUniformDataChannelReader<PointType, TimeType>::point_type_const_reference point_type_const_reference;
		typedef typename IRawUniformDataChannelReader<PointType, TimeType>::time_type time_type;

		//! Const iterator dla danych indeksowanych czasem
		typedef typename Storage::const_iterator const_iterator;
		//! Const reverse iterator dla danych indeksowanych czasem
		typedef typename Storage::const_reverse_iterator const_reverse_iterator;
		//! Const referencja do danych o charakterze czasowym
		typedef typename Storage::const_reference const_reference;

    private:
		//! Ilość próbek na sekundę
        TimeType samplesPerSecond;
		//! Czas trwania jednej próbki
        TimeType invSamplesPerSecond;
		//! Obiekt przechowujący dane
		Storage storage;
		//! Nazwa kanału
		std::string name;


	public:
        RawUniformDataChannel(TimeType samplesPerSecond = 25, const std::string & name = std::string("")) : name(name), samplesPerSecond(samplesPerSecond)
        {
            if(samplesPerSecond <= (TimeType)0.0){
                throw std::runtime_error("Invalid value for samples per second");
            }

            invSamplesPerSecond = (TimeType)1.0 / samplesPerSecond;
        }

        RawUniformDataChannel(const RawUniformDataChannel & channel) :
            name(channel.name),
            storage(channel.storage),
            samplesPerSecond(channel.samplesPerSecond),
            invSamplesPerSecond(channel.invSamplesPerSecond)
        {}

		RawUniformDataChannel(const IRawUniformDataChannelReader<PointType, TimeType> & channel) :
            name(channel.getName()),
            samplesPerSecond(channel.getSamplesPerSecond()),
            invSamplesPerSecond(channel.getSampleDuration())
		{
			//kopiuję zawartość
			for(size_type i = 0; i < channel.size(); ++i){
				storage.addPoint(channel.argument(i), channel.value(i));
			}
		}

    public:
        virtual RawUniformDataChannel<PointType, TimeType> * clone() const
        {
            return new RawUniformDataChannel(*this);
        }

        virtual RawUniformDataChannel<PointType, TimeType> * create() const
        {
            return new RawUniformDataChannel(samplesPerSecond);
        }

		virtual const std::string & getName() const
		{
			return name;
		}

		virtual void setName(const std::string & name)
		{
			this->name = name;
		}

        virtual float getSamplesPerSecond() const
        {
            return samplesPerSecond;
        }

        virtual float getSampleDuration() const
        {
            return invSamplesPerSecond;
        }

		//! \return Czas trwania kanału
		virtual time_type getLength() const
		{
			return storage.getLength();
		}

		virtual time_type argument(size_type idx) const
		{
			return storage.argument(idx);
		}

		virtual point_type_const_reference value(size_type idx) const
		{
			return storage.value(idx);
		}

		//! \return Ilość próbek w kanale
		virtual size_type size() const
		{
			return storage.size();
		}

		//! \return Czy kanał nie zawiera danych
		virtual bool empty() const
		{
			return storage.empty();
		}

		//! \return Indeks punktu.
		//! \return Wartość na podstawie indeksu punktu czasowego.
		const_reference operator[](size_type idx) const
		{
			UTILS_ASSERT((idx >= 0 && idx < size()), "Wrong index parameter");
			return storage.pair(idx);
		}

		//! \return
		const_iterator begin() const
		{
			return storage.begin();
		}

		//! \return
		const_iterator end() const
		{
			return storage.end();
		}

		//! \return
		const_reverse_iterator rbegin() const
		{
			return storage.rbegin();
		}

		//! \return
		const_reverse_iterator rend() const
		{
			return storage.rend();
		}

        virtual void addPoint(time_type time, point_type_const_reference point)
        {
            if(storage.empty() == true){
                if(time != 0) {
                    throw std::runtime_error("First sample time must be 0");
                }
            }else if(time < storage.back().first){
                throw std::runtime_error("Samples time must be in chronological order");
            }else if(time - storage.back().first != invSamplesPerSecond){
                throw std::runtime_error("Sample time does not match defined delay");
            }

            storage.addPoint(time, point);

            notify();
        }

        virtual void addPoint(point_type_const_reference point)
        {
            storage.addPoint((TimeType)storage.size() * invSamplesPerSecond, point);
            notify();
        }
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_UTILS__DATACHANNEL_H__
