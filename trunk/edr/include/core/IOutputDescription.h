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

class IOutputDescription
{
public:
    //! Struktura opisujπca typy danych wyjúciowe.
    struct OutputInfo
    {
        typedef std::set<int> DependentInput;
        //! Nazwa wyjúcia.
        std::string name;
        //! Typ wyjúcia.
        TypeInfo type;
        //!
        DependentInput dependentInput;

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
    //! Pobiera informacje o typach wyjúÊ. KolejnoúÊ obowiπzuje potem w metodzie process.
    //! \param source èrÛd≥o danych. Wejúcia mogπ zawieraÊ obiekty bπdü byÊ wyzerowane.
    virtual void getOutputInfo( std::vector<OutputInfo> & output ) = 0;

    virtual ~IOutputDescription() {}
};

}

#endif