/********************************************************************
	created:	2011/12/11
	created:	11:12:2011   19:39
	filename: 	EMGFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EMGFILTER_H__
#define HEADER_GUARD_HMM__EMGFILTER_H__

//#include "TreeItemHelper.h"

#include <plugins/c3d/C3DChannels.h>
#include <plugins/newChart/INewChartSerie.h>
#include <plugins/hmmlib/Export.h>
#include <loglib/Exceptions.h>
#include <datachannellib/IBoundedValuesFeature.h>
#include <datachannellib/Adapters.h>

using namespace core;

class RMSValueExtractor
{

};

template<typename ValueType>
class RMSValueExtractorT
{
public:

	RMSValueExtractorT(const std::size_t meanR) : meanR(meanR) {}
	RMSValueExtractorT(const RMSValueExtractorT & Other) : meanR(Other.meanR) {}
	RMSValueExtractorT(RMSValueExtractorT && Other) : meanR(Other.meanR) {}
	~RMSValueExtractorT() {}

	ValueType extract(const ValueType & value, const std::size_t idx)
	{
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
		for (sizeT idx = meanR; idx < count; ++idx) {
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
    

private:

	const std::size_t meanR;
};

class ScalarChannelIntegrator
{
public:

 //   pointT mean(const pointT& p1, const pointT& p2, const pointT& p3) const
 //   {
 //       return (p1 + p2 + p3) / 3;
 //   }
	//void operator()(c3dlib::ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
	//				const c3dlib::ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
	//				const c3dlib::ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
 //   {
 //       ////uzupełnij brakujące próbki : co dziesiata...
 //       sizeT skipN = 10;
 //       if(myChannel.size() * skipN < observedChannel.size()){
 //           for(auto idx = myChannel.size() * skipN; idx < observedChannel.size(); idx += skipN){
 //               pointT sum = 0;
 //               sizeT right = idx + skipN;
 //               right = right < observedChannel.size() ? right : observedChannel.size();
 //               for (sizeT idx2 = idx; idx2 < right; ++idx2) {
 //                   sum += observedChannel.value(idx2);
 //               }
 //               modifierInterface.addPoint(observedChannel.argument(idx), sum / skipN);
 //           }
 //       }
 //       //aktualizacja próbek

 //       sizeT meanR = 10;
 //       sizeT count = myChannel.size() - meanR;

 //       for (sizeT step = 0; step < 50; ++step) {
 //           // brzeg lewy...
 //           for (sizeT l_idx = 0; l_idx < meanR; ++l_idx) {
 //               pointT sum = 0;
 //               for (sizeT i = 0; i < l_idx + meanR; ++i) {
 //                   sum += observedChannel.value(i);
 //               }
 //               sum /= (l_idx + meanR);
 //               modifierInterface.setIndexData(l_idx, sum);
 //           }
 //           // właściwa interpolacja
 //           for(sizeT idx = meanR; idx < count; ++idx) {
 //               pointT sum = 0;
 //               for (sizeT i = idx - meanR; i < idx + meanR + 1; ++i) {
 //                   sum += observedChannel.value(i);
 //               }
 //               sum /= (meanR + meanR + 1);
 //               modifierInterface.setIndexData(idx, sum);
 //           }
 //           // brzeg prawy...
 //           for (sizeT r_idx = count; r_idx < myChannel.size(); ++r_idx) {
 //               pointT sum = 0;
 //               for (sizeT i = r_idx; i < myChannel.size(); ++i) {
 //                   sum += observedChannel.value(i);
 //               }
 //               sum /= (r_idx - count + meanR);
 //               modifierInterface.setIndexData(r_idx, sum);
 //           }
 //       }

 //   }
};

template<class ValueType, class ArgumentType>
class AbsMeanValueExtractorT;

class AbsMeanValueExtractor
{
public:

	template<typename DiscreteAccessor>
	inline static AbsMeanValueExtractorT<typename DiscreteAccessor::value_type, typename DiscreteAccessor::argument_type> create(const DiscreteAccessor & accessor)
	{
		return AbsMeanValueExtractorT<typename DiscreteAccessor::value_type, typename DiscreteAccessor::argument_type>(accessor);
	}
};

template<class ValueType, class ArgumentType>
class AbsMeanValueExtractorT
{
public:
	using accessor_type = datachannel::IDiscreteAccessor<ValueType, ArgumentType>;
	using value_type = accessor_type::value_type;
	using size_type = accessor_type::size_type;

	AbsMeanValueExtractorT(const accessor_type & accessor)
		: mean(initialize(accessor))
    {

    }

	AbsMeanValueExtractorT(const AbsMeanValueExtractorT & Other) : mean(Other.mean)
	{

	}

	AbsMeanValueExtractorT(AbsMeanValueExtractorT && Other) : mean(Other.mean)
	{

	}

	~AbsMeanValueExtractorT() {}	

	inline value_type extract(const value_type & value, ...) const
    {
        return std::abs(value - mean);
    }

private:

	inline static value_type initialize(const accessor_type & accessor)
    {
		auto bvf = accessor.getOrCreateValueFeature<datachannel::IBoundedValuesFeature>();
		const value_type min4 = bvf->minValue() / 4;
		const value_type max4 = bvf->maxValue() / 4;

		value_type sum = 0;
        //średnia
        for(size_type idx = 0; idx < count; ++idx) {
			auto val = accessor.value(idx);
            if (val > max4) {
                val = max4;
            } else if (val < min4) {
                val = min4;
            }
            sum += val;
        }

        return (sum / static_cast<value_type>(count));
    }

private:

    const point_type mean;

};


#endif
