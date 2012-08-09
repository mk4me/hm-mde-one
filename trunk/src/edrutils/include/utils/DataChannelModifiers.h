/********************************************************************
	created:	2011/12/13
	created:	13:12:2011   13:19
	filename: 	DataChannelModifiers.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__DATACHANNELMODIFIERS_H__
#define HEADER_GUARD_UTILS__DATACHANNELMODIFIERS_H__

#include <utils/DataChannel.h>
#include <utils/DataChannelDescriptors.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Interfejs obiektu obserwuj¹cego kana³, udaj¹cego kana³, wykonuj¹cego zadane operacje na kanale
template<class PointType, class TimeType>
class IChannelAutoModifier : public IChannelReader<PointType, TimeType, true>
{
public:
    typedef typename IChannelReader<PointType, TimeType, true>::_MyChannelPtr _ChannelPtr;
    typedef typename IChannelReader<PointType, TimeType, true>::_MyChannelConstPtr _ChannelConstPtr;
    typedef typename IChannelReader<PointType, TimeType, true>::_MyModifierType _ModifierType;

    virtual ~IChannelAutoModifier() {}

    virtual _ChannelPtr getObservedChannel() = 0;

    virtual _ChannelConstPtr getObservedChannel() const = 0;

    virtual void setObservedChannel(const _ChannelPtr & channel) = 0;

    virtual const _ModifierType & getModifier() const = 0;

    // Metody pochodza posrednio z IChannelReader, ich implementacja troche burzy koncept interfejsu,
    // z drugiej zas strony pokryje to 90% zastosowan i bedzie duzo bardziej
    // IChannelDescriptorReader
    virtual const std::string& getTimeBaseUnit() const { return getDesc()->getTimeBaseUnit(); }
    virtual float getTimeScaleFactor() const { return getDesc()->getTimeScaleFactor(); }
    virtual const std::string& getValueBaseUnit() const { return getDesc()->getValueBaseUnit(); }
    virtual float getValueScaleFactor() const { return getDesc()->getValueScaleFactor(); }

    virtual float getSamplesPerSecond() const { return getUniform()->getSamplesPerSecond(); }
    virtual float getSampleDuration() const { return getUniform()->getSampleDuration(); }


private:
    template<class T>
    const T* getReader() const
    {
        const T* reader =  dynamic_cast<const T*>(getObservedChannel().get());
        if (reader) {
            return reader;
        }
        // rev
        throw std::bad_cast();
        //throw std::bad_cast("Channel does not implement all necessary interfaces");
    }

    const IChannelDescriptorReader* getDesc() const { return getReader<IChannelDescriptorReader>(); }
    const IRawUniformDataChannelReader<PointType, TimeType>* getUniform() const { return getReader<IRawUniformDataChannelReader<PointType, TimeType>>(); }
};

template<class PointType, class TimeType>
class ChannelAutoModifier : public IChannelAutoModifier<PointType, TimeType>, private IRawDataChannelExtendedWriter<PointType, TimeType>
{

private:
    typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyObserverType _MyObserverType;
    //! Prywatna klasa realizuj¹ca obserwacjê zadanej klasy. Na jej zmianê wyzwala modyfikacje
    class UpdateNotifier : public _MyObserverType
    {
    public:
        typedef typename IRawGeneralDataChannelReader<PointType, TimeType>::_MyRawChannelReaderType _RawChannelReaderType;
        virtual void update(const _RawChannelReaderType * channel)
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
    typedef typename IChannelReader<PointType, TimeType, true>::_MyChannelPtr _ChannelPtr;
    typedef typename IChannelReader<PointType, TimeType, true>::_MyChannelConstPtr _ChannelConstPtr;
    typedef typename IChannelReader<PointType, TimeType, true>::_MyModifierType _ModifierType;
    typedef typename IChannelReader<PointType, TimeType, true>::time_type time_type;
    typedef typename IChannelReader<PointType, TimeType, true>::size_type size_type;
    typedef typename IChannelReader<PointType, TimeType, true>::point_type point_type;
    typedef typename IChannelReader<PointType, TimeType, true>::point_type_const_reference point_type_const_reference;
    typedef typename IChannelReader<PointType, TimeType, true>::data_range data_range;


    virtual ~ChannelAutoModifier() {}

    virtual _ChannelPtr getObservedChannel()
    {
        return channel;
    }

    virtual _ChannelConstPtr getObservedChannel() const
    {
        return channel;
    }

    virtual void setObservedChannel(const _ChannelPtr & channel)
    {
        if(channel == nullptr){
            throw std::runtime_error("Wrong channel for Tracker");
        }

        impl.storage.clear();

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

    virtual const _ModifierType & getModifier() const
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

	virtual ChannelAutoModifier * clone() const
	{
		return nullptr;
	}

	virtual const std::string & getName() const
	{
		return impl.getName();
	}

	void setName(const std::string & name)
	{
		impl.setName(name);
	}

    virtual time_type getLength() const
    {
        return impl.getLength();
    }

    //! \param idx Indeks probki
    //! \return Wartosc czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return impl.argument(idx);
    }

    //! \param idx Indeks probki
    //! \return Wartosc probki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return impl.value(idx);
    }

    //! \return Iloœæ próbek w kanale
    virtual size_type size() const
    {
        return impl.size();
    }

    //! \return Czy kana³ nie zawiera danych
    virtual bool empty() const
    {
        return impl.empty();
    }
    //! \param time Czas dla ktorego chemy uzyskac dwie najblizsze probki
    //! \return para indeksow, pierwszy wskazujke probke o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje probke o czasie wiekszym lub rownym zadanemu
    virtual data_range getValueHelper(time_type time) const
    {
        return impl.getValueHelper(time);
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

        if(impl.empty() == true){
            if(time != 0) {
                throw std::runtime_error("First sample time must be 0");
            }
        }else if(time < impl.storage.back().first){
            throw std::runtime_error("Samples time must be in chronological order");
        }

        impl.storage.addPoint(time, point);

        changed = true;
    }

    virtual void addPoint(point_type_const_reference point)
    {
        throw std::runtime_error("Not suppported");
        changed = true;
    }

    virtual void setIndexData(size_type idx, point_type_const_reference point)
    {
        impl.storage.setPoint(idx, point);
        changed = true;
    }

private:
    typename IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr channel;
    typename IChannelAutoModifier<PointType, TimeType>::_MyModifierType modifier;
    boost::shared_ptr<UpdateNotifier> notifier;
    bool changed;
	RawGeneralDataChannel<PointType, TimeType> impl;
};
////////////////////////////////////////////////////////////////////////////////
} // namespace
////////////////////////////////////////////////////////////////////////////////
#endif
