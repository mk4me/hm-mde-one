#include "CorePCH.h"
#include <core/IDataProcessor.h>
#include "DataProcessor.h"
#include "DataProcessorManager.h"
//#include "DataManager.h"
#include <core/DataAccessors.h>

using namespace core;

CORE_DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

DataProcessor::DataProcessor( core::IDataProcessor* impl ) : 
  InputOutputItem<core::IDataProcessor>(impl)
{
    DataProcessorManager::getInstance()->notifyCreated(this);
}

  DataProcessor::~DataProcessor()
  {
      DataProcessorManager::getInstance()->notifyDestroyed(this);
  }


//void DataProcessor::test()
//{
//    // obecnie definicje wrapperów znajduj¹ siê w ToolboxMain; tam równie¿ znajduje siê kod dodaj¹cy
//    // fabryki do DM
//    typedef core::shared_ptr<int> IntPtr;
//    typedef core::shared_ptr<const int> IntConstPtr;
//
//    class IncrementerMutable : public core::IDataProcessor
//    {
//    private:
//        std::string name;
//    public:
//        virtual UniqueID getID() const { return UniqueID(); }
//        virtual const std::string & getName() const { return name; }
//        virtual const std::string & getDescription() const { static const std::string desc("IncrementerMutable"); return desc; }
//        virtual QWidget* configure() { return nullptr; }
//        virtual IncrementerMutable* createClone() const { return nullptr; }
//
//        virtual void reset() {}
//
//        virtual void getInputInfo(std::vector<InputInfo>& info)
//        {
//            InputInfo input;
//            input.type = typeid(int);
//            input.name = "input";
//
//            info.push_back(input);
//        }
//        virtual void getOutputInfo(std::vector<OutputInfo>& info)
//        {
//            info.push_back(OutputInfo("input+1", typeid(int)));
//        }
//        virtual void process(IObjectSource* input, IObjectOutput* output)
//        {
//            // pobranie obiektu mo¿e, ale nie musi, go sklonowaæ
//            auto objects = input->getObjects(0);
//
//            if(objects.empty() == true){
//                return;
//            }
//
//            for(int i = 0; i < objects.size(); i++){
//                IntConstPtr val = objects.getObject(i);
//                IntPtr in(new int(*val));
//                LOG_DEBUG("Input: " << *in << " Output:" << *in + 1);
//                *in+=1;
//
//                output->getObjects(0).addObject(in);
//            }
//        }
//    };
//
//    class IncrementerImmutable : public core::IDataProcessor
//    {
//    private:
//        std::string name;
//    public:
//        IntPtr result;
//        IncrementerImmutable() : result(new int(0)) {}
//        virtual UniqueID getID() const { return UniqueID(); }
//        virtual const std::string & getDescription() const { static const std::string desc("IncrementerMutable"); return desc; }
//        virtual const std::string & getName() const { return name; }
//        virtual QWidget* getConfigurationWidget() { return nullptr; }
//        virtual IncrementerImmutable* createClone() const { return nullptr; }
//        virtual void reset() {}
//
//        virtual void getInputInfo(std::vector<InputInfo>& info)
//        {
//            InputInfo input;
//            input.type = typeid(int);
//            input.name = "input";
//
//            info.push_back(input);
//        }
//        virtual void getOutputInfo(std::vector<OutputInfo>& info)
//        {
//            info.push_back(OutputInfo("input+1", typeid(int)));
//        }
//        virtual void process(IObjectSource* input, IObjectOutput* output)
//        {
//            // takie pobranie nigdy nie spowoduje sklonowania,
//            // ale rezultatu nie mo¿na modyfikowaæ
//            auto objects = input->getObjects(0);
//
//            if(objects.empty() == true){
//                return;
//            }
//
//            for(int i = 0; i < objects.size(); i++){
//                IntConstPtr val = objects.getObject(i);
//                IntPtr in(new int(*val));
//                LOG_DEBUG("Input: " << *in << " Output:" << *in + 1);
//                *in+=1;
//
//                output->getObjects(0).addObject(in);
//            }
//        }
//    };
//
//    // tworzymy "Ÿród³o"
//    ObjectWrapperCollectionPtr wrapperCollection = DataManager::getInstance()->createWrapperCollection(typeid(int));
//    ObjectWrapperPtr wrapper = core::ObjectWrapper::create<int>();
//    wrapper->set(IntPtr(new int(666)));
//    wrapperCollection->addObject( wrapper );
//
//    wrapper = wrapper->clone();
//
//    wrapper->set(IntPtr(new int(0)));
//
//    wrapperCollection->addObject( wrapper );
//
//    // pierwszy wêze³, na wejœcie dostaje sta³y obiekt
//    DataProcessor processor(new IncrementerMutable());
//    processor.getSource()->setObjects(0, ObjectWrapperCollectionConstPtr(wrapperCollection));
//
//    // ten na wejœcie dostaje wyjœcie pierwszego elementu przetwarzaj¹cego; oczywiœcie
//    // w praktyce trzebaby to jakoœ zautomatyzowaæ, no i w tym momencie interfejs pozwala
//    // na pobranie tylko obiektu w zmiennym wariancie (nale¿a³oby to uzupe³niæ, ¿eby nie tworzyæ
//    // potworków jak ni¿ej
//    DataProcessor processor2(new IncrementerMutable());
//    //if ( processor.getOutput()->getObjects(0) ) {
//    //    // chocia¿ podajemy zmienny obiekt to i tak przy próbie dostêpu do zmiennego zostanie
//    //    // sklonowany, bo nie ustawiliœmy flagi passDirectly; ta flaga powinna byæ ustawiana
//    //    // na podstawie liczby nastêpników elementu przetwarzaj¹cego (1 - mo¿na przekazaæ bez klonowania,
//    //    // > 1 - nie mo¿na)
//    //    processor2.getSource()->setObjects(0, processor.getOutput()->getObjects(0));
//    //} else {
//    //    // przekazujemy sta³y obiekt skoro nie ma zmiennego; je¿eli element przetwarzaj¹cy bêdzie
//    //    // chcia³ to sobie sklonuje automatycznie
//    //    processor2.getSource()->setObjects(0, processor.getOutput()->getConstObjects(0));
//    //}
//
//    //DataProcessor processor3(new IncrementerMutable());
//    //// tutaj pozwalamy na przekazanie bezpoœrednie; normalnie powinno to byæ ustawiane na podstawie
//    //// liczby nastêpników processor
//    ////processor3.getSource()->setPassDirectly(0, true);
//    //if ( processor3.getOutput()->getObjects(0) ) {
//    //    // je¿eli tu wejdziemy to obiekt z processor zostanie zmodyfikowany!
//    //    processor3.getSource()->setObjects(0, processor.getOutput()->getObjects(0));
//    //} else {
//    //    processor3.getSource()->setObjects(0, processor.getOutput()->getConstObjects(0));
//    //}
//
//    //DataProcessor processor4(new IncrementerImmutable());
//    //// tutaj ta opcja nic nie zmieni, gdy¿ implementacja i tak pobiera sta³y wskaŸnik
//    ////processor4.getSource()->setPassDirectly(0, true);
//    //if ( processor4.getOutput()->getObjects(0) ) {
//    //    processor4.getSource()->setObjects(0, processor.getOutput()->getObjects(0));
//    //} else {
//    //    processor4.getSource()->setObjects(0, processor.getOutput()->getConstObjects(0));
//    //}
//
//    //processor.tryRun();
//    //processor2.tryRun();
//    //processor3.tryRun();
//    //// na wejœciu bêdzie mia³ 668, przez processor3, który ma ustawione przekazywanie bezpoœrednie
//    //processor4.tryRun();
//
//    //// mo¿na sobie podejrzeæ, co znajduje siê na outputach
//    //auto out1 = processor.getOutputObjects(0);
//    //auto out2 = processor2.getOutputObjects(0);
//    //auto out3 = processor3.getOutputObjects(0);
//    //auto out4 = processor4.getOutputObjects(0);
//}