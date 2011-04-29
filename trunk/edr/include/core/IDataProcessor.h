/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   17:29
    filename: IDataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IDATAPROCESSOR_H__
#define HEADER_GUARD_CORE__IDATAPROCESSOR_H__

#include <core/ObjectWrapper.h>
#include <core/IObjectSource.h>
#include <core/IObjectOutput.h>
#include <list>

namespace core 
{
    //! Interfejs elementu przetwarzajπcego dane.
    class IDataProcessor
    {
    public:
        static const int maxNumSources = 16;
        //! Struktura opisujπca dane wejúciowe.
        struct InputInfo
        {
            //! Nazwa wejúcia.
            std::string name;
            //! Typy wspierane przez wejúcie.
            std::list<core::TypeInfo> types;
        };
        //! Struktura opisujπca dane wyjúciowe.
        struct OutputInfo
        {
            //! Nazwa wyjúcia.
            std::string name;
            //! Typ wyjúcia.
            core::TypeInfo type;
            //!
            OutputInfo(const std::string& str, const core::TypeInfo& type) :
            name(name), type(type)
            {}
        };

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IDataProcessor() {}

        //! Pobiera informacje o wejúciu. Gdy danego wejúcia nie ma wynikowa lista typÛw powinna
        //! byÊ pusta.
        //! \param inputNo Numer wejúcia. 
        //! \param name Nazwa wejúcia.
        //! \param types Typy wspierane na danym wejúciu.
        virtual void getInputInfo(int inputNo, InputInfo& info) = 0;

        //! Pobiera informacje o typach wyjúÊ. KolejnoúÊ obowiπzuje potem w metodzie process.
        //! \param source èrÛd≥o danych. Wejúcia mogπ zawieraÊ obiekty bπdü byÊ wyzerowane.
        virtual void getOutputInfo( std::vector<OutputInfo>& info ) = 0;

        //! Przetwarza informacje.
        virtual void process(IObjectSource* input, IObjectOutput* output) = 0;
    };

    typedef shared_ptr<IDataProcessor> IDataProcessorPtr;
    typedef shared_ptr<const IDataProcessor> IDataProcessorConstPtr;
} // namespace core


#endif  // HEADER_GUARD_CORE__IDATAPROCESSOR_H__