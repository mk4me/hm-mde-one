/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   13:57
	filename: DataChannel.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__DATACHANNEL_H__
#define __HEADER_GUARD_CORE__DATACHANNEL_H__

#include <vector>
#include <string>

#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <utils/Debug.h>
#include <utils/ObserverPattern.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////
    template<bool flag, class T, class U>
    struct Select
    {
        typedef T Result;
    };

    template<class T, class U>
    struct Select<false, T, U>
    {
        typedef U Result;
    };

    //! Wzorzec wyci¹gaj¹cy const referencjê do danych - przydatny przy przekazywaniu danych w innych wzorcach
    template<class PointType>
    struct ConstReferenceType
    {
        typedef typename Select<boost::is_pod<PointType>::value, PointType, typename boost::call_traits<PointType>::const_reference>::Result type;
    };

    template<class PointType>
    struct ReferenceType
    {
        typedef typename Select<boost::is_pod<PointType>::value, PointType, typename boost::call_traits<PointType>::reference>::Result type;
    };

    //! Interfejs do czytania opisu kana³u - osi x i y oraz nazwy kana³u.
    class IChannelDescriptorReader
    {
    public:
        virtual ~IChannelDescriptorReader() {}

        //! \return
        virtual const std::string& getTimeBaseUnit() const = 0;

        virtual float getTimeScaleFactor() const = 0;

        //! \return
        virtual const std::string& getValueBaseUnit() const = 0;

        virtual float getValueScaleFactor() const = 0;
    };

    //! Interfejs do modyfikacji opisu kana³u - osi x i y oraz nazwy kana³u.
    class IChannelDescriptorWriter
    {
    public:
        virtual ~IChannelDescriptorWriter() {}

        //! \param xUnit
        virtual void setTimeBaseUnit(const std::string& xUnit) = 0;

        virtual void setTimeScaleFactor(float factor) = 0;

        //! \param yUnit
        virtual void setValueBaseUnit(const std::string& yUnit) = 0;

        virtual void setValueScaleFactor(float factor) = 0;
    };


    class IChannelDescriptor : public virtual IChannelDescriptorReader, public virtual IChannelDescriptorWriter
    {
    public:
        virtual ~IChannelDescriptor() {}

    };

    //! Implementacja interfejsu IChannelDescriptor
    class ChannelDescriptor : public IChannelDescriptor
    {
    private:
        //! Skala czasu
        float timeScaleFactor;
        //! Jednostka czasu
        std::string timeBaseUnit;
        //! Skala wartoœci
        float valueScaleFactor;
        //! Jendostka wartoœci
        std::string valueBaseUnit;

    public:

        ChannelDescriptor() : timeScaleFactor(1.0f), valueScaleFactor(1.0f) {}

        ChannelDescriptor(const std::string & valueBaseUnit,
            float valueScaleFactor = 1.0f, float timeScaleFactor = 1.0f,
            const std::string & timeBaseUnit = std::string("s"))
            : valueBaseUnit(valueBaseUnit), valueScaleFactor(valueScaleFactor),
            timeScaleFactor(timeScaleFactor), timeBaseUnit(timeBaseUnit)
        {

        }

        ChannelDescriptor(const ChannelDescriptor & channel)
            : valueBaseUnit(channel.valueBaseUnit),
            valueScaleFactor(channel.valueScaleFactor),
            timeScaleFactor(channel.timeScaleFactor),
            timeBaseUnit(channel.timeBaseUnit)
        {

        }

        ChannelDescriptor(const IChannelDescriptorReader & channel)
            : valueBaseUnit(channel.getValueBaseUnit()),
            valueScaleFactor(channel.getValueScaleFactor()),
            timeScaleFactor(channel.getTimeScaleFactor()),
            timeBaseUnit(channel.getTimeBaseUnit())
        {

        }

    public:

        virtual ~ChannelDescriptor() {}

        //! \return
        virtual const std::string& getTimeBaseUnit() const
        { 
            return timeBaseUnit;
        }

        //! \return
        virtual float getTimeScaleFactor() const
        { 
            return timeScaleFactor;
        }

        //! \return
        virtual const std::string& getValueBaseUnit() const
        { 
            return valueBaseUnit;
        }

        //! \return
        virtual float getValueScaleFactor() const
        { 
            return valueScaleFactor;
        }

        //! \param timeBaseUnit
        virtual void setTimeBaseUnit(const std::string& timeBaseUnit) 
        { 
            this->timeBaseUnit = timeBaseUnit;
        }

        //! \param timeBaseUnit
        virtual void setTimeScaleFactor(float timeScaleFactor) 
        { 
            this->timeScaleFactor = timeScaleFactor;
        }

        //! \param yUnit
        virtual void setValueBaseUnit(const std::string& valueBaseUnit) 
        { 
            this->valueBaseUnit = valueBaseUnit;
        }

        //! \param timeBaseUnit
        virtual void setValueScaleFactor(float valueScaleFactor) 
        { 
            this->valueScaleFactor = valueScaleFactor;
        }
    };

    //! Klasa pozwalaj¹ca obserwowaæ obiekty z danymi i modyfikowaæ w³asne wartoœci
    //! U¿ywana do tworzenia modyfikatorów danych - œrednich krocz¹cych, ca³kowania, ...
    template<class PointType, class TimeType>
    class ChannelAutoModifier;

    //! Klasa zapewniajaca dostep do danych w sposob ciagly (sposob realizacji wrazenia ciaglasci zalezny od implementacji)
    template<class PointType, class TimeType>
    class IDataChannelTimeAccessor;

    //! Interfejs umozliwia dodawanie probek do kanalu w sposob chronologiczny
    template<class PointType, class TimeType>
    class IRawDataChannelBasicWriter;

    //! Interfejs na potrzeby kanalow sledzacych, umozliwia zmiane wartosci probek dla danego indesku
    template<class PointType, class TimeType>
    class IRawDataChannelExtendedWriter;

    //! Interfejs zapewniajacy odczyt surowych danych po indexach probek - czas probek w kolejnosci chronologicznej, uniezaleznia od sposobu przechowywania danych(vector, map, ...)
    //! Mozna obserwowac
    template<class PointType, class TimeType>
    class IRawGeneralDataChannelReader : public virtual Observable<IRawGeneralDataChannelReader<PointType, TimeType>>
    {
    public:

        typedef IRawGeneralDataChannelReader<PointType, TimeType> _MyRawChannelReaderType;

        //! Typ czasu
        typedef TimeType time_type;
        //! Typ danych
        typedef PointType point_type;
        //! Typ sta³ej referencji do danych dla przekazywania parametrów
        typedef typename ConstReferenceType<point_type>::type point_type_const_reference;

        //! Typ referencji do danych dla przekazywania parametrów
        typedef typename ReferenceType<point_type>::type point_type_reference;

    protected:
        //! Typ danych o charakterze czasowym - czas -> próbka
        typedef std::pair<time_type, point_type> value_type;
        //! Typ przechowuj¹cy dane indeksowane po czasie
        typedef std::vector<value_type> TimeData;

    public:
        //! Typ iloœci elementów w kanale
        typedef typename TimeData::size_type size_type;

        //! Typ obserwatora dla kana³u o podanym interfejsie ( typach danych)
        typedef Observer<_MyRawChannelReaderType> _MyObserverType;
        //! Typ dajacy siê obserwowaæ dla kana³u o podanym interfejsie ( typach danych)
        typedef Observable<_MyRawChannelReaderType> _MyObservableType;
        //! Timer dla danego kana³u danych

        typedef boost::shared_ptr<_MyRawChannelReaderType> _MyChannelPtr;
        typedef boost::shared_ptr<const _MyRawChannelReaderType> _MyChannelConstPtr;

        typedef ChannelAutoModifier<point_type, time_type> _MyChannelTracker;

        typedef IRawDataChannelExtendedWriter<point_type, time_type> _MyExtendedWriter;

        //! Funktor do modyfikacji danych w Trackerach kana³ów
        typedef boost::function<void(_MyExtendedWriter &, const _MyRawChannelReaderType &, const _MyRawChannelReaderType &)> _MyModifierType;

    public:

        virtual ~IRawGeneralDataChannelReader() {};

        virtual _MyRawChannelReaderType* clone() const
        {
            return nullptr;
        }

        //! \return
        virtual const std::string& getName() const = 0;

        //! \return Czas trwania kana³u
        virtual time_type getLength() const = 0;

        //! \param idx Indeks probki
        //! \return Wartosc czasu dla danego indeksu
        virtual time_type argument(size_type idx) const = 0;

        //! \param idx Indeks probki
        //! \return Wartosc probki dla danego indeksu
        virtual point_type_const_reference value(size_type idx) const = 0;

        //! \return Iloœæ próbek w kanale
        virtual size_type size() const = 0;

        //! \return Czy kana³ nie zawiera danych
        virtual bool empty() const = 0;
        //! \param time Czas dla ktorego chemy uzyskac dwie najblizsze probki
        //! \return para indeksow, pierwszy wskazujke probke o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje probke o czasie wiekszym lub rownym zadanemu
        virtual std::pair<size_type, size_type> getValueHelper(time_type time) const
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

            return std::pair<size_type, size_type>(minIdx, maxIdx);
        }
    };

    template<class PointType, class TimeType>
    class IRawUniformDataChannelReader : public virtual IRawGeneralDataChannelReader<PointType, TimeType>
    {
    public:

        virtual ~IRawUniformDataChannelReader() {};

        // Fix na buga w Visual Studio z virtualnym dziedziczeniem i koherentnymi typami
        // Niestety nie mo¿na wtedy klonowaæ obiektów typu IRawUniformChannelReader :(
        // Link: https://connect.microsoft.com/VisualStudio/feedback/details/590625/visual-c-incorrectly-reports-ambiguity-when-covariance-is-used-with-virtual-inheritance
        #ifndef _MSC_VER

        virtual IRawUniformDataChannelReader<PointType, TimeType>* clone() const
        {
            return nullptr;
        }

        #endif

        //! \return Czas trwania kana³u
        virtual float getSamplesPerSecond() const = 0;

        virtual float getSampleDuration() const = 0;

        virtual std::pair<size_type, size_type> getValueHelper(time_type time) const
        {
            // wyznaczenie indeksów
            size_type idx = static_cast<size_type>(time * getSamplesPerSecond());
            UTILS_ASSERT( idx >= 0 && idx < size() );

            return std::pair<size_type, size_type>(idx, (idx == size() - 1) ? idx : idx + 1);
        }
    };

    template<class PointType, class TimeType>
    class IRawDataChannelBasicWriter
    {
    public:
        virtual ~IRawDataChannelBasicWriter() {}

        virtual void addPoint(TimeType time, typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point) = 0;
        virtual void addPoint(typename typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point) = 0;
    };

    template<class PointType, class TimeType>
    class IRawDataChannelExtendedWriter : public virtual IRawDataChannelBasicWriter<PointType, TimeType>
    {
    public:
        virtual ~IRawDataChannelExtendedWriter() {}
      
        virtual void setIndexData(typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type idx, typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_const_reference point) = 0;
    };

    template<class PointType, class TimeType>
    class IDataChannelTimeAccessor
    {
    public:
        typedef boost::shared_ptr<IDataChannelTimeAccessor<PointType, TimeType>> IDataChannelTimeAccessorPtr;
        typedef boost::shared_ptr<const IDataChannelTimeAccessor<PointType, TimeType>> IDataChannelTimeAccessorConstPtr;

    public:
        virtual ~IDataChannelTimeAccessor() {}

        virtual PointType getChannelValue(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel) const = 0;
        
        PointType getValue(TimeType time) const
        {
            return getChannelValue(time, *getChannel());
        }

        PointType operator[](TimeType time) const
        {
            return getValue(time);
        }

        virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & getChannel() const = 0;
        virtual void setChannel(const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & channel) = 0;
    };

    template<class PointType, class TimeType>
    class RawGeneralDataChannel : public virtual IRawGeneralDataChannelReader<PointType, TimeType>, public virtual IRawDataChannelBasicWriter<PointType, TimeType>
    {
        friend class ChannelAutoModifier<PointType, TimeType>;

    public:
        //! Const iterator dla danych indeksowanych czasem
        typedef typename TimeData::const_iterator const_iterator;
        //! Const reverse iterator dla danych indeksowanych czasem
        typedef typename TimeData::const_reverse_iterator const_reverse_iterator;
        //! Const referencja do danych o charakterze czasowym
        typedef typename TimeData::const_reference const_reference;

    protected:
        TimeData data;
        std::string name;

    public:
        RawGeneralDataChannel(const std::string & name = std::string("")) : name(name) {}
        RawGeneralDataChannel(const IRawGeneralDataChannelReader & channel, const std::string & name = std::string("")) : data(channel.begin(), channel.end()), name(name) {}
        RawGeneralDataChannel(const RawGeneralDataChannel & channel) : data(channel.data), name(channel.name) {}
        RawGeneralDataChannel(const RawGeneralDataChannel & channel, boost::false_type) : name(channel.name) {}


        virtual RawGeneralDataChannel<PointType, TimeType> * clone() const
        {
            return new RawGeneralDataChannel(*this);
        }

        virtual RawGeneralDataChannel<PointType, TimeType> * create() const
        {
            return new RawGeneralDataChannel(*this, boost::false_type());
        }

        virtual const std::string & getName() const
        {
            return name;
        }

        virtual void setName(const std::string & name)
        {
            this->name = name;
        }

        //! \return Czas trwania kana³u
        virtual time_type getLength() const
        {
            if(data.empty() == true){
                return 0;
            }

            return data.rbegin()->first;
        }

        virtual time_type argument(size_type idx) const
        {
            return data[idx].first;
        }

        virtual point_type_const_reference value(size_type idx) const
        {
            return data[idx].second;
        }

        //! \return Iloœæ próbek w kanale
        virtual size_type size() const
        {
            return data.size();
        }

        //! \return Czy kana³ nie zawiera danych
        virtual bool empty() const
        {
            return data.empty();
        }

        //! \return Indeks punktu.
        //! \return Wartoœæ na podstawie indeksu punktu czasowego.
        virtual const_reference operator[](size_type idx) const
        {
            UTILS_ASSERT((idx >= 0 && idx < size()), "Wrong index parameter");
            return data[idx];
        }

        //! \return
        virtual const_iterator begin() const
        {
            return data.begin();
        }

        //! \return
        virtual const_iterator end() const
        {
            return data.end();
        }

        //! \return
        virtual const_reverse_iterator rbegin() const
        {
            return data.rbegin();
        }

        //! \return
        virtual const_reverse_iterator rend() const
        {
            return data.rend();
        }

        virtual void addPoint(time_type time, point_type_const_reference point)
        {
            if(data.empty() == true){
                if(time != 0) {
                    throw std::runtime_error("First sample time must be 0");
                }
            }else if(time < data.back().first){
                throw std::runtime_error("Samples time must be in chronological order");
            }

            data.push_back(value_type(time, point));

            notify();
        }

        virtual void addPoint(point_type_const_reference point)
        {
            throw std::runtime_error("Not supported in general channel");
            notify();
        }
    };

    template<class PointType, class TimeType>
    class RawUniformDataChannel : public IRawUniformDataChannelReader<PointType, TimeType>, public RawGeneralDataChannel<PointType, TimeType>
    {
    private:
        float samplesPerSecond;
        float invSamplesPerSecond;

    protected:
        RawUniformDataChannel(float samplesPerSecond = 25, const std::string & name = std::string("")) : RawGeneralDataChannel(name), samplesPerSecond(samplesPerSecond)
        {
            if(samplesPerSecond <= 0){
                throw std::runtime_error("Invalid value for samples per second");
            }

            invSamplesPerSecond = 1.0 / samplesPerSecond;
        }

        RawUniformDataChannel(const RawUniformDataChannel & channel) : RawGeneralDataChannel(channel), samplesPerSecond(channel.samplesPerSecond), invSamplesPerSecond(channel.invSamplesPerSecond) {}
        RawUniformDataChannel(const RawUniformDataChannel & channel, boost::false_type) : RawGeneralDataChannel(channel.name), samplesPerSecond(channel.samplesPerSecond), invSamplesPerSecond(channel.invSamplesPerSecond) {}


    public:
        virtual RawUniformDataChannel<PointType, TimeType> * clone() const
        {
            return new RawUniformDataChannel(*this);
        }

        virtual RawUniformDataChannel<PointType, TimeType> * create() const
        {
            return new RawUniformDataChannel(*this, boost::false_type());
        }

        virtual float getSamplesPerSecond() const
        {
            return samplesPerSecond;
        }

        virtual float getSampleDuration() const
        {
            return invSamplesPerSecond;
        }

        virtual void addPoint(time_type time, point_type_const_reference point)
        {
            if(data.empty() == true){
                if(time != 0) {
                    throw std::runtime_error("First sample time must be 0");
                }
            }else if(time < data.back().first){
                throw std::runtime_error("Samples time must be in chronological order");
            }else if(time - data.back().first != invSamplesPerSecond){
                throw std::runtime_error("Sample time does not match defined delay");
            }

            data.push_back(value_type(time, point));

            notify();
        }

        virtual void addPoint(point_type_const_reference point)
        {
            time_type t = 0;
            if(data.empty() == false){
                t = data.back().first + invSamplesPerSecond;
            }

            data.push_back(value_type(t, point));

            notify();
        }
    };

    template<class PointType, class TimeType>
    class SimpleDataChannelTimeAccessor : public IDataChannelTimeAccessor<PointType, TimeType>
    {
    protected:
        SimpleDataChannelTimeAccessor() {}

    public:
        SimpleDataChannelTimeAccessor(const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & channel) : channel(channel)
        {
            UTILS_ASSERT((channel != nullptr), "Wrong channel for time accessor");
        }

        virtual PointType getChannelValue(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel) const
        {
            if(channel.empty() == true){
                throw std::runtime_error("Accessing data in empty channel");
            }

            if(time < 0){
                return channel.value(0);
            }else if(time > channel.argument(channel.size() - 1)){
                return channel.value(channel.size() - 1);
            }else{
                auto r = channel.getValueHelper(time);
                if(channel.argument(r.first) == time){
                    return channel.value(r.first);
                }else if(channel.argument(r.second) == time){
                    return channel.value(r.second);
                }else{
                    if(time - channel.argument(r.first) < channel.argument(r.second) - time){
                        return channel.value(r.first);
                    }else{
                        return channel.value(r.second);
                    }
                }
            }            
        }

        virtual const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & getChannel() const
        {
            return channel;
        }

        virtual void setChannel(const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & channel)
        {
            if(channel == nullptr){
                throw std::runtime_error("Wrong channel for time accessor");
            }

            this->channel = channel;
        }

        virtual ~SimpleDataChannelTimeAccessor() {}

    protected:
        typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr channel;
    };

    template <class PointType, class TimeType, class PointRefType = typename IRawGeneralDataChannelReader<PointType, TimeType>::point_type_reference >
    class ExceptionTimeOverflowResolver
    {
    public:
        //! Domyœ³ny konstruktor
        ExceptionTimeOverflowResolver() {}

        //! Konstruktor kopiujacy
        ExceptionTimeOverflowResolver(const ExceptionTimeOverflowResolver & resolver) {}
        //! Destruktor polimorficzny
        ~ExceptionTimeOverflowResolver() {}

    public:
        //! \param time Czas ponizej zera
        //! \param point Zwracana wartosc
        PointType timeUnderflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel) const
        {
            throw std::runtime_error("Time value less than 0");
            return PointType();
        }

        //! \param time Czas poza dlugoscia kanalu
        //! \param point Zwracana wartosc
        PointType timeOverflow(TimeType time, typename const IRawGeneralDataChannelReader<PointType, TimeType> & channel) const
        {
            throw std::runtime_error("Time value greater than channel length");
            return PointType();
        }
    };

    //! Manipulator danych bazuj¹cy na operatorach typu. Interpoluje liniowo.
    template <class PointType, class TimeType>
    struct DataOperatorsManipulator
    {
        typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::size_type IndexType;

        //! \param point Zwracana interpolowana wartosc
        //! \param p1 Probka dla czasu ponizej lub identycznego
        //! \param Probka dla czasu powyzej lub identycznego
        //! \param time Wartosc okreslajaca procent przesuniecia wzgledem probki ponizej
        PointType extractValue(IndexType idxA, IndexType idxB, TimeType time, const typename IRawGeneralDataChannelReader<PointType, TimeType> & channel) const
        {
            //// lerp
            //auto iA = channel.value(idxA);
            //auto iB = channel.value(idxB);
            //auto aA = channel.argument(idxA);
            //auto aB = channel.argument(idxB);
            //auto ret = iA + (iB - iA) * (time - aA) / (aB - aA);
            //return ret;
            return channel.value(idxA) + (channel.value(idxB) - channel.value(idxA)) * (time - channel.argument(idxA)) / (channel.argument(idxB) - channel.argument(idxA));
            
        }
    };

    template<class PointType, class TimeType, class TimeOverflowResolver = ExceptionTimeOverflowResolver<PointType, TimeType>, class Manipulator = DataOperatorsManipulator<PointType, TimeType>>
    class GeneralDataChannelTimeAccessor : public SimpleDataChannelTimeAccessor<PointType, TimeType>, public TimeOverflowResolver, public Manipulator
    {
    public:
        GeneralDataChannelTimeAccessor(const typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyChannelConstPtr & channel) : SimpleDataChannelTimeAccessor(channel) {}

    protected:

        GeneralDataChannelTimeAccessor() {}

    public:

        virtual PointType getChannelValue(TimeType time, const IRawGeneralDataChannelReader<PointType, TimeType> & channel) const
        {
            if(channel.empty() == true){
                throw std::runtime_error("Accessing data in empty channel");
            }

            if(time < 0){
                return TimeOverflowResolver::timeUnderflow(time, channel);
            }else if(time > channel.argument(channel.size() - 1)){
                return TimeOverflowResolver::timeOverflow(time, channel);
            }else{
                auto r = channel.getValueHelper(time);
                if(channel.argument(r.first) == time){
                    return channel.value(r.first);
                }else if(channel.argument(r.second) == time){
                    return channel.value(r.second);
                }else{
                    auto test = Manipulator::extractValue(r.first, r.second, time, channel);
                    return test;
                }
            }            
        }
    };

    template<class TimeType>
    class ITimerReader
    {
    public:
        typedef ITimerReader<TimeType> TimerReaderType;
        typedef boost::shared_ptr<TimerReaderType> TimerReaderPtr;
        typedef boost::shared_ptr<const TimerReaderType> TimerReaderConstPtr;

    public:
        virtual ~ITimerReader() {}

        virtual TimeType getTime() const = 0;
    };

    template<class TimeType>
    class ITimer : public ITimerReader<TimeType> 
    {
    public:
        typedef ITimer<TimeType> TimerType;
        typedef boost::shared_ptr<TimerType> TimerPtr;
        typedef boost::shared_ptr<const TimerType> TimerConstPtr;

    public:

        virtual void setTime(TimeType time) = 0;
    };

    template<class TimeType>
    class Timer : public ITimer<TimeType>
    {
    public:

        Timer() : time(0) {}

        template<class U>
        Timer(const ITimer<U> & timer) : time(timer.getTime()) {}

        template<class U>
        Timer(const Timer<U> & timer) : time(timer.time) {}

        template<class U>
        Timer(U time) : time(time) {}

        virtual ~Timer() {}

        virtual void setTime(TimeType time)
        {
            this->time = time;
        }

        virtual TimeType getTime() const
        {
            return time;
        }

    private:
        TimeType time;
    };

    template<class TimeType>
    class TimerOnTimer : public ITimerReader<TimeType>
    {
    public:

        TimerOnTimer( const TimerReaderType::TimerReaderPtr & timerReader ) : timerReader(timerReader), constTimerReader(timerReader)
        {
            if(timerReader == nullptr){
                throw std::runtime_error("Wrong TimerReader for TimerOnTimer");
            }
        }

        virtual TimeType getTime() const
        {
            return constTimerReader->getTime();
        }

        const TimerReaderType::TimerReaderPtr & getTrueTimerReader()
        {
            return timerReader;
        }

        const TimerReaderType::TimerReaderConstPtr & getTrueTimerReader() const
        {
            return constTimerReader;
        }

    private:
        TimerReaderType::TimerReaderPtr timerReader;
        TimerReaderType::TimerReaderConstPtr constTimerReader;
    };

    template<class PointType, class TimeType>
    class CurrentValueExtractor
    {
    public:
        typedef typename ITimer<TimeType>::TimerType TimerType;
        typedef IDataChannelTimeAccessor<PointType, TimeType> Accessor;
        typedef IRawGeneralDataChannelReader<PointType, TimeType> DataChannel;

    public:

        CurrentValueExtractor(const typename Accessor::IDataChannelTimeAccessorPtr & accessor, const typename TimerType::TimerPtr & timer = TimerType::TimerPtr(new Timer<TimeType>())) : timer(timer), constTimer(timer),
            accessor(accessor), constAccessor(accessor)
        {
            if(timer == nullptr){
                throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
            }

            if(accessor == nullptr){
                throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
            }

            if(accessor->getChannel() == nullptr){
                throw std::runtime_error("Accessor not initialized with channel for CurrentValueExtractor");
            }
        }

        virtual ~CurrentValueExtractor() {}

        virtual PointType getCurrentValue() const
        {
            return accessor->getValue(timer->getTime());
        }

        virtual const typename Accessor::IDataChannelTimeAccessorPtr & getDataAccesor()
        {
            return accessor;
        }

        virtual const typename Accessor::IDataChannelTimeAccessorConstPtr & getDataAccesor() const
        {
            return constAccessor;
        }

        virtual void setDataAccesor(const typename Accessor::IDataChannelTimeAccessorPtr & accessor)
        {
            if(accessor == nullptr){
                throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
            }

            this->constAccessor = this->accessor = accessor;
        }

        virtual const typename TimerType::TimerPtr & getTimer()
        {
            return timer;
        }

        virtual const typename TimerType::TimerConstPtr & getTimer() const
        {
            return constTimer;
        }

        virtual void setTimer(const typename TimerType::TimerPtr & timer)
        {
            if(timer == nullptr){
                throw std::runtime_error("Wrong accessor for CurrentValueExtractor");
            }

            this->constTimer = this->timer = timer;
        }

    private:
        typename TimerType::TimerPtr timer;
        typename TimerType::TimerConstPtr constTimer;

        typename Accessor::IDataChannelTimeAccessorPtr accessor;
        typename Accessor::IDataChannelTimeAccessorConstPtr constAccessor;
    };

    template<class T>
    class EpsilonManager
    {
    public:
        EpsilonManager() : epsilon(0), smallerIfEqual(false) {}
        EpsilonManager(const EpsilonManager & manager) : epsilon(manager.epsilon), smallerIfEqual(manager.smallerIfEqual) {}
        EpsilonManager(T epsilon, bool smallerIfEqual) : epsilon(epsilon), smallerIfEqual(smallerIfEqual) {}

        //! \return Domyœlny epsilon dla czasu
        T getEpsilon() const
        {
            return epsilon;
        }

        //! \param
        void setEpsilon(T epsilon)
        {
            this->epsilon = epsilon;
        }

        //! \return Czy brana bêdzie mniejsza próbka jeœli równe
        bool getSmallerIfEqualPolicy() const
        {
            return smallerIfEqual;
        }


        //! \param
        void setSmallerIfEqualPolicy(bool smallerIfEqual)
        {
            this->smallerIfEqual = smallerIfEqual;
        }

    private:
        T epsilon;
        bool smallerIfEqual;
    };

    //! Klasa ze statystykami kana³u - instancja trzymana po stronie kana³u, leniwie aktualizowana w momencie zapytania
    template<class PointType, class TimeType>
    class ChannelStats
    {
    public:
        typedef IRawGeneralDataChannelReader<PointType, TimeType> Channel;
        typedef typename Channel::_MyChannelConstPtr ChannelConstPtr;
        typedef typename Channel::_MyChannelPtr ChannelPtr;
        typedef typename Channel::point_type_const_reference PointTypeConstReference;

    private:
        //! Klasa obserwuj¹ca - obiekt statystyk bezpoœrendio nie moze byæ obserwatore bo mo¿naby go by³o przypadkiem podpi¹æ pod kilka kana³ów!!
        class ChannelObserver : public Channel::_MyObserverType
        {
        public:
            //! \see update wzorca Obserwator
            virtual void update(const Channel * channel)
            {
                stats->changed = true;
            }

            //! \param Obiekt statystyk aktualizowany na zmianê danych kana³u
            void setChannelStats(ChannelStats * stats)
            {
                this->stats = stats;
            }

        private:
            ChannelStats* stats;
        };

    public:

        ChannelStats(const ChannelPtr & channel) : channel(channel), constChannel(channel), changed(false)
        {
            if(channel == nullptr){
                throw std::runtime_error("Invalid channel for statistics");
            }

            if(channel->empty() == false){
                changed = true;
            }

            observer.setChannelStats(this);
            channel->attach(&observer);
        }

        virtual ~ChannelStats() {}

        //! \return Srednia wartoœ danych kana³u
        PointTypeConstReference meanValue() const
        {
            if(changed == true){
                updateStats();
            }

            return meanVal;
        }

        //! \return Minimalna wartoœæ danych kana³u
        PointTypeConstReference minValue() const
        {
            if(changed == true){
                updateStats();
            }

            return minVal;
        }

        //! \return Maksymalna wartoœæ danych kana³u
        PointTypeConstReference maxValue() const
        {
            if(changed == true){
                updateStats();
            }

            return maxVal;
        }

        //! \return Variancja danych kana³u
        PointTypeConstReference varianceValue() const
        {
            if(changed == true){
                updateStats();
            }

            return varianceVal;
        }

        //! \return Kana³ którego zegar dotyczy
        const ChannelPtr & getChannel()
        {
            return channel;
        }

        //! \return Kana³ którego zegar dotyczy
        const ChannelConstPtr & getChannel() const
        {
            return constChannel;
        }

    protected:
        //! Aktualizuje statystyki kana³u
        virtual void updateStats() const
        {
            using namespace boost::accumulators;
            typedef boost::accumulators::accumulator_set<PointType, features< tag::min, tag::max, tag::mean, tag::variance >> Stats;

            changed = false;
            Stats sts;

            for(auto i = 0; i < channel->size(); i++){
                float val = channel->value(i);
                sts(channel->value(i));
            }

            meanVal = mean(sts);
            minVal = min(sts);
            maxVal = max(sts);
            varianceVal = variance(sts);
        }

    private:
        //! Œrednia wartoœæ danych w kanale
        mutable PointType meanVal;
        //! Minimalna wartoœæ danych w kanale
        mutable PointType minVal;
        //! Maksymalna wartoœæ danych w kanale
        mutable PointType maxVal;
        //! Wariancja danych w kanale
        mutable PointType varianceVal;

        //! Czy kana³ uleg³ zmianie
        mutable bool changed;

        //! Obserwator zmian kana³u
        ChannelObserver observer;
        //! Obserwowany kana³
        ChannelConstPtr constChannel;

        ChannelPtr channel;
    };

    template<class PointType, class TimeType>
    class IChannelAutoModifier : public virtual IRawGeneralDataChannelReader<PointType, TimeType>
    {
    public:

        virtual ~IChannelAutoModifier() {}

        virtual _MyChannelPtr getObservedChannel() = 0;

        virtual _MyChannelConstPtr getObservedChannel() const = 0;

        virtual void setObservedChannel(const _MyChannelPtr & channel) = 0;

        virtual const _MyModifierType & getModifier() const = 0;
    };

    template<class PointType, class TimeType>
    class ChannelAutoModifier : public IChannelAutoModifier<PointType, TimeType>, private IRawDataChannelExtendedWriter<PointType, TimeType>, private RawGeneralDataChannel<PointType, TimeType>
    {

    private:
        //! Prywatna klasa realizuj¹ca obserwacjê zadanej klasy. Na jej zmianê wyzwala modyfikacje
        class UpdateNotifier : public _MyObserverType
        {
        public:

            virtual void update(const _MyRawChannelReaderType * channel)
            {
                mod->update();
            }

            void setModifier(ChannelAutoModifier* mod)
            {
                this->mod = mod;
            }

        private:
            ChannelAutoModifier* mod;
        };

        friend class UpdateNotifier;

    public:

        virtual ~ChannelAutoModifier() {}

        virtual _MyChannelPtr getObservedChannel()
        {
            return channel;
        }

        virtual _MyChannelConstPtr getObservedChannel() const
        {
            return channel;
        }

        virtual void setObservedChannel(const _MyChannelPtr & channel)
        {
            if(channel == nullptr){
                throw std::runtime_error("Wrong channel for Tracker");
            }

            data.swap(TimeData());

            this->channel->detach(notifier.get());
            this->channel = channel;
            channel->attach(notifier.get());

            //sprawdziæ czy s¹ jakieœ dane? jesli tak odpaliæ modifier
            if(channel->empty() == false){
                update();
            }else{
                notify();
            }
        }

        virtual const _MyModifierType & getModifier() const
        {
            return modifier;
        }

        ChannelAutoModifier(const typename IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr & channel,
            const typename IChannelAutoModifier<PointType, TimeType>::_MyModifierType & modifier) : channel(channel),
            modifier(modifier), notifier(new UpdateNotifier()), changed(false)
        {
            UTILS_ASSERT((channel != nullptr), "Wrong channel for Tracker");

            if(channel == nullptr){
                throw std::runtime_error("Wrong channel for Tracker");
            }

            notifier->setModifier(this);
            channel->attach(notifier.get());

            //sprawdziæ czy s¹ jakieœ dane? jesli tak odpaliæ modifier
            if(channel->empty() == false){
                update();
            }
        }

        virtual time_type getLength() const
        {
            return RawGeneralDataChannel<PointType, TimeType>::getLength();
        }

        //! \param idx Indeks probki
        //! \return Wartosc czasu dla danego indeksu
        virtual time_type argument(size_type idx) const
        {
            return RawGeneralDataChannel<PointType, TimeType>::argument(idx);
        }

        //! \param idx Indeks probki
        //! \return Wartosc probki dla danego indeksu
        virtual point_type_const_reference value(size_type idx) const
        {
            return RawGeneralDataChannel<PointType, TimeType>::value(idx);
        }

        //! \return Iloœæ próbek w kanale
        virtual size_type size() const
        {
            return RawGeneralDataChannel<PointType, TimeType>::size();
        }

        //! \return Czy kana³ nie zawiera danych
        virtual bool empty() const
        {
            return RawGeneralDataChannel<PointType, TimeType>::empty();
        }
        //! \param time Czas dla ktorego chemy uzyskac dwie najblizsze probki
        //! \return para indeksow, pierwszy wskazujke probke o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje probke o czasie wiekszym lub rownym zadanemu
        virtual std::pair<size_type, size_type> getValueHelper(time_type time) const
        {
            return RawGeneralDataChannel<PointType, TimeType>::getValueHelper(time);
        }

    private:

        void update()
        {
            changed = false;

            modifier(*this, *channel, *this);

            if(changed == true){
                notify();
            }
        }

        virtual void addPoint(time_type time, point_type_const_reference point)
        {

            if(data.empty() == true){
                if(time != 0) {
                    throw std::runtime_error("First sample time must be 0");
                }
            }else if(time < data.back().first){
                throw std::runtime_error("Samples time must be in chronological order");
            }

            data.push_back(value_type(time, point));

            changed = true;
        }

        virtual void addPoint(point_type_const_reference point)
        {
            throw std::runtime_error("Not suppported");
            changed = true;
        }

        virtual void setIndexData(size_type idx, point_type_const_reference point)
        {
            data[idx].second = point;
            changed = true;
        }

    private:
        typename IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr channel;
        typename IChannelAutoModifier<PointType, TimeType>::_MyModifierType modifier;
        boost::shared_ptr<UpdateNotifier> notifier;
        bool changed;
    };

    template<class Channel>
    class ChannelWithDescriptor : public Channel, public ChannelDescriptor
    {
        UTILS_STATIC_ASSERT((boost::is_base_of<RawGeneralDataChannel<typename Channel::point_type, typename Channel::time_type>, Channel>::value), "Base class should inherit from RawGeneralDataChannel");
        UTILS_STATIC_ASSERT(!(boost::is_base_of<ChannelDescriptor, Channel>::value), "Do you need two descriptor bases for this channel? Already added descriptor to channel?");

    public:

        ChannelWithDescriptor() {}
        ChannelWithDescriptor(const IChannelDescriptorReader & descriptor) : ChannelDescriptor(descriptor) {}
        ChannelWithDescriptor(const Channel & channel, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(channel), ChannelDescriptor(descriptor) {}
        ChannelWithDescriptor(const ChannelWithDescriptor & channel) : Channel(channel), ChannelDescriptor(channel) {}
        ChannelWithDescriptor(float samplesPerSecond, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(samplesPerSecond), ChannelDescriptor(descriptor) {}

        ChannelWithDescriptor(const Channel & channel, boost::false_type, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Channel(channel, boost::false_type()), ChannelDescriptor(descriptor) {}
        ChannelWithDescriptor(const ChannelWithDescriptor & channel, boost::false_type) : Channel(channel, boost::false_type()), ChannelDescriptor(channel) {}

        virtual ~ChannelWithDescriptor() {}

        virtual ChannelWithDescriptor * clone() const
        {
            return new ChannelWithDescriptor(*this);
        }

        virtual ChannelWithDescriptor * create() const
        {
            return new ChannelWithDescriptor(*this, boost::false_type());
        }

    };

    template<class PointType, class TimeType, bool>
    class IChannelReader : public virtual utils::IRawGeneralDataChannelReader<PointType, TimeType>, public virtual IChannelDescriptorReader
    {
    public:
        virtual ~IChannelReader() {}

        typedef utils::RawGeneralDataChannel<PointType, TimeType> RawChannel;
        typedef utils::ChannelWithDescriptor<RawChannel> Implementation;
    };

    template<class PointType, class TimeType>
    class IChannelReader<PointType, TimeType, true> : public virtual utils::IRawUniformDataChannelReader<PointType, TimeType>, public virtual IChannelDescriptorReader
    {
    public:
        virtual ~IChannelReader() {}

        typedef utils::RawUniformDataChannel<PointType, TimeType> RawChannel;
        typedef utils::ChannelWithDescriptor<RawChannel> Implementation;
    };

    template<class PointType, class TimeType = float, bool uniform = true>
    class Channel : public IChannelReader<PointType, TimeType, uniform>, public IChannelReader<PointType, TimeType, uniform>::Implementation
    {
    public:
        typedef IChannelReader<PointType, TimeType, uniform> Interface;

    public:

        Channel() {}

        Channel(const Channel & channel) : Implementation(channel) {}
        Channel(const Channel & channel, boost::false_type) : Implementation(channel, boost::false_type()) {}

        Channel(const IChannelDescriptorReader & descriptor) : Implementation(descriptor) {}
        Channel(const RawChannel & channel, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Implementation(channel, descriptor) {}
        
        Channel(float samplesPerSecond, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Implementation(samplesPerSecond, descriptor) {}

        Channel(const RawChannel & channel, boost::false_type, const IChannelDescriptorReader & descriptor = ChannelDescriptor()) : Implementation(channel, boost::false_type(), descriptor) {}

        virtual ~Channel() {}

        virtual Channel * clone() const
        {
            return new Channel(*this);
        }

        virtual Channel * create() const
        {
            return new Channel(*this, boost::false_type());
        }

        virtual float getSamplesPerSecond() const
        {
            return Implementation::getSamplesPerSecond();
        }

        virtual float getSampleDuration() const
        {
            return Implementation::getSampleDuration();
        }
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__DATACHANNEL_H__
