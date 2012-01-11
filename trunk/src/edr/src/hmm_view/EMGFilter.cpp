#include "hmmPCH.h"
#include "EMGFilter.h"
#include <plugins/c3d/C3DChannels.h>
#include <plugins/newChart/NewChartSerie.h>

template<class PointType, class TimeType>
class ChannelNoCopyModifier : public utils::IChannelAutoModifier<PointType, TimeType>
{
public:
    ChannelNoCopyModifier(const typename utils::IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr & channel) : 
      channel(channel)
    {
        UTILS_ASSERT((channel != nullptr), "Wrong channel for Tracker");

        if(channel == nullptr){
            throw std::runtime_error("Wrong channel for Tracker");
        }
    }
    virtual ~ChannelNoCopyModifier() {}

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

    //! \param idx Indeks probki
    //! \return Wartosc czasu dla danego indeksu
    virtual time_type argument(size_type idx) const
    {
        return channel->argument(idx);
    }

    //! \param idx Indeks probki
    //! \return Wartosc probki dla danego indeksu
    virtual point_type_const_reference value(size_type idx) const
    {
        return modify(channel->value(idx));
    }

    //! \return Ilo�� pr�bek w kanale
    virtual size_type size() const
    {
        return channel->size();
    }

    //! \return Czy kana� nie zawiera danych
    virtual bool empty() const
    {
        return channel->empty();
    }
    //! \param time Czas dla ktorego chemy uzyskac dwie najblizsze probki
    //! \return para indeksow, pierwszy wskazujke probke o czasie mniejszym lub rownym zadanemu czasowi, drugi wskazuje probke o czasie wiekszym lub rownym zadanemu
    virtual std::pair<size_type, size_type> getValueHelper(time_type time) const
    {
        return channel->getValueHelper(time);
    }

    virtual point_type_const_reference modify(point_type_const_reference point) const = 0;
    virtual void initialize() = 0;

    virtual const _MyModifierType & getModifier() const
    {
        UTILS_ASSERT(false);
        return _MyModifierType();
    }

protected:
    typename IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr channel;
};

//class ScalarChannelAbs
//{
//public:
//
//    void operator()(ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
//        const ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
//        const ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
//    {
//        //uzupe�nij brakujace prboki
//        if(myChannel.size() < observedChannel.size()){
//            for(auto idx = myChannel.size(); idx < observedChannel.size(); idx++){
//                modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
//            }
//        }
//
//        ScalarChannelReaderInterface::point_type sum = 0;
//        //srednia
//        for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); idx++){
//            sum += observedChannel.value(idx);
//        }
//
//        ScalarChannelReaderInterface::point_type mean = sum / myChannel.size();
//        //aktualizacja pr�bek
//        for(ScalarChannelReaderInterface::size_type idx = 0; idx < myChannel.size(); idx++){
//            auto point = observedChannel.value(idx);
//            modifierInterface.setIndexData(idx, point > mean ? point : -point);
//        }
//    }
//};

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
        //uzupe�nij brakujace probki : co dziesiata...
        int skipN = 10;
        if(myChannel.size() * skipN < observedChannel.size()){
            for(auto idx = myChannel.size() * skipN; idx < observedChannel.size(); idx += skipN){
                pointT sum = 0;
                int right = idx + skipN;
                right = right < observedChannel.size() ? right : observedChannel.size();
                for (auto idx2 = idx; idx2 < right; idx2++) {
                    sum += observedChannel.value(idx2);
                }
                modifierInterface.addPoint(observedChannel.argument(idx), sum / skipN);
            }
        }
        //aktualizacja pr�bek
        
        sizeT meanR = 10;
        sizeT count = myChannel.size() - meanR;

        for (int step = 0; step < 5; step++) {
            // brzeg lewy...
            for (sizeT l_idx = 0; l_idx < meanR; l_idx++) {
                pointT sum = 0;
                for (int i = 0; i < l_idx + meanR; i++) {
                    sum += observedChannel.value(i);
                }
                sum /= (l_idx + meanR);
                modifierInterface.setIndexData(l_idx, sum);
            }
            // wlasciwa interpolacja
            for(sizeT idx = meanR; idx < count; idx++) {
                pointT sum = 0;
                for (int i = idx - meanR; i < idx + meanR + 1; i++) {
                   sum += observedChannel.value(i);
                }
                sum /= (meanR + meanR + 1);
                modifierInterface.setIndexData(idx, sum);
            }
            // brzeg prawy...
            for (sizeT r_idx = count; r_idx < myChannel.size(); r_idx++) {
                pointT sum = 0;
                for (int i = r_idx; i < myChannel.size(); i++) {
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
//        //srednia
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
    AbsMeanChannel(const utils::IChannelAutoModifier<PointType, TimeType>::_MyChannelPtr & channel) : 
      ChannelNoCopyModifier(channel)
    {
    }

    virtual point_type_const_reference modify( point_type_const_reference point ) const
    {
        return tempPoint = point > mean ? point : -point;
    }

    virtual void initialize() 
    {
        point_type sum = 0;
        //srednia
        size_type count = channel->size();
        for(size_type idx = 0; idx < channel->size(); idx++){
            sum += channel->value(idx);
        }

        mean = sum / static_cast<point_type>(count);
    }

private:
    mutable point_type tempPoint;
    point_type mean;

};


VisualizerPtr EMGFilterHelper::createVisualizer()
{
    return NewChartItemHelper::createVisualizer();
}

void EMGFilterHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    ScalarChannelReaderInterfacePtr channel = wrapper->get();
    ScalarChannelReaderInterfacePtr nonConstChannel2(core::const_pointer_cast<ScalarChannelReaderInterface>(channel));

    //core::shared_ptr<ScalarModifier> absChannel(new ScalarModifier(nonConstChannel2, ScalarChannelAbs()));
   
    boost::shared_ptr<AbsMeanChannel<float, float>> absTest( new AbsMeanChannel<float, float>(nonConstChannel2));
    absTest->initialize();
    
    core::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, ScalarChannelIntegrator()));
    
    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    wrapperX->set(core::dynamic_pointer_cast<ScalarChannelReaderInterface>(integratorChannel));
    wrapperX->setName  (wrapper->getName());
    wrapperX->setSource(wrapper->getSource());
    visualizer->getOrCreateWidget();

    auto serieX = visualizer->createSerie(wrapperX, wrapperX->getName());

    NewChartSerie* chartSerieX = dynamic_cast<NewChartSerie*>(serieX.get());

    //chartSerieX->setColor(255, 0, 0);

    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
}
