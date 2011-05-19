/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:14
	filename: 	IOutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__

#include <vector>

namespace core {

class IOutputDescription
{
public:
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
    //! Pobiera informacje o typach wyjúÊ. KolejnoúÊ obowiπzuje potem w metodzie process.
    //! \param source èrÛd≥o danych. Wejúcia mogπ zawieraÊ obiekty bπdü byÊ wyzerowane.
    virtual void getOutputInfo( std::vector<OutputInfo>& info ) = 0;

    virtual ~IOutputDescription() {}
};

}

#endif