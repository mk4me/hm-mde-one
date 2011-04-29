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
    //! Interfejs elementu przetwarzaj�cego dane.
    class IDataProcessor
    {
    public:
        static const int maxNumSources = 16;
        //! Struktura opisuj�ca dane wej�ciowe.
        struct InputInfo
        {
            //! Nazwa wej�cia.
            std::string name;
            //! Typy wspierane przez wej�cie.
            std::list<core::TypeInfo> types;
        };
        //! Struktura opisuj�ca dane wyj�ciowe.
        struct OutputInfo
        {
            //! Nazwa wyj�cia.
            std::string name;
            //! Typ wyj�cia.
            core::TypeInfo type;
            //!
            OutputInfo(const std::string& str, const core::TypeInfo& type) :
            name(name), type(type)
            {}
        };

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IDataProcessor() {}

        //! Pobiera informacje o wej�ciu. Gdy danego wej�cia nie ma wynikowa lista typ�w powinna
        //! by� pusta.
        //! \param inputNo Numer wej�cia. 
        //! \param name Nazwa wej�cia.
        //! \param types Typy wspierane na danym wej�ciu.
        virtual void getInputInfo(int inputNo, InputInfo& info) = 0;

        //! Pobiera informacje o typach wyj��. Kolejno�� obowi�zuje potem w metodzie process.
        //! \param source �r�d�o danych. Wej�cia mog� zawiera� obiekty b�d� by� wyzerowane.
        virtual void getOutputInfo( std::vector<OutputInfo>& info ) = 0;

        //! Przetwarza informacje.
        virtual void process(IObjectSource* input, IObjectOutput* output) = 0;
    };

    typedef shared_ptr<IDataProcessor> IDataProcessorPtr;
    typedef shared_ptr<const IDataProcessor> IDataProcessorConstPtr;
} // namespace core


#endif  // HEADER_GUARD_CORE__IDATAPROCESSOR_H__