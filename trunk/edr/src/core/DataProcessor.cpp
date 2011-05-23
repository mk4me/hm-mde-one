#include "CorePCH.h"
#include <core/IDataProcessor.h>
#include "DataProcessor.h"
#include "DataManager.h"


using namespace core;



DataProcessor::DataProcessor( core::IDataProcessor* impl ) : 
  InputOutputItem<core::IDataProcessor>(impl)
{
    //// TODO: informacje o wyj�ciu/wej�ciu mog� by� liczone raz podczas rejestracji
    //// i potem kopiowane jako wska�niki/referencje

    //// wczytanie danych o wej�ciu
    //// uwaga: core::IDataProcessor::InputInfo i InputInfo to r�ne typy! zrobione tak po to,
    //// aby wewn�trznie mo�liwe by�o dodanie/modyfikacja informacji
    //std::vector<InputInfo> inputInfos;
    //core::IDataProcessor::InputInfo inputInfo;
    //for (int i = 0; i < core::IDataProcessor::maxNumSources; ++i) {
    //    inputInfo.name.clear();
    //    inputInfo.types.clear();
    //    impl->getInputInfo(i, inputInfo);
    //    if ( inputInfo.types.empty() ) {
    //        break;
    //    } else {
    //        // zamiast push_backa mo�na zrobi� bardziej optymalnie i nie kopiowa� wektora...
    //        // sourcesTypes.push_back(info);
    //        auto inserted = inputInfos.insert(inputInfos.end(), InputInfo());
    //        inserted->name.swap(inputInfo.name);
    //        inserted->types.swap(inputInfo.types);
    //    }
    //}
    //source.reset(new ObjectSource(inputInfos));

    //// wczytanie danych o wej�ciu
    //// uwaga: core::IDataProcessor::OutputInfo i OutputInfo to r�ne typy! zrobione tak po to,
    //// aby wewn�trznie mo�liwe by�o dodanie/modyfikacja informacji
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

    //// stworzenie instancji ObjectWrapper�w dla ka�dego z wyj��
    //for (int i = 0; i < output->getNumSlots(); ++i) {
    //    static_cast<ObjectSlots*>(output.get())->setObject(i, DataManager::getInstance()->createWrapper( output->getSlotTypes(i).front() ) );
    //}
}


void DataProcessor::test()
{
    // obecnie definicje wrapper�w znajduj� si� w ToolboxMain; tam r�wnie� znajduje si� kod dodaj�cy
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
            // pobranie obiektu mo�e, ale nie musi, go sklonowa�
            IntPtr in = input->getObject(0);
            LOG_DEBUG("Input: " << *in << " Output:" << *in + 1);
            *in+=1;
            output->setObject(0, in);
            // je�eli zale�y nam, �eby nigdy nie nast�pi�a modyfikacja przez nast�pnik�w,
            // mo�na zapakowa� obiekt do wska�nika na sta�y obiekt
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
            // ale rezultatu nie mo�na modyfikowa�
            IntConstPtr in = input->getObject(0);
            LOG_DEBUG("Input: " << *in << " Output:" << *in + 1);
            *result = *in + 1;
            // tutaj podobnie, aby zablokowa� mo�liwo�� zmiany przez nast�pnik�w
            // mo�naby opakowa�
            output->setObject(0, result);
        }
    };

    // tworzymy "�r�d�o"
    ObjectWrapperPtr wrapper = DataManager::getInstance()->createWrapper(typeid(int));
    wrapper->set( IntPtr(new int(666)) );

    // pierwszy w�ze�, na wej�cie dostaje sta�y obiekt
    DataProcessor processor(new IncrementerMutable());
    processor.getSource()->setObject(0, ObjectWrapperConstPtr(wrapper));

    // ten na wej�cie dostaje wyj�cie pierwszego elementu przetwarzaj�cego; oczywi�cie
    // w praktyce trzebaby to jako� zautomatyzowa�, no i w tym momencie interfejs pozwala
    // na pobranie tylko obiektu w zmiennym wariancie (nale�a�oby to uzupe�ni�, �eby nie tworzy�
    // potwork�w jak ni�ej
    DataProcessor processor2(new IncrementerMutable());
    if ( processor.getOutput()->getObject(0) ) {
        // chocia� podajemy zmienny obiekt to i tak przy pr�bie dost�pu do zmiennego zostanie
        // sklonowany, bo nie ustawili�my flagi passDirectly; ta flaga powinna by� ustawiana
        // na podstawie liczby nast�pnik�w elementu przetwarzaj�cego (1 - mo�na przekaza� bez klonowania,
        // > 1 - nie mo�na)
        processor2.getSource()->setObject(0, processor.getOutput()->getObject(0));
    } else {
        // przekazujemy sta�y obiekt skoro nie ma zmiennego; je�eli element przetwarzaj�cy b�dzie
        // chcia� to sobie sklonuje automatycznie
        processor2.getSource()->setObject(0, processor.getOutput()->getConstObject(0));
    }

    DataProcessor processor3(new IncrementerMutable());
    // tutaj pozwalamy na przekazanie bezpo�rednie; normalnie powinno to by� ustawiane na podstawie
    // liczby nast�pnik�w processor
    processor3.getSource()->setPassDirectly(0, true);
    if ( processor3.getOutput()->getObject(0) ) {
        // je�eli tu wejdziemy to obiekt z processor zostanie zmodyfikowany!
        processor3.getSource()->setObject(0, processor.getOutput()->getObject(0));
    } else {
        processor3.getSource()->setObject(0, processor.getOutput()->getConstObject(0));
    }

    DataProcessor processor4(new IncrementerImmutable());
    // tutaj ta opcja nic nie zmieni, gdy� implementacja i tak pobiera sta�y wska�nik
    processor4.getSource()->setPassDirectly(0, true);
    if ( processor4.getOutput()->getObject(0) ) {
        processor4.getSource()->setObject(0, processor.getOutput()->getObject(0));
    } else {
        processor4.getSource()->setObject(0, processor.getOutput()->getConstObject(0));
    }

    processor.tryProcess();
    processor2.tryProcess();
    processor3.tryProcess();
    // na wej�ciu b�dzie mia� 668, przez processor3, kt�ry ma ustawione przekazywanie bezpo�rednie
    processor4.tryProcess();

    // mo�na sobie podejrze�, co znajduje si� na outputach
    auto out1 = processor.getOutputObject(0);
    auto out2 = processor2.getOutputObject(0);
    auto out3 = processor3.getOutputObject(0);
    auto out4 = processor4.getOutputObject(0);
}