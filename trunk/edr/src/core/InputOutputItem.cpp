#include "CorePCH.h"
#include "InputOutputItem.h"

//template<class T>
//InputOutputItem<T>::InputOutputItem( T* implementation ) : 
//    WorkflowItemEncapsulator<T>(implementation),
//    InputDescription(createSource()),
//    OutputDescription(createOutput())
//{}
//
//template<class T>
//ObjectOutput InputOutputItem<T>::createOutput()
//{
//    std::vector<T::OutputInfo> coreOutputInfos;
//    getImplementation()->getOutputInfo(coreOutputInfos);
//    std::vector<OutputInfo> outputInfos;
//    outputInfos.reserve(coreOutputInfos.size());
//    std::for_each(coreOutputInfos.begin(), coreOutputInfos.end(), [&](T::OutputInfo& info) {
//        auto inserted = outputInfos.insert(outputInfos.end(), std::vector<OutputInfo>::value_type());
//        inserted->name.swap(info.name);
//        inserted->types.push_back(info.type);
//    });
//    ObjectOutput output(outputInfos);
//    return output;
//}
//
//template<class T>
//ObjectSource InputOutputItem<T>::createSource()
//{
//    std::vector<InputInfo> inputInfos;
//    T::InputInfo inputInfo;
//    for (int i = 0; i < T::maxNumSources; ++i) {
//        inputInfo.name.clear();
//        inputInfo.types.clear();
//        getImplementation()->getInputInfo(i, inputInfo);
//        if ( inputInfo.types.empty() ) {
//            break;
//        } else {
//            // zamiast push_backa mo¿na zrobiæ bardziej optymalnie i nie kopiowaæ wektora...
//            // sourcesTypes.push_back(info);
//            auto inserted = inputInfos.insert(inputInfos.end(), InputInfo());
//            inserted->name.swap(inputInfo.name);
//            inserted->types.swap(inputInfo.types);
//        }
//    }
//    //core::shared_ptr<ObjectSource> source(new ObjectSource(inputInfos));
//    ObjectSource source(inputInfos);
//    return source;
//}
