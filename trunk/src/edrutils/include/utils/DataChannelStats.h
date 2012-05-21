/********************************************************************
	created:	2011/12/10
	created:	10:12:2011   19:28
	filename: 	DataChannelStats.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__DATACHANNELSTATS_H__
#define HEADER_GUARD_UTILS__DATACHANNELSTATS_H__

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/mpl/always.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/type_traits.hpp>
#include <utils/DataChannel.h>

// Implementacje dodatkowych akumulatorow 
namespace boost {
	
	namespace accumulators
	{

    namespace impl
    {
        //! Implementacja akumulatora wyszukujacego minimum oraz indeks, dla ktorego minimum wystapilo
        template <typename Sample>
        struct min_with_idex_impl
            : accumulator_base
        {
            //! zwracany typ - para : minimum, index
            typedef std::pair<Sample, std::size_t> result_type;

            template<typename Args>
            min_with_idex_impl(Args const &args) :
            minimum(numeric::as_max(args[sample | Sample()])),
                count(0), index(0)
            {
            }

            //! wywolywane po dadanu kolejnej probki
            template<typename Args>
            void operator ()(Args const &args)
            {
                boost::add_reference<const Sample>::type current = args[sample];
                if (current < minimum) {
                    minimum = current;
                    index = count;
                }
                ++count;
            }


            //! Odpytanie o aktualny stan akumulatora
            //! \param  brak
            //! \return para zawierajaca minimum i index dla niego
            result_type result(dont_care) const
            {
                return std::make_pair(this->minimum, this->index);
            }

        private:
            //! aktualne minimum dla podanych probek
            Sample minimum;
            //! indeks, dla ktorego wystapilo aktualne minimum
            std::size_t index;
            //! liczba podanych probek (potrzebne by wyliczyc indeks)
            std::size_t count;
        };

        //! Implementacja akumulatora wyszukujacego maksimum oraz indeks, dla ktorego maksimum wystapilo
        //! implementacja analogiczna do min_with_index_impl
        template <typename Sample>
        struct max_with_idex_impl
            : accumulator_base
        {
            typedef std::pair<Sample, std::size_t> result_type;

            template<typename Args>
            max_with_idex_impl(Args const &args) :
            maximum(numeric::as_min(args[sample | Sample()])),
                count(0), index(0)
            {
            }

            template<typename Args>
            void operator ()(Args const &args)
            {
                boost::add_reference<const Sample>::type current = args[sample];
                if (current > maximum) {
                    maximum = current;
                    index = count;
                }
                ++count;
            }

            result_type result(dont_care) const
            {
                return std::make_pair(this->maximum, this->index);
            }

        private:
            Sample maximum;
            std::size_t index;
            std::size_t count;
        };

    } // namespace impl

    // tagi, pozwalajace umieszczac akumulatory w accumulator_set
    namespace tag
    {
        //! tag dla akumulatora minimum z indeksem
        struct min_with_index
            : depends_on<>
        {
            typedef accumulators::impl::min_with_idex_impl<mpl::_1> impl;
        };

        //! tag dla akumulatora maksimum z indeksem
        struct max_with_index
            : depends_on<>
        {
            typedef accumulators::impl::max_with_idex_impl<mpl::_1> impl;
        };
    }

    // ekstraktory, umozliwiaja odpytanie o statystyki
    namespace extract
    {
        extractor<tag::max_with_index> const max_with_index = {};
        extractor<tag::min_with_index> const min_with_index = {};

        BOOST_ACCUMULATORS_IGNORE_GLOBAL(max_with_index)
        BOOST_ACCUMULATORS_IGNORE_GLOBAL(min_with_index)
    }

    using extract::max_with_index;
    using extract::min_with_index;

}} // namespace boost::accumulators

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Klasa ze statystykami kana�u, leniwie aktualizowana w momencie zapytania
template<class PointType, class TimeType>
class ChannelStats
{
public:
    typedef IRawGeneralDataChannelReader<PointType, TimeType> Channel;
    typedef typename Channel::_MyChannelConstPtr ChannelConstPtr;
    typedef typename Channel::_MyChannelPtr ChannelPtr;
    typedef typename Channel::point_type_const_reference PointTypeConstReference;
    typedef std::pair<PointType, std::size_t> extremumPair;

private:
    //! Klasa obserwuj�ca - obiekt statystyk bezpo�rendio nie moze by� obserwatore bo mo�naby go by�o przypadkiem podpi�� pod kilka kana��w!!
    class ChannelObserver : public Channel::_MyObserverType
    {
    public:
        //! \see update wzorca Obserwator
        virtual void update(const Channel * channel)
        {
            stats->changed = true;
        }

        //! \param Obiekt statystyk aktualizowany na zmian� danych kana�u
        void setChannelStats(ChannelStats * stats)
        {
            this->stats = stats;
        }

    private:
        ChannelStats* stats;
    };

public:

    ChannelStats(const ChannelPtr & channel) : 
      useDefinedTimes(false),
      definedFrom(0),
      definedTo(channel->getLength())
    {
        init(channel);
    }

    ChannelStats(const ChannelPtr& channel, const TimeType& from, const TimeType& to) : 
        useDefinedTimes(true),
        definedFrom(from),
        definedTo(to)
    {
        init(channel);
    }
    virtual ~ChannelStats() {}

    void setDefinedTimes(const TimeType& from, const TimeType& to)
    {
        definedFrom = from;
        definedTo = to;
        changed = true;
    }

    //! \return Srednia warto� danych kana�u
    PointTypeConstReference meanValue() const
    {
        if(changed == true){
            updateStats();
        }

        return meanVal;
    }

    //! \return Minimalna warto�� danych kana�u
    extremumPair minimum() const
    {
        if(changed == true){
            updateStats();
        }

        return minVal;
    }

    //! \return Maksymalna warto�� danych kana�u
    extremumPair maximum() const
    {
        if(changed == true){
            updateStats();
        }

        return maxVal;
    }

    //! \return Minimalna warto�� danych kana�u
    PointType minValue() const
    {
        return minimum().first;
    }

    //! \return Maksymalna warto�� danych kana�u
    PointType maxValue() const
    {
        return maximum().first;
    }

    //! \return Indeks dla minimalnej wartosci danych kanalu
    std::size_t minIndex() const
    {
        return minimum().second;
    }

    //! \return Indeks dla maksymalnej wartosci danych kanalu
    std::size_t maxIndex() const
    {
        return maximum().second;
    }

    //! \return Argument dla minimalnej wartosci danych kanalu
    PointType minArgument() const
    {
        return constChannel->argument(minimum().second);
    }

    //! \return Argument dla maksymalnej wartosci danych kanalu
    PointType maxArgument() const
    {
        return constChannel->argument(maximum().second);
    }

   
    //! \return Variancja danych kana�u
    PointTypeConstReference varianceValue() const
    {
        if(changed == true){
            updateStats();
        }

        return varianceVal;
    }

    //! \return Kana� kt�rego zegar dotyczy
    const ChannelPtr & getChannel()
    {
        return channel;
    }

    //! \return Kana� kt�rego zegar dotyczy
    const ChannelConstPtr & getChannel() const
    {
        return constChannel;
    }

    //! \return czas, od ktorego liczone sa statystyki
    TimeType getDefinedFrom() const 
    { 
        return definedFrom; 
    }

    //! \return czas, do ktorego liczone sa statystyki
    TimeType getDefinedTo() const 
    { 
        return definedTo; 
    }

    const std::string& getTimeUnit() const
    {
        UTILS_ASSERT(channel);
        IChannelDescriptorReader* reader = dynamic_cast<IChannelDescriptorReader*>(channel.get());
        if (reader) {
            return reader->getTimeBaseUnit();
        } else {
            static std::string dummy = "NA";
            return dummy;
        }
    }

    const std::string& getValueUnit() const 
    {
        UTILS_ASSERT(channel);
        IChannelDescriptorReader* reader = dynamic_cast<IChannelDescriptorReader*>(channel.get());
        if (reader) {
            return reader->getValueBaseUnit(); 
        } else {
            static std::string dummy = "NA";
            return dummy;
        }
    }

protected:
    //! Aktualizuje statystyki kana�u
    void updateStats() const
    {
        using namespace boost::accumulators;
        typedef boost::accumulators::accumulator_set<PointType, features< tag::min_with_index, tag::max_with_index, tag::mean, tag::variance >> Stats;

        changed = false;
        Stats sts;

        size_t from;
        size_t to;
        
        if (useDefinedTimes) {
            // w jaki sposob brac probki?
            from = channel->getValueHelper(definedFrom).first;
            to = channel->getValueHelper(definedTo).second;
        } else {
            from = 0;
            to = channel->size();
        }
        

        for(auto i = from; i < to; i++){
            float val = channel->value(i);
            sts(channel->value(i));
        }

        meanVal = mean(sts);
        minVal = min_with_index(sts);
        maxVal = max_with_index(sts);
        varianceVal = variance(sts);
    }

    void init( const ChannelPtr & channel ) 
    {
        this->channel = channel; 
        this->constChannel = channel; 
        this->changed = false;

        if(channel == nullptr){
            throw std::runtime_error("Invalid channel for statistics");
        }

        if(channel->empty() == false){
            changed = true;
        }

        observer.setChannelStats(this);
        channel->attach(&observer);
    }

private:
    //! �rednia warto�� danych w kanale
    mutable PointType meanVal;
    //! Minimalna warto�� danych w kanale
    mutable extremumPair minVal;
    //! Maksymalna warto�� danych w kanale
    mutable extremumPair maxVal;
    //! Wariancja danych w kanale
    mutable PointType varianceVal;

    //! Czy kana� uleg� zmianie
    mutable bool changed;

    bool useDefinedTimes;
    TimeType definedFrom;
    TimeType definedTo;
    //! Obserwator zmian kana�u
    ChannelObserver observer;
    //! Obserwowany kana�
    ChannelConstPtr constChannel;

    ChannelPtr channel;
};

////////////////////////////////////////////////////////////////////////////////
} //namespace utils 
////////////////////////////////////////////////////////////////////////////////

#endif
