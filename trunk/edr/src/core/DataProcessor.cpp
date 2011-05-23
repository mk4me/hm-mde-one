#include "CorePCH.h"
#include <core/IDataProcessor.h>
#include "DataProcessor.h"
#include "DataManager.h"


using namespace core;



DataProcessor::DataProcessor( core::IDataProcessor* impl ) : 
  InputOutputItem<core::IDataProcessor>(impl)
{
    //// TODO: informacje o wyjœciu/wejœciu mog¹ byæ liczone raz podczas rejestracji
    //// i potem kopiowane jako wskaŸniki/referencje

    //// wczytanie danych o wejœciu
    //// uwaga: core::IDataProcessor::InputInfo i InputInfo to ró¿ne typy! zrobione tak po to,
    //// aby wewnêtrznie mo¿liwe by³o dodanie/modyfikacja informacji
    //std::vector<InputInfo> inputInfos;
    //core::IDataProcessor::InputInfo inputInfo;
    //for (int i = 0; i < core::IDataProcessor::maxNumSources; ++i) {
    //    inputInfo.name.clear();
    //    inputInfo.types.clear();
    //    impl->getInputInfo(i, inputInfo);
    //    if ( inputInfo.types.empty() ) {
    //        break;
    //    } else {
    //        // zamiast push_backa mo¿na zrobiæ bardziej optymalnie i nie kopiowaæ wektora...
    //        // sourcesTypes.push_back(info);
    //        auto inserted = inputInfos.insert(inputInfos.end(), InputInfo());
    //        inserted->name.swap(inputInfo.name);
    //        inserted->types.swap(inputInfo.types);
    //    }
    //}
    //source.reset(new ObjectSource(inputInfos));

    //// wczytanie danych o wejœciu
    //// uwaga: core::IDataProcessor::OutputInfo i OutputInfo to ró¿ne typy! zrobione tak po to,
    //// aby wewnêtrznie mo¿liwe by³o dodanie/modyfikacja informacji
    //std::vector<core::IDataProcessor::OutputInfo> coreOutputInfos;
    //impl->getOutputInfo(coreOutputInfos);
    //std::vector<OutputInfo> outputInfos;
    //outputInfos.reserve(coreOutputInfos.size());
    //std::for_each(coreOutputInfos.begin(), coreOutputInfos.end(), [&](core::IDataProcessor::OutputInfo& info) {
    //    auto inserted = outputInfos.insert(outputInfos.end(), std::vector<OutputInfo>::value_type());
    //    inserted->name.swap(info.name);
    //    inserted->types.push_back(info.type);
    //});
    //output.reset(new ObjectOutput(outputInfos));

    //// stworzenie instancji ObjectWrapperów dla ka¿dego z wyjœæ
    //for (int i = 0; i < output->getNumSlots(); ++i) {
    //    static_cast<ObjectSlots*>(output.get())->setObject(i, DataManager::getInstance()->createWrapper( output->getSlotTypes(i).front() ) );
    //}
}


void DataProcessor::test()
{
    // obecnie definicje wrapperów znajduj¹ siê w ToolboxMain; tam równie¿ znajduje siê kod dodaj¹cy
    // fabryki do DM
    typedef core::shared_ptr<int> IntPtr;
    typedef core::shared_ptr<const int> IntConstPtr;

    class IncrementerMutable : public core::IDataProcessor
    {
    private:
        std::string name;
    public:
        virtual UniqueID getID() const { return UniqueID(); }
        virtual const std::string &getName(void) const { return name; }
        virtual std::string getDescription() const { return ""; }
        virtual QWidget* configure() { return nullptr; }
        virtual IncrementerMutable* createClone() const { return nullptr; }
        virtual void getInputInfo(int inputNo, InputInfo& info)
        {
            if ( inputNo == 0 ) {
                info.name = "input";
                info.types.push_back(typeid(int));
            }
        }
        virtual void getOutputInfo(std::vector<OutputInfo>& info)
        {
            info.push_back(OutputInfo("input+1", typeid(int)));
        }
        virtual void process(IObjectSource* input, IObjectOutput* output)
        {
            // pobranie obiektu mo¿e, ale nie musi, go sklonowaæ
            IntPtr in = input->getObject(0);
            LOG_DEBUG("Input: " << *in << " Output:" << *in + 1);
            *in+=1;
            output->setObject(0, in);
            // je¿eli zale¿y nam, ¿eby nigdy nie nast¹pi³a modyfikacja przez nastêpników,
            // mo¿na zapakowaæ obiekt do wskaŸnika na sta³y obiekt
            // output->setObject(0, IntConstPtr(in));
        }
    };

    class IncrementerImmutable : public core::IDataProcessor
    {
    private:
        std::string name;
    public:
        IntPtr result;
        IncrementerImmutable() : result(new int(0)) {}
        virtual UniqueID getID() const { return UniqueID(); }
        virtual std::string getDescription() const { return ""; }
        virtual const std::string &getName(void) const { return name; }
        virtual QWidget* getConfigurationWidget() { return nullptr; }
        virtual IncrementerImmutable* createClone() const { return nullptr; }

        virtual void getInputInfo(int inputNo, InputInfo& info)
        {
            if ( inputNo == 0 ) {
                info.name = "input";
                info.types.push_back(typeid(int));
            }
        }
        virtual void getOutputInfo(std::vector<OutputInfo>& info)
        {
            info.push_back(OutputInfo("input+1", typeid(int)));
        }
        virtual void process(IObjectSource* input, IObjectOutput* output)
        {
            // takie pobranie nigdy nie spowoduje sklonowania,
            // ale rezultatu nie mo¿na modyfikowaæ
            IntConstPtr in = input->getObject(0);
            LOG_DEBUG("Input: " << *in << " Output:" << *in + 1);
            *result = *in + 1;
            // tutaj podobnie, aby zablokowaæ mo¿liwoœæ zmiany przez nastêpników
            // mo¿naby opakowaæ
            output->setObject(0, result);
        }
    };

    // tworzymy "Ÿród³o"
    ObjectWrapperPtr wrapper = DataManager::getInstance()->createWrapper(typeid(int));
    wrapper->set( IntPtr(new int(666)) );

    // pierwszy wêze³, na wejœcie dostaje sta³y obiekt
    DataProcessor processor(new IncrementerMutable());
    processor.getSource()->setObject(0, ObjectWrapperConstPtr(wrapper));

    // ten na wejœcie dostaje wyjœcie pierwszego elementu przetwarzaj¹cego; oczywiœcie
    // w praktyce trzebaby to jakoœ zautomatyzowaæ, no i w tym momencie interfejs pozwala
    // na pobranie tylko obiektu w zmiennym wariancie (nale¿a³oby to uzupe³niæ, ¿eby nie tworzyæ
    // potworków jak ni¿ej
    DataProcessor processor2(new IncrementerMutable());
    if ( processor.getOutput()->getObject(0) ) {
        // chocia¿ podajemy zmienny obiekt to i tak przy próbie dostêpu do zmiennego zostanie
        // sklonowany, bo nie ustawiliœmy flagi passDirectly; ta flaga powinna byæ ustawiana
        // na podstawie liczby nastêpników elementu przetwarzaj¹cego (1 - mo¿na przekazaæ bez klonowania,
        // > 1 - nie mo¿na)
        processor2.getSource()->setObject(0, processor.getOutput()->getObject(0));
    } else {
        // przekazujemy sta³y obiekt skoro nie ma zmiennego; je¿eli element przetwarzaj¹cy bêdzie
        // chcia³ to sobie sklonuje automatycznie
        processor2.getSource()->setObject(0, processor.getOutput()->getConstObject(0));
    }

    DataProcessor processor3(new IncrementerMutable());
    // tutaj pozwalamy na przekazanie bezpoœrednie; normalnie powinno to byæ ustawiane na podstawie
    // liczby nastêpników processor
    processor3.getSource()->setPassDirectly(0, true);
    if ( processor3.getOutput()->getObject(0) ) {
        // je¿eli tu wejdziemy to obiekt z processor zostanie zmodyfikowany!
        processor3.getSource()->setObject(0, processor.getOutput()->getObject(0));
    } else {
        processor3.getSource()->setObject(0, processor.getOutput()->getConstObject(0));
    }

    DataProcessor processor4(new IncrementerImmutable());
    // tutaj ta opcja nic nie zmieni, gdy¿ implementacja i tak pobiera sta³y wskaŸnik
    processor4.getSource()->setPassDirectly(0, true);
    if ( processor4.getOutput()->getObject(0) ) {
        processor4.getSource()->setObject(0, processor.getOutput()->getObject(0));
    } else {
        processor4.getSource()->setObject(0, processor.getOutput()->getConstObject(0));
    }

    processor.tryProcess();
    processor2.tryProcess();
    processor3.tryProcess();
    // na wejœciu bêdzie mia³ 668, przez processor3, który ma ustawione przekazywanie bezpoœrednie
    processor4.tryProcess();

    // mo¿na sobie podejrzeæ, co znajduje siê na outputach
    auto out1 = processor.getOutputObject(0);
    auto out2 = processor2.getOutputObject(0);
    auto out3 = processor3.getOutputObject(0);
    auto out4 = processor4.getOutputObject(0);
}