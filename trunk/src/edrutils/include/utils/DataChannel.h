/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   13:57
	filename: DataChannel.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__DATACHANNEL_H__
#define __HEADER_GUARD_CORE__DATACHANNEL_H__

#include <stack>
#include <iterator>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <string>

#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
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

    //! Wzorzec wyci¹gaj¹cy const referencjê do danych - przydatny przy przekazywaniu danych w innych wzorcach
    template<class PointType>
    struct ConstReferenceType
    {
        typedef typename boost::add_reference<typename boost::add_const<PointType>::type>::type type;
    };

    //! Sposób obs³ugi zapytañ o czas spoza zakresu
    template <class PointType, class TimeType, class PointRefType = typename boost::add_reference< typename PointType>::type >
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
        void timeUnderflow(TimeType time, PointRefType point) const
        {
            throw std::runtime_error("Time value less than 0");
        }

        //! \param time Czas poza dlugoscia kanalu
        //! \param point Zwracana wartosc
        void timeOverflow(TimeType time, PointRefType point) const
        {
            throw std::runtime_error("Time value greater than channel length");
        }
    };

    //! Manipulator danych bazuj¹cy na operatorach typu. Interpoluje liniowo.
    template <class PointType, class TimeType, class PointRefType = typename boost::add_reference<typename PointType>::type >
    struct DataOperatorsManipulator
    {
        //! Typ stalej referencji do przekazywania wartosci
        typedef typename ConstReferenceType<PointType>::type PointParamType;

        bool isLower(PointParamType p1, PointParamType p2)
        {
            return p1 < p2;
        }
        PointType normalize(PointParamType p, PointParamType min, PointParamType max) const
        {
            return (p - min) / (max - min);
        }

        //! \param point Zwracana interpolowana wartosc
        //! \param p1 Probka dla czasu ponizej lub identycznego
        //! \param Probka dla czasu powyzej lub identycznego
        //! \param time Wartosc okreslajaca procent przesuniecia wzgledem probki ponizej
        void interpolate(typename PointRefType point, PointParamType p1, PointParamType p2, TimeType t) const
        {
            // lerp
            point = (p1 + (p2 - p1) * t);
        }
    };

    //! Interfejs do opisu kana³u - osi x i y oraz nazwy kana³u. Pozwala na edycjê opisu
    class IChannelDescriptor
    {
    public:
        virtual ~IChannelDescriptor() {}

        //! \return
        virtual const std::string& getXUnit() const = 0;

        //! \param xUnit
        virtual void setXUnit(const std::string& xUnit) = 0;

        //! \return
        virtual const std::string& getYUnit() const = 0;

        //! \param yUnit
        virtual void setYUnit(const std::string& yUnit) = 0;

        //! \return
        virtual const std::string& getName() const = 0;

        //! \param name
        virtual void setName(const std::string& name) = 0;
    };

    //! Formward declaration
    //! Interfejs zapewniaj¹cy dostêp do danych w kanale - po czasie i indeksie
    template<class PointType, class TimeType>
    class IBaseChannel;

    //! Klasa timera dla kana³ów - mo¿na tworzyc wiele timerów na jednym kanale, kazdy ma swój niezale¿ny czas
    template<class PointType, class TimeType = float>
    class ChannelTimer
    {
    public:
        typedef IBaseChannel<PointType, TimeType> BaseChannel;
        typedef boost::shared_ptr<const BaseChannel> BaseChannelConstPtr;

    public:
        //! \param channel Kana³ któego dotyczy timer
        ChannelTimer(const BaseChannelConstPtr & channel) : channel(channel), currentTime(0) {}
        
        //! \param timer Zegar który kopiujemy
        ChannelTimer(const ChannelTimer & timer) : channel(timer.channel), currentTime(timer.currentTime) {}
        
        //! Polimorficzny destruktor
        virtual ~ChannelTimer() {}

        //! \param time Aktualny czas zegara
        void setTime(TimeType time)
        {
            currentTime = time;
        }

        //! \return Aktualny czas zegara
        TimeType getTime() const
        {
            return currentTime;
        }

        //! \return Wartosc kana³y dla aktualnego czasu zegara
        PointType getCurrentValue()
        {
            return channel->getValue(currentTime);
        }

        //! \return Kana³ którego zegar dotyczy
        const BaseChannelConstPtr & getChannel() const
        {
            return channel;
        }

    private:
        //! Kana³ na którym bazuje zegar
        BaseChannelConstPtr channel;

        //! Aktualny czas zegara
        TimeType currentTime;
    };


    //! Klasa ze statystykami kana³u - instancja trzymana po stronie kana³u, leniwie aktualizowana w momencie zapytania
    template<class PointType, class TimeType>
    class ChannelStats
    {
    public:
        typedef IBaseChannel<PointType, TimeType> BaseChannel;
        typedef boost::shared_ptr<const BaseChannel> BaseChannelConstPtr;
        typedef boost::shared_ptr<BaseChannel> BaseChannelPtr;
        typedef typename ConstReferenceType<PointType>::type PointTypeConstReference;

        friend class BaseChannel;

    private:
        //! Klasa obserwuj¹ca - obiekt statystyk bezpoœrendio nie moze byæ obserwatore bo mo¿naby go by³o przypadkiem podpi¹æ pod kilka kana³ów!!
        class ChannelObserver : public BaseChannel::_MyObserverType
        {
        public:
            //! \see update wzorca Obserwator
            virtual void update(const BaseChannel * channel)
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
        //! Prywatny konstruktor - tylko kana³ mo¿e tworzyæ obiekty statystyk
        ChannelStats(const BaseChannelPtr & channel) : channel(channel), changed(false)
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
        const BaseChannelConstPtr & getChannel() const
        {
            return channel;
        }

    protected:
        //! Aktualizuje statystyki kana³u
        virtual void updateStats() const
        {
            using namespace boost::accumulators;
            typedef boost::accumulators::accumulator_set<PointType, features< tag::min, tag::max, tag::mean, tag::variance >> Stats;

            changed = false;
            Stats sts;

            for(auto it = channel->begin(); it != channel->end(); it++){
                sts(it->second);
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
        BaseChannelConstPtr channel;
    };

    //! Klasa do topwrzenia obserwujacych, automodyfikujacych kana³ów
    class AutoModifier;

    //! Interfejs zapewniaj¹cy dostêp do danych w kanale - po czasie i indeksie
    template<class PointType, class TimeType = float>
    class IBaseChannel : public Observable<IBaseChannel<PointType, TimeType>>, public boost::enable_shared_from_this<IBaseChannel<PointType, TimeType>>
    {

        friend class AutoModifier;

    public:
        //! Typ czasu
        typedef TimeType time_type;
        //! Typ danych
        typedef PointType point_type;
        //! Typ sta³ej referencji do danych dla przekazywania parametrów
        typedef typename ConstReferenceType<point_type>::type point_type_const_reference;

        //! Typ referencji do danych dla przekazywania parametrów
        typedef typename boost::add_reference<point_type>::type point_type_reference;

        //! Typ aktualnego interfejsu
        typedef IBaseChannel<point_type, time_type> _MyChannelInterface;

        //! Typ przechowuj¹cy dane indeksowane po czasie
        typedef std::map<time_type, point_type> TimeMappedData;

        //! Const iterator dla danych indeksowanych czasem
        typedef typename TimeMappedData::const_iterator const_iterator;
        //! Const reverse iterator dla danych indeksowanych czasem
        typedef typename TimeMappedData::const_reverse_iterator const_reverse_iterator;
        //! Typ danych o charakterze czasowym - czas -> próbka
        typedef typename TimeMappedData::value_type value_type;
        //! Const referencja do danych o charakterze czasowym
        typedef typename TimeMappedData::const_reference const_reference;
        //! Typ iloœci elementów w kanale
        typedef typename TimeMappedData::size_type size_type;

        //! Typ obserwatora dla kana³u o podanym interfejsie ( typach danych)
        typedef Observer<_MyChannelInterface> _MyObserverType;
        //! Typ dajacy siê obserwowaæ dla kana³u o podanym interfejsie ( typach danych)
        typedef Observable<_MyChannelInterface> _MyObservableType;
        //! Timer dla danego kana³u danych
        typedef ChannelTimer<point_type, time_type> Timer;
        //! Inteligetny wskaŸnik na Timer
        typedef boost::shared_ptr<Timer> TimerPtr;
    private:
        //! Typ wpisu dla danych indexowanych (nie czasem a numerem próbki o charakterze czsaowym)
        typedef const value_type* const_value_type_ptr;
        //! Typ wpisu.
        typedef std::vector<const_value_type_ptr> IndexedData;

    private:
        //! W³aœciwe dane indeksowane czasem
        TimeMappedData dataMap;

        //! Dane poindeksowane (referencje) wg hronologii
        IndexedData data;

    public:
        //! Domyslny konstruktor na potrzeby klas pochodnych
        IBaseChannel()
        {

        }

        //! Konstruktor kopiuj¹cy
        //! \param channel
        IBaseChannel(const IBaseChannel& channel) :
        dataMap(channel.begin(), channel.end())
        {
            for(auto it = dataMap.begin(); it != dataMap.end(); it++){
                data.push_back(&*it);
            }
        }

    public:

        //! Pusty polimorficzny destruktor.
        virtual ~IBaseChannel() {}

        //! Klonuje kana³ wraz z ustawieniami i danymi
        virtual IBaseChannel * clone() const
        {
            return new IBaseChannel(*this);
        }

        //! Tworzy pusty kana³ z kopia ustawieñ danego
        virtual IBaseChannel * create() const
        {
            return new IBaseChannel();
        }

        //! \return Nowy timer dla naszego kana³u
        TimerPtr createTimer() const
        {
            return TimerPtr(new Timer(shared_from_this()));
        }

        //! \param Czas dla którego pobieramy wartoœæ
        //! \return Wartoœæ kana³u dla zadanego czasu - mo¿e rzucac wyj¹tkami
        point_type getValue(time_type time) const
        {
            if(empty() == true){
                throw std::runtime_error("Value request to empty channel");
            }

            return innerGetValue(time);
        }

        //! \return Czas trwania kana³u
        time_type getLength() const
        {
            if(empty() == true){
                return 0;
            }

            return rbegin()->first;
        }

    public:

        //! \return Iloœæ próbek w kanale
        size_type size() const
        {
            return dataMap.size();
        }

        //! \return Czy kana³ nie zawiera danych
        bool empty() const
        {
            return dataMap.empty();
        }

        //! \return Czas punktu.
        //! \return Wartoœæ na podstawie punktu czasowego.
        point_type operator[](time_type time) const
        {
            return getValue(time);
        }

        //! \return Indeks punktu.
        //! \return Wartoœæ na podstawie indeksu punktu czasowego.
        const_reference operator[](size_type idx) const
        {
            return *(data[idx]);
        }

        //! \param time Czas dla ktorego poszukujemy probki
        //! \return Iterator do probki o zadanym indeksie czasowym lub iterator konca kanalu jesli nie ma takiej probki
        const_iterator find(time_type time) const
        {
            return dataMap.find(time);
        }

        //! \param time
        //! \return
        const_iterator lower_bound(time_type time) const
        {
            return dataMap.lower_bound(time);
        }

        //! \param time
        //! \return
        const_iterator upper_bound(time_type time) const
        {
            return dataMap.upper_bound(time);
        }

        //! \return
        const_iterator begin() const
        {
            return dataMap.begin();
        }
        //! \return
        const_iterator end() const
        {
            return dataMap.end();
        }
        //! \return
        const_reverse_iterator rbegin() const
        {
            return dataMap.rbegin();
        }
        //! \return
        const_reverse_iterator rend() const
        {
            return dataMap.rend();
        }
        //! \param
        //! \param
        void setTimeValue(time_type time, point_type_const_reference point)
        {
            __innerSetTimeValue(time, point);
            notify();
        }

        //! \param
        //! \param
        void setIndexValue(size_type idx, point_type_const_reference point)
        {
            __innerSetIndexValue(idx, point);
            notify();
        }

        //! \param
        //! \param
        void addPoint(time_type time, point_type_const_reference point)
        {
            __innerAddPoint(time, point);
            notify();
        }

    protected:        

        //! \param
        //! \param
        virtual bool customAddPointVerification(time_type time, point_type_const_reference point)
        {
            return true;
        }

        virtual point_type innerGetValue(time_type time) const
        {
            auto it = lower_bound(time);

            if( (time != it->first) && (it != begin()) ){
                it--;
            }

            return it->second;
        }

    private:

        //! \param
        //! \param
        void __innerSetTimeValue(time_type time, point_type_const_reference point)
        {
            auto it = find(time);
            if(it == end()){
                throw std::runtime_error("No sample with given time index");
            }

            const_cast<value_type*>(&*it)->second = point;
        }

        //! \param
        //! \param
        void __innerSetIndexValue(size_type idx, point_type_const_reference point)
        {
            if(idx >= size() || idx < 0){
                throw std::runtime_error("No sample with given chronological index");
            }

            const_cast<value_type*>(data[idx])->second = point;            
        }

        //! \param
        //! \param
        void __innerAddPoint(time_type time, point_type_const_reference point)
        {
            if(time < 0){
                throw std::runtime_error("Time negative value not allowed");
            }

            if(dataMap.empty() == true && time != 0){
                throw std::runtime_error("First sample in channel must be for time 0");
            }

            if(time < getLength()){
                throw std::runtime_error("Data not in chronological order");
            }

            if(dataMap.find(time) != dataMap.end()){
                throw std::runtime_error("Value for specified time already exists");
            }

            if(customAddPointVerification(time, point) == false){
                throw std::runtime_error("Sample insertion breaks custom channel rules");
            }

            auto ins = dataMap.insert(value_type(time, point));
            data.push_back(&*ins.first);
        }
    };

    //! Klasa generuj¹ca kana³y reagujace na zmiany innych kana³ów
    class AutoModifier
    {
        public:
            //! Interfejs pozwalajacy zmieniac dane podczas notyfikacji o zmianie obiektu obserwowanego
            template<class Channel>
            class ChannelModifierInterface
            {
            public:
                virtual void setTimeValue(typename Channel::time_type time, typename Channel::point_type_const_reference point) = 0;

                virtual void setIndexValue(typename Channel::size_type idx, typename Channel::point_type_const_reference point) = 0;

                virtual void addPoint(typename Channel::time_type time, typename Channel::point_type_const_reference point) = 0;
            };

            template<class Channel, class Modifier>
            static Channel * createAutoModChannel(Channel & channel, Modifier modifier)
            {
                return new AutoModImpl<Channel, Modifier>(channel, modifier);
            }

            template<class Channel, class Modifier>
            static void modifyChannel(Channel & channel, Modifier modifier)
            {
                ChannelModifierImplementation<Channel> modImpl(&channel);
                modifier((ChannelModifierInterface<Channel>&)(&(modImpl)), (const Channel &)mod);
                if(modImpl.wasUpdated() == true)
                {
                    channel.notify();
                }
            }

        private:
            template<class Channel>
            //! Prywatna klasa realizujaca interfejs umo¿liwiaj¹cy modyfikacje
            class ChannelModifierImplementation : public ChannelModifierInterface<Channel>
            {
            public:
                ChannelModifierImplementation() : updated(false) {}

                ~ChannelModifierImplementation() {}

                virtual void setTimeValue(typename Channel::time_type time, typename Channel::point_type_const_reference point)
                {
                    MakePublicOperations::setTimeValue(*channel, time, point);
                    updated = true;
                }

                virtual void setIndexValue(typename Channel::size_type idx, typename Channel::point_type_const_reference point)
                {
                    MakePublicOperations::setIndexValue(*channel, idx, point);
                    updated = true;
                }

                virtual void addPoint(typename Channel::time_type time, typename Channel::point_type_const_reference point)
                {
                    MakePublicOperations::addPoint(*channel, time, point);
                    updated = true;
                }

                bool wasUpdated() const
                {
                    return updated;
                }

                void resetUpdate(){
                    updated = false;
                }

                void setChannelToMod(Channel * channel)
                {
                    this->channel = channel;
                }

            private:
                bool updated;
                Channel * channel;
            };

            template<class Channel, class Modifier>
            class AutoModImpl : public Channel 
            {
            private:

            //! Prywatna klasa realizuj¹ca obserwacjê zadanej klasy. Na jej zmianê wyzwala modyfikacje
            class UpdateNotifier : public Channel::_MyObserverType
            {
            public:

                virtual void update(const typename Channel::_MyChannelInterface * channel)
                {
                    mod->fullDataUpdate();
                }

                void setModifier(AutoModImpl* mod)
                {
                    this->mod = mod;
                }

            private:
                AutoModImpl* mod;
            };

            friend class UpdateNotifier;

            private:
                Modifier modifier;
                UpdateNotifier notifier;
                ChannelModifierImplementation<Channel> modifierImplementation;
                Channel * observedChannel;

            public:
                AutoModImpl(Channel & channel, Modifier modifier)
                    : Channel(channel), modifier(modifier), observedChannel(&channel)
            {
                modifierImplementation.setChannelToMod(this);
                notifier.setModifier(this);
                channel.attach(&notifier);

                if(channel.empty() == false){
                    refreshData();
                }
            }

            private:

                void refreshData()
                {
                    modifier((ChannelModifierInterface<Channel>&)modifierImplementation, (const Channel &)*this, (const Channel &)*observedChannel);
                }

                void fullDataUpdate()
                {
                    refreshData();
                    if(modifierImplementation.wasUpdated() == true)
                    {
                        modifierImplementation.resetUpdate();
                        notify();
                    }
                }
            };

            //! Pozwala modyfikowaæ dane indeksowane czasem
        template<class Channel>
        static void setTimeValue(Channel & channel, typename Channel::time_type time, typename Channel::point_type_const_reference point)
        {
            channel.__innerSetTimeValue(time, point);
        }

        //! Pozwala modyfikowaæ dane indeksowane kolejnoœci¹ próbek
        template<class Channel>
        static void setIndexValue(Channel & channel, typename Channel::size_type idx, typename Channel::point_type_const_reference point)
        {
            channel.__innerSetIndexValue(idx, point);
        }

        //! Pozwala dodawaæ punkty do kana³u
        template<class Channel>
        static void addPoint(Channel & channel, typename Channel::time_type time, typename Channel::point_type_const_reference point)
        {
            channel.__innerAddPoint(time, point);
        }

        class MakePublicOperations
        {
        public:
            //! Pozwala modyfikowaæ dane indeksowane czasem
            template<class Channel>
            static void setTimeValue(Channel & channel, typename Channel::time_type time, typename Channel::point_type_const_reference point)
            {
                AutoModifier::setTimeValue(channel, time, point);
            }

            //! Pozwala modyfikowaæ dane indeksowane kolejnoœci¹ próbek
            template<class Channel>
            static void setIndexValue(Channel & channel, typename Channel::size_type idx, typename Channel::point_type_const_reference point)
            {
                AutoModifier::setIndexValue(channel, idx, point);
            }

            //! Pozwala dodawaæ punkty do kana³u
            template<class Channel>
            static void addPoint(Channel & channel, typename Channel::time_type time, typename Channel::point_type_const_reference point)
            {
                AutoModifier::addPoint(channel, time, point);
            }
        };

        friend class MakePublicOperations;
    };


    //! Kana³ z danymi dyskretnymi - dane nie s¹ interpolowane. Zapytanie o dok³adn¹ próbke czasow¹ kiedy ona nie wystepuje skutkuje wyj¹tkiem
    //! Obs³uguje zapytania o czas spoza zakresu. Zapytania o czas wspieraj¹ obszar który jest rozpatrywany w przypadku braku próbki - zwracana jest najbli¿sza w przedziale.
    //! W przypadku braku wyj¹tek. 
    template<class PointType, class TimeType = float, class TimeOverflowResolver = ExceptionTimeOverflowResolver<PointType, TimeType>>
    class DiscreteDataChannel : public IBaseChannel<PointType, TimeType>, public TimeOverflowResolver
    {
    public:
        //! Mój typ bazowy
        typedef DiscreteDataChannel<PointType, TimeType, TimeOverflowResolver> _MyChannelType;

    private:
        //! Domyœlne otoczenie wokó³ którego szukamy próbek
        time_type timeEpsilon;
        //! Czy jeœli nie znaleziono odpowiadaj¹cej próbki i ta po lewej i po prawej maj¹ tak¹ sam¹ odleg³oœæ to wybraæ mniejsz¹
        bool smallerIfEqual;

    public:
        //! \param timeEpsilon Domyœlny przedzia³ przeszukiwania dla zapytañ czasowych
        //! \param smallerIfEqual Polityka wyboru próbki jesli nie znaleziono w³aœciwej a odleg³oœæ po lewej i prawej stronie jest identyczna
        DiscreteDataChannel(time_type timeEpsilon = 0, bool smallerIfEqual = true)
            : timeEpsilon(timeEpsilon), smallerIfEqual(smallerIfEqual)
        {

        }

        //! Konstruktor kopiuj¹cy
        //! \param channel
        //! \param timeEpsilon
        //! \param smallerIfEqual
        DiscreteDataChannel(const _MyChannelInterface& channel, time_type timeEpsilon = 0, bool smallerIfEqual = true) :
          _MyChannelInterface(channel), timeEpsilon(timeEpsilon), smallerIfEqual(smallerIfEqual)
        {
            
        }

          //! Konstruktor kopiuj¹cy
          //! \param channel
          DiscreteDataChannel(const DiscreteDataChannel& channel) : _MyChannelInterface(channel),
              timeEpsilon(channel.timeEpsilon), smallerIfEqual(channel.smallerIfEqual)
          {
             
          }

          //! Pusty polimorficzny destruktor.
          virtual ~DiscreteDataChannel() {}

    public:

        //! Klonuje kana³.
        virtual DiscreteDataChannel* clone() const
        {
            return new DiscreteDataChannel(*this);
        }

        //! Tworzy pusty kana³ z ustawieniami twórcy
        virtual DiscreteDataChannel* create() const
        {
            return new DiscreteDataChannel(timeEpsilon, smallerIfEqual);
        }

        //! \return Domyœlny epsilon dla czasu
        time_type getTimeEpsilon() const
        {
            return timeEpsilon;
        }

        //! \param
        void setTimeEpsilon(time_type timeEpsilon)
        {
            this->timeEpsilon = timeEpsilon;
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

        using _MyChannelInterface::getValue;

        //! \param
        //! \param
        //! \param
        point_type getValue(time_type time, time_type epsilon, bool smallerIfEqual = true) const
        {
            UTILS_ASSERT((epsilon >= 0), "Nieprawid³owy epsilon - musi byæ wiêkszy lub równy 0");

            if(empty() == true){
                throw std::runtime_error("Value request to empty channel");
            }
            
            return __getValue(time, epsilon, smallerIfEqual);
        }

    protected:
        //! \param time
        //! \return Zinterpolowana wartoœæ dla zadanego czasu.
        virtual point_type innerGetValue(time_type time) const
        {   
            return __getValue(time, timeEpsilon, smallerIfEqual);
        }

    private:
        point_type __getValue(time_type time, time_type epsilon, bool smallerIfEqual = false) const
        {
            bool pointSet = false;
            point_type ret;

            if ( time < 0 ) {
                if(time >= -epsilon){
                    pointSet = true;
                    ret = (*this)[0u].second;
                }else{
                    TimeOverflowResolver::timeUnderflow(time, ret);
                    pointSet = true;
                }
            }else if ( time > getLength() ) {
                if(time - epsilon <= getLength()){
                    pointSet = true;
                    ret = rbegin()->second;
                }else{
                    TimeOverflowResolver::timeOverflow(time, ret);
                    pointSet = true;
                }
            }else{
                auto it = find(time);
                if(it == end()){
                    it = lower_bound(time);
                    // Czy wartoœæ mniejsza od pierwszej próbki
                    if(it == begin()){
                        if(it->first - epsilon <= time){
                            pointSet = true;
                            ret = it->second;
                        }
                        // Wartoœæ gdzieœ wewn¹trz kana³u - istnieje próbka poprzednia
                    }else{
                        //probka poprzednia, mniejszy czas
                        auto iT = it;
                        iT--;

                        auto closerIt = iT;

                        if( (it->first - time) < (time - iT->first) || ( (it->first - time) == (time - iT->first) && smallerIfEqual == false) ){
                            closerIt = it;
                        }

                        if(std::fabs(closerIt->first - time) <= epsilon){
                            pointSet = true;
                            ret = closerIt->second;
                        }
                    }
                }else{
                    pointSet = true;
                    ret = it->second;
                }
            }

            if(pointSet == false){
                throw std::runtime_error("No sample for specified time and surrounding");
            }

            return ret;
        }
    };

    //! Kana³ z danymi ci¹g³ymi - wspiera interpolacjê za pomoca klasy Manipulator. Manipulator powinien umo¿liwiæ poprawne zapytanie o czas z zakrewsu [0, length] kana³u
    //! Obs³uguje interpolacjê danych oraz obs³uge czasu spoza zakresu.
    template<class PointType, class TimeType = float,
        class Manipulator = DataOperatorsManipulator<PointType, TimeType>, class TimeOverflowResolver = ExceptionTimeOverflowResolver<PointType, TimeType>>
    class ContiniousDataChannel : public IBaseChannel<PointType, TimeType>, public TimeOverflowResolver, public Manipulator
    {
    public:
        //! Mój typ kana³u
        typedef ContiniousDataChannel<PointType, TimeType, Manipulator, TimeOverflowResolver> _MyChannelType;        

    public:
        
        ContiniousDataChannel()
        {

        }
        
        //! Konstruktor kopiuj¹cy
        //! \param channel
        ContiniousDataChannel(const _MyChannelInterface& channel) : _MyChannelInterface(channel)
        {
            
        }

        //! Konstruktor kopiuj¹cy
        //! \param channel
        ContiniousDataChannel(const ContiniousDataChannel& channel) : _MyChannelInterface(channel),
            TimeOverflowResolver(channel), Manipulator(channel)
        {
            
        }

        //! Pusty polimorficzny destruktor.
        virtual ~ContiniousDataChannel() {}

    public:

        //! Klonuje kana³.
        virtual ContiniousDataChannel* clone() const
        {
            return new ContiniousDataChannel(*this);
        }

        //! Tworzy pusty kana³ z ustawieniami twórcy
        virtual ContiniousDataChannel* create() const
        {
            return new ContiniousDataChannel();
        }
        
    protected:
        //! \param time
        //! \return Zinterpolowana wartoœæ dla zadanego czasu.
        virtual point_type innerGetValue(time_type time) const
        {   
            point_type ret;
            if ( time < 0 ) {
                TimeOverflowResolver::timeUnderflow(time, ret);
            }else if ( time > getLength() ) {
                TimeOverflowResolver::timeOverflow(time, ret);
            }else{
                auto points = getValueHelper(time);
                if(&(points.first) == &(points.second)){
                    ret = points.first.second;
                }else{
                    Manipulator::interpolate(ret, points.first.second, points.second.second, (time - points.first.first) / (points.second.first - points.first.first) );
                }
            }

            return ret;
        }

        //! \return Para punktów pomiarowych pomiêdzy którymi nale¿y wykonaæ interpolacjê.
        virtual std::pair<const_reference, const_reference> getValueHelper(time_type time) const
        {            
            auto it = find(time);
            auto iT = it;

            if(it == end()){
                it = lower_bound(time);
                iT = it;
                it--;
            }

            return std::pair<const_reference, const_reference>(*it, *iT);
        }            
    };

    template<class Channel>
    class UniformTimeDataChannel : public Channel
    {

    public:

        typedef typename Channel::time_type time_type;
        typedef typename Channel::point_type point_type;
        typedef typename Channel::point_type_const_reference point_type_const_reference;
        typedef typename Channel::const_reference const_reference;

        typedef UniformTimeDataChannel<Channel> _MyChannelType;
        typedef typename Channel::_MyChannelType _MyBaseChannelType;

    private:

        const time_type samplesPerSecond;
        time_type samplesPerSecondInv;

    public:
        //! \param samplesPerSec Liczba próbek na sekundê.
        UniformTimeDataChannel(time_type samplesPerSecond = 25)
            : samplesPerSecond(samplesPerSecond)
        {
            if(samplesPerSecond <= 0){
                throw std::runtime_error("Incorrect samples per second value");
            }

            samplesPerSecondInv = 1.0 / samplesPerSecond;
        }

        //! Konstruktor kopiuj¹cy
        //! \param channel
        UniformTimeDataChannel(const UniformTimeDataChannel& channel) :
        Channel(channel), samplesPerSecond(channel.samplesPerSecond),
            samplesPerSecondInv(channel.samplesPerSecondInv)
        {

        }

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~UniformTimeDataChannel() {}

        //! \return Klon obiektu
        virtual UniformTimeDataChannel* clone() const
        {
            return new UniformTimeDataChannel(*this);
        }

        //! Tworzy pusty kana³ z ustawieniami twórcy
        virtual UniformTimeDataChannel* create() const
        {
            return new UniformTimeDataChannel(samplesPerSecond);
        }

        //! \return Iloœc próbek na sekundê
        time_type getSamplesPerSecond() const
        {
            return samplesPerSecond;
        }

        //! \return Czas trwania jednej próbki
        time_type getSampleDuration() const
        {
            return samplesPerSecondInv;
        }

        using Channel::addPoint;

        void addPoint(point_type_const_reference point)
        {
            time_type t = getLength();

            if(empty() == false){
                t += samplesPerSecondInv;
            }

            Channel::addPoint(t, point);
        }

    protected:
        virtual bool customAddPointVerification(time_type time, point_type_const_reference point)
        {
            time_type nextTime = getLength() + samplesPerSecondInv;
            if( (time != 0) && (time != nextTime)){
                throw std::runtime_error("Time breaks channel resolution");
            }

            return true;
        }

        //! \return Para punktów pomiarowych pomiêdzy którymi nale¿y wykonaæ interpolacjê.
        virtual std::pair<const_reference, const_reference> getValueHelper(time_type time) const
        {
            // wyznaczenie indeksów
            size_type idx = static_cast<size_type>(time * samplesPerSecond);
            UTILS_ASSERT( idx < size() );

            // pobranie punktów
            const_reference p1 = operator[](idx);
            const_reference p2 = (idx == size()-1) ? p1 : operator[](idx+1);
            return std::pair<const_reference, const_reference>(p1, p2);
        }
    };

    //! Implementacja interfejsu IChannelDescriptor
    class ChannelDescriptor
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
        //! Nazwa kana³u
        std::string name;

    public:

        ChannelDescriptor() : timeScaleFactor(1.0f), valueScaleFactor(1.0f) {}

        ChannelDescriptor(const std::string & name, const std::string & valueBaseUnit,
            float valueScaleFactor = 1.0f, float timeScaleFactor = 1.0f,
            const std::string & timeBaseUnit = std::string("s"))
            : name(name), valueBaseUnit(valueBaseUnit), valueScaleFactor(valueScaleFactor),
            timeScaleFactor(timeScaleFactor), timeBaseUnit(timeBaseUnit)
        {

        }

        ChannelDescriptor(const ChannelDescriptor & channel)
            : name(channel.name), valueBaseUnit(channel.valueBaseUnit),
            valueScaleFactor(channel.valueScaleFactor),
            timeScaleFactor(channel.timeScaleFactor),
            timeBaseUnit(channel.timeBaseUnit)
        {

        }

    public:

        virtual ~ChannelDescriptor() {}

        //! \return
        const std::string& getTimeBaseUnit() const
        { 
            return timeBaseUnit;
        }

        //! \return
        float getTimeScaleFacotr() const
        { 
            return timeScaleFactor;
        }

        //! \return
        const std::string& getValueBaseUnit() const
        { 
            return valueBaseUnit;
        }

        //! \return
        float getValueScaleFacotr() const
        { 
            return valueScaleFactor;
        }

        //! \return
        const std::string& getName() const
        { 
            return name;
        }

        //! \param timeBaseUnit
        void setTimeBaseUnit(const std::string& timeBaseUnit) 
        { 
            this->timeBaseUnit = timeBaseUnit;
        }

        //! \param timeBaseUnit
        void setTimeScaleFactor(float timeScaleFactor) 
        { 
            this->timeScaleFactor = timeScaleFactor;
        }

        //! \param yUnit
        void setValueBaseUnit(const std::string& valueBaseUnit) 
        { 
            this->valueBaseUnit = valueBaseUnit;
        }

        //! \param timeBaseUnit
        void setValueScaleFactor(float valueScaleFactor) 
        { 
            this->valueScaleFactor = valueScaleFactor;
        }

        //! \param name
        void setName(const std::string& name) 
        { 
            this->name = name;
        }
    };

    template<class PointType, class TimeType = float, class Manipulator = utils::DataOperatorsManipulator<PointType, TimeType>>
    class BaseChannel : public utils::UniformTimeDataChannel<utils::ContiniousDataChannel<PointType, TimeType, Manipulator>>, public utils::ChannelDescriptor
    {
    public:
        BaseChannel(time_type samplesPerSecond = 25, const ChannelDescriptor & descriptor = ChannelDescriptor()) : utils::UniformTimeDataChannel<utils::ContiniousDataChannel<PointType, TimeType, Manipulator>>(samplesPerSecond),
            ChannelDescriptor(descriptor) {}

        BaseChannel(const BaseChannel & channel) : utils::UniformTimeDataChannel<utils::ContiniousDataChannel<PointType, TimeType, Manipulator>>(channel), ChannelDescriptor(channel) {}

        virtual ~BaseChannel() {}

        virtual BaseChannel * clone() const
        {
            return new BaseChannel(*this);
        }

        virtual BaseChannel * create() const
        {
            return new BaseChannel(getSamplesPerSecond(), *this);
        }
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__DATACHANNEL_H__
