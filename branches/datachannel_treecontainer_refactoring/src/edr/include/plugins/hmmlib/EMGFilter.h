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

using namespace core;

class RMSModifier
{
public:

	//void change()
 //   {
 //       //uzupełnij brakujące próbki
 //       if(myChannel.size() < observedChannel.size()){
 //           for(auto idx = myChannel.size(); idx < observedChannel.size(); ++idx){
 //               modifierInterface.addPoint(observedChannel.argument(idx), observedChannel.value(idx));
 //           }
 //       }
 //       //aktualizacja próbek

 //       sizeT meanR = 250;
 //       sizeT count = myChannel.size() - meanR;
 //       // brzeg lewy...
 //       for (sizeT l_idx = 0; l_idx < meanR; ++l_idx) {
 //           pointT sum = 0;
 //           for (sizeT i = 0; i < l_idx + meanR; ++i) {
 //               sum += observedChannel.value(i) * observedChannel.value(i);
 //           }
 //           sum /= (l_idx + meanR);
 //           sum = sqrt(sum);
 //           modifierInterface.setIndexData(l_idx, sum);
 //       }
 //       // właściwa interpolacja
 //       for(sizeT idx = meanR; idx < count; ++idx) {
 //           pointT sum = 0;
 //           for (sizeT i = idx - meanR; i < idx + meanR + 1; ++i) {
 //               sum += observedChannel.value(i) * observedChannel.value(i);
 //           }
 //           sum /= (meanR + meanR + 1);
 //           sum = sqrt(sum);
 //           modifierInterface.setIndexData(idx, sum);
 //       }
 //       // brzeg prawy...
 //       for (sizeT r_idx = count; r_idx < observedChannel.size(); ++r_idx) {
 //           pointT sum = 0;
 //           for (sizeT i = r_idx; i < observedChannel.size(); ++i) {
 //               sum += observedChannel.value(i) * observedChannel.value(i);
 //           }
 //           //sum /= (r_idx - count + meanR);
 //           sum /= observedChannel.size() - r_idx;
 //           sum = sqrt(sum);
 //           modifierInterface.setIndexData(r_idx, sum);
 //       }
 //   }
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

//template <class PointType, class TimeType>
//class AbsMeanChannel : public ChannelNoCopyModifier<PointType, TimeType>
//{
//public:
//    typedef ChannelNoCopyModifier<PointType, TimeType> BaseClass;
//    typedef typename BaseClass::_MyChannelPtr _MyChannelPtr;
//    typedef typename BaseClass::_MyChannelConstPtr _MyChannelConstPtr;
//    typedef typename BaseClass::_MyModifierType _MyModifierType;
//    typedef typename BaseClass::time_type time_type;
//    typedef typename BaseClass::point_type point_type;
//    typedef typename BaseClass::size_type size_type;
//    typedef typename BaseClass::point_type_const_reference point_type_const_reference;
//
//    AbsMeanChannel(const _MyChannelPtr & channel) :
//    BaseClass(channel)
//    {
//    }
//
//    virtual point_type_const_reference modify( point_type_const_reference point ) const
//    {
//        tempPoint = point - mean;
//        tempPoint = tempPoint > 0 ? tempPoint : -tempPoint;
//
//        return tempPoint;
//    }
//
//    virtual void initialize()
//    {
//        point_type min = this->channel->value(0);
//        point_type max = min;
//
//        size_type count = this->channel->size();
//        for(size_type idx = 0; idx < count; ++idx){
//            point_type val = this->channel->value(idx);
//            if (val > max) {
//                max = val;
//            } else if (val < min) {
//                min = val;
//            }
//        }
//
//        point_type min4 = min / 4;
//        point_type max4 = max / 4;
//
//        point_type sum = 0;
//        //średnia
//        for(size_type idx = 0; idx < count; ++idx) {
//            point_type val = this->channel->value(idx);
//            if (val > max4) {
//                val = max4;
//            } else if (val < min4) {
//                val = min4;
//            }
//            sum += val;
//        }
//
//        mean = sum / static_cast<point_type>(count);
//    }
//
//private:
//    mutable point_type tempPoint;
//    point_type mean;
//
//};


#endif
