/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:14
	filename: 	IOutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__

#include <set>
#include <core/TypeInfo.h>
#include <map>

namespace core {

//! Interfejs s�u��cy do opisu typ�w danych wyj�ciowcyh oferowanych przez element przetwarzajacy.
class IOutputDescription
{
public:
    //! Struktura opisuj�ca typy danych wyj�ciowe.
    struct OutputInfo
    {
        //! Typ opisuj�cy zale�ne elementy wej�ciowe, kt�re s� wymagane do generowana danego typu wyj�ciowego
        typedef std::set<int> DependentInput;
        //! Nazwa wyj�cia.
        std::string name;
        //! Typ wyj�cia.
        TypeInfo type;
        //! Zale�ne wej�cia
        DependentInput dependentInput;

        //!
        OutputInfo() {}

        //!
        OutputInfo(const std::string& name, const TypeInfo& type, const DependentInput & dependentInput = DependentInput()) :
        name(name), type(type), dependentInput(dependentInput)
        {}

        OutputInfo & operator= (const OutputInfo & other)
        {
            name = other.name;
            type = other.type;
            dependentInput = other.dependentInput;
            return *this;
        }
    };

public:
    //! Pobiera informacje o typach wyj��. Kolejno�� obowi�zuje potem w metodzie process.
    //! \param output Obiekt umozliwiaj�cy zapis danych wyjsciowych - zapis zawsze jest mo�liwy!!
    virtual void getOutputInfo( std::vector<OutputInfo> & output ) = 0;

    virtual ~IOutputDescription() {}
};

}

#endif