#include "hmmPCH.h"
#include "EMGFilter.h"
#include <plugins/c3d/C3DChannels.h>
#include <plugins/newChart/INewChartSerie.h>

template<class PointType, class TimeType>
class ChannelNoCopyModifier : public utils::IChannelAutoModifier<PointType, TimeType>
{
public:
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr _MyChannelPtr;
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::_MyChannelConstPtr _MyChannelConstPtr;
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::_MyModifierType _MyModifierType;
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::time_type time_type;
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::point_type point_type;
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::size_type size_type;
    typedef typename utils::IChannelAutoModifier<PointType, TimeType>::point_type_const_reference point_type_const_reference;


    ChannelNoCopyModifier(const typename utils::IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr & channel) :
      channel(channel)
    {
        UTILS_ASSERT((channel != nullptr), "Wrong channel for Tracker");

        if(channel == nullptr){
            throw std::runtime_error("Wrong channel for Tracker");
        }
    }
    virtual ~ChannelNoCopyModifier() {}

	virtual ChannelNoCopyModifier * clone() const
	{
		return nullptr;
	}

    virtual const std::string& getName() const {
        return channel->getName();
    }

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

        //this->channel->detach(notifier.get());
        this->channel = channel;
        //channel->attach(notifier.get());
    }

    virtual time_type getLength() const
    {
        return channel->getLength();
    }

    //! \param idx Indeks próbki
    //! \return Wartość czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return channel->argument(idx);
    }

    //! \param idx Indeks próbki
    //! \return Wartość próbki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return modify(channel->value(idx));
    }

    //! \return Ilość próbek w kanale
    virtual size_type size() const
    {
        return channel->size();
    }

    //! \return Czy kanał nie zawiera danych
    virtual bool empty() const
    {
        return channel->empty();
    }
    //! \param time Czas dla którego chemy uzyskac dwie najbliższe próbki
    //! \return para indeksów, pierwszy wskazujke próbkę o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje próbkę o czasie wiekszym lub rownym zadanemu
    virtual std::pair<size_type, size_type> getValueHelper(time_type time) const
    {
        return channel->getValueHelper(time);
    }

    virtual point_type_const_reference modify(point_type_const_reference point) const = 0;
    virtual void initialize() = 0;

    virtual const _MyModifierType & getModifier() const
    {
        UTILS_ASSERT(false);
        static auto m = _MyModifierType();
        return m;
    }

protected:
    _MyChannelPtr channel;
};

//class ScalarChannelAbs
//{
//public:
//
//    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
//        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
//        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
//    {
//        //uzupełnij brakujące prboki
//        if(myChannel.size() < observedChannel.size()){
//            for(auto idx = myChannel.size(); idx < observedChannel.size(); idx++){
//                modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
//            }
//        }
//
//        ScalarChannelReaderInterface::point_type sum = 0;
//        //średnia
//        for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); idx++){
//            sum += observedChannel.value(idx);
//        }
//
//        ScalarChannelReaderInterface::point_type mean = sum / myChannel.size();
//        //aktualizacja próbek
//        for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); idx++){
//            auto point = observedChannel.value(idx);
//            modifierInterface.setIndexData(idx, point > mean ? point : -point);
//        }
//    }
//};

class RMSModifier
{
public:
    typedef ScalarChannelReaderInterface::point_type pointT;
    typedef ScalarChannelReaderInterface::size_type sizeT;

    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
    {
        //uzupełnij brakujące próbki
        if(myChannel.size() < observedChannel.size()){
            for(auto idx = myChannel.size(); idx < observedChannel.size(); ++idx){
                modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
            }
        }
        //aktualizacja próbek

        sizeT meanR = 250;
        sizeT count = myChannel.size() - meanR;
        // brzeg lewy...
        for (sizeT l_idx = 0; l_idx < meanR; ++l_idx) {
            pointT sum = 0;
            for (sizeT i = 0; i < l_idx + meanR; ++i) {
                sum += observedChannel.value(i) * observedChannel.value(i);
            }
            sum /= (l_idx + meanR);
            sum = sqrt(sum);
            modifierInterface.setIndexData(l_idx, sum);
        }
        // właściwa interpolacja
        for(sizeT idx = meanR; idx < count; ++idx) {
            pointT sum = 0;
            for (sizeT i = idx - meanR; i < idx + meanR + 1; ++i) {
                sum += observedChannel.value(i) * observedChannel.value(i);
            }
            sum /= (meanR + meanR + 1);
            sum = sqrt(sum);
            modifierInterface.setIndexData(idx, sum);
        }
        // brzeg prawy...
        for (sizeT r_idx = count; r_idx < observedChannel.size(); ++r_idx) {
            pointT sum = 0;
            for (sizeT i = r_idx; i < observedChannel.size(); ++i) {
                sum += observedChannel.value(i) * observedChannel.value(i);
            }
            //sum /= (r_idx - count + meanR);
            sum /= observedChannel.size() - r_idx;
            sum = sqrt(sum);
            modifierInterface.setIndexData(r_idx, sum);
        }
    }
};

class ScalarChannelIntegrator
{
public:
    typedef ScalarChannelReaderInterface::point_type pointT;
    typedef ScalarChannelReaderInterface::size_type sizeT;

    pointT mean(const pointT& p1, const pointT& p2, const pointT& p3) const
    {
        return (p1 + p2 + p3) / 3;
    }
    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
    {
        ////uzupełnij brakujące próbki : co dziesiata...
        sizeT skipN = 10;
        if(myChannel.size() * skipN < observedChannel.size()){
            for(auto idx = myChannel.size() * skipN; idx < observedChannel.size(); idx += skipN){
                pointT sum = 0;
                sizeT right = idx + skipN;
                right = right < observedChannel.size() ? right : observedChannel.size();
                for (sizeT idx2 = idx; idx2 < right; ++idx2) {
                    sum += observedChannel.value(idx2);
                }
                modifierInterface.addPoint(observedChannel.argument(idx), sum / skipN);
            }
        }
        //aktualizacja próbek

        sizeT meanR = 10;
        sizeT count = myChannel.size() - meanR;

        for (sizeT step = 0; step < 50; ++step) {
            // brzeg lewy...
            for (sizeT l_idx = 0; l_idx < meanR; ++l_idx) {
                pointT sum = 0;
                for (sizeT i = 0; i < l_idx + meanR; ++i) {
                    sum += observedChannel.value(i);
                }
                sum /= (l_idx + meanR);
                modifierInterface.setIndexData(l_idx, sum);
            }
            // właściwa interpolacja
            for(sizeT idx = meanR; idx < count; ++idx) {
                pointT sum = 0;
                for (sizeT i = idx - meanR; i < idx + meanR + 1; ++i) {
                   sum += observedChannel.value(i);
                }
                sum /= (meanR + meanR + 1);
                modifierInterface.setIndexData(idx, sum);
            }
            // brzeg prawy...
            for (sizeT r_idx = count; r_idx < myChannel.size(); ++r_idx) {
                pointT sum = 0;
                for (sizeT i = r_idx; i < myChannel.size(); ++i) {
                    sum += observedChannel.value(i);
                }
                sum /= (r_idx - count + meanR);
                modifierInterface.setIndexData(r_idx, sum);
            }
        }

    }
};

//template <class PointType, class TimeType>
//class IntegratorChannel : public ChannelNoCopyModifier<PointType, TimeType>
//{
//    typedef float PointType;
//    typedef float TimeType;
//    class IntegratorChannel(const utils::IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr & channel) :
//        ChannelNoCopyModifier(channel)
//    {
//    }
//
//    virtual point_type_const_reference modify( point_type_const_reference point ) const
//    {
//        return tempPoint = point > mean ? point : -point;
//    }
//
//    virtual void initialize()
//    {
//        point_type sum = 0;
//        //średnia
//        size_type count = channel->size();
//        for(size_type idx = 0; idx < channel->size(); idx++){
//            sum += channel->value(idx);
//        }
//
//        mean = sum / static_cast<point_type>(count);
//    }
//
//private:
//    mutable point_type tempPoint;
//    point_type mean;
//};

template <class PointType, class TimeType>
class AbsMeanChannel : public ChannelNoCopyModifier<PointType, TimeType>
{
public:
    typedef ChannelNoCopyModifier<PointType, TimeType> BaseClass;
    typedef typename BaseClass::_MyChannelPtr _MyChannelPtr;
    typedef typename BaseClass::_MyChannelConstPtr _MyChannelConstPtr;
    typedef typename BaseClass::_MyModifierType _MyModifierType;
    typedef typename BaseClass::time_type time_type;
    typedef typename BaseClass::point_type point_type;
    typedef typename BaseClass::size_type size_type;
    typedef typename BaseClass::point_type_const_reference point_type_const_reference;

    AbsMeanChannel(const _MyChannelPtr & channel) :
      BaseClass(channel)
    {
    }

    virtual point_type_const_reference modify( point_type_const_reference point ) const
    {
        tempPoint = point - mean;
        tempPoint = tempPoint > 0 ? tempPoint : -tempPoint;

        return tempPoint;
    }

    virtual void initialize()
    {
        point_type min = this->channel->value(0);
        point_type max = min;

        size_type count = this->channel->size();
        for(size_type idx = 0; idx < count; ++idx){
            point_type val = this->channel->value(idx);
            if (val > max) {
                max = val;
            } else if (val < min) {
                min = val;
            }
        }

        point_type min4 = min / 4;
        point_type max4 = max / 4;

        point_type sum = 0;
        //średnia
        for(size_type idx = 0; idx < count; ++idx) {
            point_type val = this->channel->value(idx);
            if (val > max4) {
                val = max4;
            } else if (val < min4) {
                val = min4;
            }
            sum += val;
        }

        mean = sum / static_cast<point_type>(count);
    }

private:
    mutable point_type tempPoint;
    point_type mean;

};

EMGFilterHelper::EMGFilterHelper( const core::ObjectWrapperConstPtr& wrapper ) :
NewChartItemHelper(wrapper)
{
}

VisualizerPtr EMGFilterHelper::createVisualizer()
{
    return NewChartItemHelper::createVisualizer();
}

void EMGFilterHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    ScalarChannelReaderInterfacePtr channel = wrapper->clone()->get();

    boost::shared_ptr<AbsMeanChannel<float, float>> absTest( new AbsMeanChannel<float, float>(channel));
    absTest->initialize();

    //core::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, ScalarChannelIntegrator()));
    core::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, RMSModifier()));

    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    wrapperX->set(core::dynamic_pointer_cast<ScalarChannelReaderInterface>(integratorChannel));
	(*wrapperX)["core/name"] = wrapper->getName();
    (*wrapperX)["core/source"] = wrapper->getSource();
    visualizer->getOrCreateWidget();

    //series.push_back(core::dynamic_pointer_cast<core::IVisualizer::ITimeSerie>(visualizer->createSerie(wrapperX, wrapperX->getName())));
	series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(wrapperX, "UNKNOWN")));
}


